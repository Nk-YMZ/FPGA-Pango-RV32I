// 振动数据分析与结果输出。每块含一个 16 位序号及 124 个有符号 16 位采样。
// 连续样本组成 1024 点滑动窗口，每新增 124 点执行一次去直流、Hann 加窗、Q15 FFT 和特征计算。
// 信号采样率为 8000 Hz；处理周期通过 VIB_FCLK 换算，幅值与背景扰动以采样值计。
// VIB/VAN 输出指标和状态，SPC 输出相对功率，WAV 输出接收窗口中的原始样本。
#include "vibration.h"
#include "vibration_tables.h"

#ifdef VIB_HOST
#define VIB_IO_SERVICE() ((void)0)
#else
extern void app_vib_tx_service(void);
#define VIB_IO_SERVICE() app_vib_tx_service()
#endif

// real/imag 为 FFT 原位工作区，band_power 保存 64 个显示频组的峰值功率。
// band_lo/hi 单位 Hz，threshold 为千分比，例如 250 表示目标频带占总功率 25%。
static int32_t real[VIB_N], imag[VIB_N];
static uint64_t band_power[VIB_BINS];
static uint32_t band_lo=1000,band_hi=3000,threshold=250;
#ifndef VIB_HOST
static uint32_t rms_limit=8000,crest_limit=40;
static uint32_t noise_limit=1500,reference_hz=500,deviation_limit=150,hold_ms=500;
// raw_previous 保存上一块未经持续确认的条件，observed 保存确认后的活动指标。
// onset/release_at 分别记录出现/解除起点；recovered 与 recovered_at 保存短暂恢复提示。
static uint32_t observed,raw_previous,onset[4],release_at[4],recovered,recovered_at;
static uint32_t spike_at,spike_seen;
#endif
static uint32_t quiet_io;
#ifdef VIB_HOST
int vib_stream_enabled(void) { return 1; }
#endif

static void service_io(void) { if (!quiet_io) VIB_IO_SERVICE(); }

// 无浮点整数平方根，返回不超过真实平方根的最大整数。
// 按从高到低的两位组逐次确定结果，用于 RMS 与背景扰动幅值换算。
static __attribute__((optimize("Os"))) uint32_t root32(uint32_t n)
{
    uint32_t value=0,bit=1u<<30;
    while(bit>n)bit>>=2;
    while(bit){if(n>=value+bit){n-=value+bit;value=(value>>1)+bit;}else value>>=1;bit>>=2;}
    return value;
}

static __attribute__((noinline,optimize("O2"))) void transform(void)
{
    // FFT 先按位反转重排，再执行 10 级基 2 蝶形；每级结果除以 2，总尺度为 1/1024。
    for (uint32_t i=1,j=0;i<VIB_N;i++) {
        if (!(i&63u)) service_io();
        uint32_t bit=VIB_N>>1;
        for (;j&bit;bit>>=1) j^=bit;
        j^=bit;
        if (i<j) {
            int32_t t=real[i];real[i]=real[j];real[j]=t;
            t=imag[i];imag[i]=imag[j];imag[j]=t;
        }
    }
    for (uint32_t length=2;length<=VIB_N;length<<=1) {
        uint32_t half=length>>1, step=VIB_N/length;
        for (uint32_t base=0;base<VIB_N;base+=length) {
            for (uint32_t j=0;j<half;j++) {
                uint32_t u=base+j,v=u+half,w=j*step;
                if (!(u&31u)) service_io();
                int32_t wr=vib_cos[w],wi=vib_sin[w];
                int32_t tr=(int32_t)(((int64_t)wr*real[v]-(int64_t)wi*imag[v])>>15);
                int32_t ti=(int32_t)(((int64_t)wr*imag[v]+(int64_t)wi*real[v])>>15);
                int32_t ar=real[u],ai=imag[u];
                real[u]=(ar+tr)/2;imag[u]=(ai+ti)/2;
                real[v]=(ar-tr)/2;imag[v]=(ai-ti)/2;
            }
        }
    }
}

// 从正频率 k=4,8,...,508 的 127 个功率值取中位数。
// 实部数组在功率扫描后保存这些值；宽带随机背景按中位功率、Hann 窗及 FFT 尺度换算 RMS。
static uint32_t background(void)
{
    uint32_t *p=(uint32_t *)real;int lo=1,hi=127;
    while(lo<hi) {
        uint32_t pivot=p[(lo+hi)/2];int left=lo,i=lo,right=hi;
        while(i<=right) {
            uint32_t t;
            if(p[i]<pivot){t=p[left];p[left++]=p[i];p[i++]=t;}
            else if(p[i]>pivot){t=p[right];p[right--]=p[i];p[i]=t;}
            else i++;
        }
        if(64<left)hi=left-1;else if(64>right)lo=right+1;else return p[64];
    }
    return p[64];
}

__attribute__((optimize("O2"))) void vib_analyze(const int16_t *samples, vib_result_t *result)
{
    int32_t sum = 0;
    for (uint32_t i=0;i<VIB_N;i++) {
        if (!(i&63u)) service_io();
        sum += samples[i];
    }
    // 先计算整窗均值并去直流。RMS、峰值在未加窗的完整去直流信号上计算。
    // FFT 使用缩半后的差值和 Hann 窗，时域指标不使用该窗函数尺度。
    int32_t mean = sum / (int32_t)VIB_N;
    uint64_t squares=0;uint32_t peak_abs=0;
    for (uint32_t i=0;i<VIB_N;i++) {
        if (!(i&31u)) service_io();
        // RMS 与峰值使用完整去直流差值；FFT 输入先除以 2，再乘 Q15 窗系数。
        int32_t delta=samples[i]-mean;uint32_t absolute=delta<0?(uint32_t)-delta:(uint32_t)delta;
        squares+=(uint64_t)((int64_t)delta*delta);if(absolute>peak_abs)peak_abs=absolute;
        real[i] = (int32_t)(((int64_t)(delta/2) * vib_window[i]) >> 15);
        ((volatile int32_t *)imag)[i] = 0; // 逐项写入虚部初值零。
    }
    // RMS = sqrt(去直流样本平方和 / 1024)；peak 为最大绝对差值。
    // 平方和采用 64 位保存，避免满幅样本累积超过 32 位范围。
    result->rms=root32((uint32_t)(squares/VIB_N));result->peak=peak_abs;
    // crest10 为峰值/RMS 的十倍整数；RMS 为零时返回零。
    result->crest10=result->rms?peak_abs*10u/result->rms:0;
    transform();
    for (uint32_t i=0;i<VIB_BINS;i++) ((volatile uint64_t *)band_power)[i]=0;
    // 正频率统计不含直流与 Nyquist 点。目标频带占比按功率求和；
    // 显示的 64 组分别取每 8 个频点中的最大功率，再相对全局峰值归一化。
    uint64_t total=0,high=0,largest=0;
    uint32_t peak=0;
    for (uint32_t k=1;k<VIB_N/2;k++) {
        if (!(k&15u)) service_io();
        uint64_t power=(uint64_t)((int64_t)real[k]*real[k]+(int64_t)imag[k]*imag[k]);
        total+=power;
        if (power>band_power[k>>3]) band_power[k>>3]=power;
        if (k*8000u>=band_lo*VIB_N && k*8000u<=band_hi*VIB_N) high+=power;
        if (power>largest) {largest=power;peak=k;}
        if(!(k&3u))((uint32_t *)real)[k>>2]=(uint32_t)power;
    }
    result->peak_bin=peak;
    result->peak_hz=peak*8000u/VIB_N;
    result->high_permille=total?(uint32_t)(high*1000u/total):0;
    result->alarm=result->high_permille>=threshold;
    // 背景扰动取 127 个均匀频点的功率中位数并按固定系数换算到输入 RMS 尺度。
    // 此值反映宽带随机成分，窄带主峰对中位数的影响与其对总 RMS 的影响不同。
    uint64_t variance=(uint64_t)background()*15759u;
    result->noise=root32(variance>0xffffffffu?0xffffffffu:(uint32_t)variance);
    for (uint32_t i=0;i<VIB_BINS;i++) {
        if (!(i&3u)) service_io();
        result->spectrum[i]=largest?(uint16_t)(band_power[i]*65535u/largest):0;
    }
}

#ifndef VIB_HOST
extern void app_put_str(const char *s);
extern void app_put_dec(uint32_t n);
extern uint32_t app_vib_tx_room(void);
#define VIB_CLOCK (*(volatile uint32_t *)0x10002000u)
#define VIB_QUEUE 4u
// ring 保存最近 1024 点，write_at 指向下次写入位置；filled 为当前连续有效点数。
// ordered 按时间从旧到新展开完整窗口，pending 最多暂存四个待处理的 124 点块。
static int16_t ring[VIB_N], ordered[VIB_N], pending[VIB_QUEUE][VIB_SAMPLES];
// pending_read/write 是单调队列计数，下标用低位掩码回绕。
// 每个槽位的 sequence 与该槽位 124 个采样点同时发布。
static uint32_t pending_sequence[VIB_QUEUE],pending_read,pending_write;
static uint32_t write_at,filled,ready,sequence,previous,have_previous;
// gaps 为输入序号不连续次数，dropped 为待分析队列满导致的新块丢失数。
// blocks 仅统计完整窗口已执行的分析次数，cycles_max 保存当前模式下最大处理周期。
static uint32_t gaps,dropped,blocks,cycles_max;
static uint32_t plot_last,plot_sent;
static uint32_t wave_last,wave_sent;
static uint32_t report_last,report_sent;
// stream_mode 控制是否接收主动发送的采样格式；组窗和 FFT 算法两种传输方式共用。
// trend 为频带占比的平滑参考，above_ms 为本次持续达到频带阈值的时间。
static uint32_t stream_mode,above_since,was_above,trend,trend_valid;
static uint32_t above_ms,above_remainder;

int vib_stream_enabled(void) { return stream_mode!=0; }

// 振动命令采用十进制整数参数；全部参数验证通过后统一更新。
static int number(const char **p,uint32_t *value)
{
    uint32_t n=0;const char *s=*p;
    if (*s<'0'||*s>'9') return 0;
    while (*s>='0'&&*s<='9') {n=n*10u+(uint32_t)(*s++-'0');if(n>100000u)return 0;}
    *p=s;*value=n;return 1;
}
__attribute__((optimize("Os"))) int vib_command(const char *s)
{
    if (s[0]!='v'||s[1]!='i'||s[2]!='b'||s[3]!=' ') return 0;
    s+=4;
    if (s[0]=='b'&&s[1]=='e'&&s[2]=='n'&&s[3]=='c'&&s[4]=='h'&&!s[5]) {
        // 重复 32 次完整分析，返回累计、最小和最大周期。
        // 尚无完整窗口时使用确定的周期序列；此命令期间不穿插结果发送。
        vib_result_t r;uint32_t n=32,total=0,min=0xffffffffu,max=0;
        if(filled<VIB_N)for(uint32_t i=0;i<VIB_N;i++)ordered[i]=(int16_t)((i&63u)*400u-12800u);
        quiet_io=1;
        for(uint32_t i=0;i<n;i++) {
            uint32_t start=VIB_CLOCK;vib_analyze(ordered,&r);uint32_t d=VIB_CLOCK-start;
            total+=d;if(d<min)min=d;if(d>max)max=d;
        }
        quiet_io=0;
        app_put_str("BENCH n=");app_put_dec(n);app_put_str(" points=1024 total=");app_put_dec(total);
        app_put_str(" min=");app_put_dec(min);app_put_str(" max=");app_put_dec(max);app_put_str(" hz=");app_put_dec(r.peak_hz);app_put_str("\n");
        return 1;
    }
    // obs 参数依次为参考主频 Hz、允许偏差 Hz、背景扰动阈值采样值。
    // hold 为出现和解除的确认时间 ms；lim 为 RMS 阈值和十倍峰值因子阈值。
    if(s[0]=='o'&&s[1]=='b'&&s[2]=='s'&&s[3]==' ') {
        s+=4;uint32_t f,d,n;
        if(number(&s,&f)&&*s++==' '&&number(&s,&d)&&*s++==' '&&number(&s,&n)&&!*s&&f<=4000u&&d<=4000u&&n<=65535u) {
            reference_hz=f;deviation_limit=d;noise_limit=n;raw_previous=observed=recovered=spike_seen=0;
            app_put_str("OK vib obs\n");return 1;
        }
    } else if(s[0]=='h'&&s[1]=='o'&&s[2]=='l'&&s[3]=='d'&&s[4]==' ') {
        s+=5;uint32_t n;
        if(number(&s,&n)&&!*s&&n<=5000u){hold_ms=n;raw_previous=observed=recovered=spike_seen=0;app_put_str("OK vib hold\n");return 1;}
    } else if(s[0]=='l'&&s[1]=='i'&&s[2]=='m'&&s[3]==' ') {
        s+=4;uint32_t r,c;
        if(number(&s,&r)&&*s++==' '&&number(&s,&c)&&!*s&&r<=65535u&&c<=200u) {
            rms_limit=r;crest_limit=c;app_put_str("OK vib lim\n");return 1;
        }
    } else if(s[0]=='m'&&s[1]=='o'&&s[2]=='d'&&s[3]=='e'&&s[4]==' ') {
        s+=5;uint32_t mode;
        if(number(&s,&mode)&&!*s&&mode<=1u) {
            stream_mode=mode;filled=ready=have_previous=pending_read=pending_write=0;gaps=dropped=blocks=cycles_max=0;
            plot_sent=wave_sent=report_sent=was_above=trend_valid=0;raw_previous=observed=recovered=spike_seen=0;
            app_put_str("OK vib mode=");app_put_dec(mode);app_put_str("\n");return 1;
        }
    // band 参数为下限 Hz、上限 Hz 和千分比阈值，范围为 0≤下限<上限≤4000。
    // 修改该配置重置频带持续状态及平滑趋势，不清接收样本。
    } else if (s[0]=='b'&&s[1]=='a'&&s[2]=='n'&&s[3]=='d'&&s[4]==' ') {
        s+=5;uint32_t lo,hi,t;
        if(number(&s,&lo)&&*s++==' '&&number(&s,&hi)&&*s++==' '&&number(&s,&t)&&!*s&&lo<hi&&hi<=4000u&&t<=1000u) {
            band_lo=lo;band_hi=hi;threshold=t;was_above=trend_valid=0;
            app_put_str("OK vib band=");app_put_dec(lo);app_put_str(" hi=");app_put_dec(hi);app_put_str(" threshold=");app_put_dec(t);app_put_str("\n");return 1;
        }
    }
    app_put_str("ERR vib-arg\n");return 1;
}

// 每笔 125 个寄存器：序号在前，随后为 124 点大端样本；队列满时丢弃新块并计数。
void app_vib_samples(uint8_t addr,const uint8_t *data,uint32_t registers)
{
    if (addr!=1u || registers!=125u) return;
    if (pending_write-pending_read>=VIB_QUEUE) {dropped++;return;}
    uint32_t slot=pending_write&(VIB_QUEUE-1u);
    pending_sequence[slot]=((uint32_t)data[0]<<8)|data[1];
    for (uint32_t i=0;i<VIB_SAMPLES;i++)
        pending[slot][i]=(int16_t)(((uint32_t)data[2+2*i]<<8)|data[3+2*i]);
    // 序号与样本全部写完后发布队列写计数；处理函数只消费此前完整发布的槽位。
    pending_write++;
}

__attribute__((optimize("Os"))) void vib_service(void)
{
    if (pending_read==pending_write) return;
    // 处理计时从取待分析块开始，包含本块入窗、展开窗口和全部特征计算。
    // 后续阈值状态更新与串口报告生成位于 elapsed 记录之后，不计入本块 cycles。
    uint32_t begin=VIB_CLOCK;
    uint32_t slot=pending_read&(VIB_QUEUE-1u);sequence=pending_sequence[slot];
    // 序号按 16 位循环递增；不连续时累计断档并重新填满分析窗。
    if (have_previous && sequence!=((previous+1u)&65535u)) {gaps++;filled=0;}
    previous=sequence;have_previous=1;
    for (uint32_t i=0;i<VIB_SAMPLES;i++) {
        ring[write_at]=pending[slot][i];write_at=(write_at+1u)&(VIB_N-1u);
        if (filled<VIB_N) filled++;
    }
    pending_read++;
    if (filled<VIB_N) return;
    // 1024 点填满后，每新增 124 点都分析最近完整一窗，两次窗口重叠 900 点。
    // 信号时间跨度为 1024/8000=128 ms；这与现实输入块到达间隔分别计量。
    for (uint32_t i=0;i<VIB_N;i++) ordered[i]=ring[(write_at+i)&(VIB_N-1u)];
    vib_result_t r;
    vib_analyze(ordered,&r);
    uint32_t elapsed=VIB_CLOCK-begin;
    uint32_t now=VIB_CLOCK;
    uint32_t deviation=r.peak_hz>reference_hz?r.peak_hz-reference_hz:reference_hz-r.peak_hz;
    // met 位域：bit0 RMS、bit1 尖峰因子、bit2 背景扰动、bit3 主峰偏移。
    // 阈值为零关闭对应判定；尖峰与偏移还要求 RMS 至少为 100。
    uint32_t raw=(rms_limit&&r.rms>=rms_limit?1u:0u)|(crest_limit&&r.rms>=100u&&r.crest10>=crest_limit?2u:0u)
        |(noise_limit&&r.noise>=noise_limit?4u:0u)|(deviation_limit&&r.rms>=100u&&deviation>deviation_limit?8u:0u);
    uint32_t active=0;
    for(uint32_t i=0;i<4;i++) {
        uint32_t bit=1u<<i;
        // 尖峰判定立即生效并保留 1 秒；其他指标的发生与恢复按 hold_ms 确认。
        if(bit==2u) {
            if(raw&bit){spike_at=now;spike_seen=1;}
            if(spike_seen&&(uint32_t)(now-spike_at)<VIB_FCLK)active|=bit;
            else spike_seen=0;
            continue;
        }
        if(!(raw_previous&bit))onset[i]=now;
        if(raw_previous&bit)release_at[i]=now;
        if(raw&bit) {
            if((observed&bit)||(uint32_t)(now-onset[i])/(VIB_FCLK/1000u)>=hold_ms)active|=bit;
        } else if((observed&bit)&&(uint32_t)(now-release_at[i])/(VIB_FCLK/1000u)<hold_ms)active|=bit;
    }
    uint32_t cleared=observed&~active;
    if(cleared){recovered|=cleared;recovered_at=now;}
    if((uint32_t)(now-recovered_at)>VIB_FCLK)recovered=0;
    recovered&=~active;observed=active;raw_previous=raw;
    // 趋势比较当前频带千分比与平滑参考，差值超过 20 才记增强或减弱。
    // 参考值每块按 7/8 旧值加 1/8 当前值更新，趋势不代表设备故障分类。
    uint32_t direction=trend_valid?(r.high_permille>trend+20u?1u:trend>r.high_permille+20u?2u:0u):0;
    if(!trend_valid){trend=r.high_permille;trend_valid=1;}else trend=(trend*7u+r.high_permille)/8u;
    if(r.alarm) {
        if(!was_above){above_ms=above_remainder=0;above_since=now;}
        else {
            uint32_t d=(uint32_t)(now-above_since)+above_remainder;
            above_ms+=d/(VIB_FCLK/1000u);above_remainder=d%(VIB_FCLK/1000u);above_since=now;
        }
    }
    uint32_t duration=r.alarm?above_ms:0;
    uint32_t state=r.alarm?(duration>=500u?2u:1u):(was_above?3u:0u);
    was_above=r.alarm;
    if (elapsed>cycles_max) cycles_max=elapsed;
    blocks++;
    // VIB/VAN 最多每秒输出 15 组；每个满窗后的 124 点新块均执行分析。
    if(!report_sent||(uint32_t)(now-report_last)>=VIB_FCLK/15u) {
    report_sent=1;report_last=now;
    // VIB：序号、累计分析块数、主峰 Hz、频带千分比、频带阈值状态、处理周期，
    // 以及队列丢块 qdrop、序号断档 gaps 和 CPU 频率 fclk。
    app_put_str("VIB seq=");app_put_dec(sequence);
    app_put_str(" blocks=");app_put_dec(blocks);
    app_put_str(" hz=");app_put_dec(r.peak_hz);
    app_put_str(" hf_pm=");app_put_dec(r.high_permille);
    app_put_str(" alarm=");app_put_dec(r.alarm);
    app_put_str(" cycles=");app_put_dec(elapsed);
    app_put_str(" max=");app_put_dec(cycles_max);
    app_put_str(" qdrop=");app_put_dec(dropped);
    app_put_str(" gaps=");app_put_dec(gaps);app_put_str(" fclk=");app_put_dec(VIB_FCLK);app_put_str("\n");
    // VAN 与 VIB 使用同一块序号：给出频带、持续状态、RMS/峰值/峰值因子、
    // 确认后的 met 位域、背景扰动、各判定阈值及恢复位域 rec。
    app_put_str("VAN seq=");app_put_dec(sequence);
    app_put_str(" lo=");app_put_dec(band_lo);app_put_str(" hi=");app_put_dec(band_hi);
    app_put_str(" trend=");app_put_dec(direction);app_put_str(" state=");app_put_dec(state);
    app_put_str(" ms=");app_put_dec(duration);app_put_str(" threshold=");app_put_dec(threshold);
    app_put_str(" rms=");app_put_dec(r.rms);app_put_str(" peak=");app_put_dec(r.peak);
    app_put_str(" cf=");app_put_dec(r.crest10);app_put_str(" lr=");app_put_dec(rms_limit);
    app_put_str(" lc=");app_put_dec(crest_limit);app_put_str(" met=");
    app_put_dec(active);app_put_str(" noise=");app_put_dec(r.noise);app_put_str(" ln=");app_put_dec(noise_limit);
    app_put_str(" ref=");app_put_dec(reference_hz);app_put_str(" tol=");app_put_dec(deviation_limit);
    app_put_str(" hold=");app_put_dec(hold_ms);app_put_str(" rec=");app_put_dec(recovered);app_put_str("\n");
    }
    // SPC 最多每秒 15 组，每组分两行、每行 32 个值，序号相同。
    // 相对功率取值 0..65535；整组发送前检查队列空间，不足时顺延。
    if ((!plot_sent || (uint32_t)(now-plot_last)>=VIB_FCLK/15u) && app_vib_tx_room()>=480u) {
        plot_last=now;plot_sent=1;
        // SPC 每个频组覆盖 8 个 FFT 频点，即 62.5 Hz；数值是组内峰值相对全局峰值的功率比例。
        // 64 个值不是 64 个频段的能量总和；频带能量占比由逐 FFT 频点功率另外求和。
        for (uint32_t part=0;part<2u;part++) {
            app_put_str("SPC seq=");app_put_dec(sequence);
            app_put_str(" part=");app_put_dec(part);app_put_str(" scale=65535 data=");
            for (uint32_t i=0;i<32u;i++) {
                if (i) app_put_str(",");
                app_put_dec(r.spectrum[part*32u+i]);
            }
            app_put_str("\n");
        }
    }
    // WAV 最多每秒 10 组，取窗口最后 64 个连续样本，对应 8 ms 信号时间。
    // 每个有符号样本加 32768 后以十进制无符号值发送，两行共用同一序号。
    if ((!wave_sent || (uint32_t)(now-wave_last)>=VIB_FCLK/10u) && app_vib_tx_room()>=448u) {
        wave_last=now;wave_sent=1;
        for(uint32_t part=0;part<2u;part++) {
            app_put_str("WAV seq=");app_put_dec(sequence);app_put_str(" part=");app_put_dec(part);app_put_str(" data=");
            for(uint32_t i=0;i<32u;i++) {
                if(i)app_put_str(",");
                app_put_dec((uint32_t)((int32_t)ordered[VIB_N-64u+part*32u+i]+32768));
            }
            app_put_str("\n");
        }
    }
}
#endif
