param(
    [ValidateRange(1,1500)][int]$Iterations = 10,
    [ValidateSet('v1.01','local-main')][string]$CoreMarkSource = 'v1.01',
    [ValidateRange(1,2000000000)][int]$TimeoutCycles = 100000000,
    [string]$GccRoot = 'D:\gcc\xpack-riscv-none-elf-gcc-14.2.0-3\bin',
    [string]$Verilator = 'D:\msys64\ucrt64\bin\verilator_bin.exe',
    [string]$VerilatorRoot = 'D:\msys64\ucrt64\share\verilator',
    [string]$Cxx = 'D:\MinGW\MinGW\bin\g++.exe',
    [string]$Python = 'D:\anaconda\python.exe',
    [string]$RtlDir = 'D:\1014\fixed_project\source\rtl',
    [string]$TbFile = '',
    [string]$PackageRoot = 'C:\Users\szy\Documents\Codex\2026-10-13\d-download-fpga-pango-rv32i-rv32i\outputs\coremark-reproduction',
    [string]$WorkRoot = 'D:\1014\coremark_quick'
)
$ErrorActionPreference = 'Stop'
# Use the verified package for software, startup code, linker script and utilities.
# RTL and testbench come directly from the user's *current* PDS project.
$source = Join-Path $PackageRoot 'source'
$coremark = Join-Path $PackageRoot $(if($CoreMarkSource -eq 'v1.01'){'coremark-1.01'}else{'coremark-local-main'})
$build = Join-Path $WorkRoot 'build'
$result = Join-Path $WorkRoot 'logs'
$lastScoreFile = Join-Path $WorkRoot 'last_score.json'
$marker = Join-Path $WorkRoot '.coremark_quick_workspace'

# Choose a testbench beside the current fixed project when available.
# Fall back to the previously verified 1010 testbench. -TbFile can override.
if ([string]::IsNullOrWhiteSpace($TbFile)) {
    $candidates = @(
        'D:\1014\fixed_project\source\tb\tb_soc.sv',
        'D:\1014\fixed_project\tb\tb_soc.sv',
        'D:\1010\tb\tb_soc.sv'
    )
    $TbFile = @($candidates | Where-Object { Test-Path -LiteralPath $_ -PathType Leaf } |
        Select-Object -First 1) | Select-Object -First 1
    if (-not $TbFile) {
        throw 'No tb_soc.sv was found. Specify its full path using -TbFile.'
    }
}

# Prevent accidental deletion of someone else's folder. Only our fixed build/log
# subdirectories are cleaned, never any original RTL, testbench or CoreMark files.
if (Test-Path -LiteralPath $WorkRoot) {
    if (-not (Test-Path -LiteralPath $marker)) {
        throw "Existing work folder has no CoreMark marker, will not overwrite: $WorkRoot"
    }
} else {
    New-Item -ItemType Directory -Path $WorkRoot -Force | Out-Null
    Set-Content -LiteralPath $marker -Value 'CoreMark one-click reusable workspace' -Encoding ASCII
}
foreach ($tmp in @($build, $result)) {
    if (Test-Path -LiteralPath $tmp) { Remove-Item -LiteralPath $tmp -Recurse -Force }
    New-Item -ItemType Directory -Path $tmp -Force | Out-Null
}
$previousScore = $null
if (Test-Path -LiteralPath $lastScoreFile) {
    try { $previousScore = Get-Content -LiteralPath $lastScoreFile -Raw | ConvertFrom-Json }
    catch { Write-Warning 'Cannot read previous score; comparison skipped.' }
}

$gcc = Join-Path $GccRoot 'riscv-none-elf-gcc.exe'
$objcopy = Join-Path $GccRoot 'riscv-none-elf-objcopy.exe'
$objdump = Join-Path $GccRoot 'riscv-none-elf-objdump.exe'
$oldPath = $env:PATH
$oldVerilatorRoot = $env:VERILATOR_ROOT
$commands = [Collections.Generic.List[object]]::new()
$record = [ordered]@{
    status='RUNNING'; iterations=$Iterations; tick_unit='CPU clock cycles';
    ticks_per_second_in_port=1; official_duration_valid=$false;
    source=$source; rtl_directory=$RtlDir; testbench=$TbFile;
    coremark_source=$CoreMarkSource; coremark_directory=$coremark; build=$build; logs=$result;
    adaptations=@('Use installed Windows tool executables',
        'Compile simulator through native C++ driver; waveform tracing disabled',
        'Build CoreMark objects separately with original port flags and link order',
        'Short compiler display string avoids the original ee_printf 256-byte buffer overflow; full arguments recorded separately');
    rtl_modified=$false
}
function Invoke-Logged(
    [string]$Executable,
    [string[]]$Arguments,
    [string]$Cwd,
    [string]$LogName
) {
    $log = Join-Path $result $LogName

    $commands.Add([ordered]@{
        executable = $Executable
        arguments = $Arguments
        cwd = $Cwd
        log = $log
    })

    Push-Location -LiteralPath $Cwd
    $oldPreference = $ErrorActionPreference

    try {
        # 不让外部程序的普通 stderr 警告终止 PowerShell
        $ErrorActionPreference = 'Continue'

        & $Executable @Arguments > $log 2>&1
        $rc = $LASTEXITCODE
    }
    finally {
        $ErrorActionPreference = $oldPreference
        Pop-Location
    }

    # 真正失败时仍然停止
    if ($rc -ne 0) {
        $tail = (Get-Content -LiteralPath $log -Tail 16) -join "`n"
        throw "Process failed ($rc): $LogName`n$tail"
    }
}
try {
    foreach($tool in @($gcc,$objcopy,$objdump,$Verilator,$Cxx,$Python,$TbFile)) {
        if(-not(Test-Path -LiteralPath $tool -PathType Leaf)){throw "Required file missing: $tool"}
    }
    foreach($folder in @($source,$coremark,$RtlDir)) {
        if(-not(Test-Path -LiteralPath $folder -PathType Container)){throw "Required folder missing: $folder"}
    }
    $rtlModules = @('core_pkg','alu','muldiv','regfile','decoder','bht','ras','csr_file',
        'core','imem','dmem','uart','gpio','cyclectr','mtimer','soc_top')
    $rtl = @($rtlModules | ForEach-Object { Join-Path $RtlDir ($_.ToString()+'.sv') })
    foreach($file in $rtl){if(-not(Test-Path -LiteralPath $file -PathType Leaf)){throw "RTL missing: $file"}}
    foreach($file in @((Join-Path $source 'sw\crt0.S'),(Join-Path $source 'sw\link.ld'),
        (Join-Path $source 'scripts\bin2hex.py'))) {
        if(-not(Test-Path -LiteralPath $file -PathType Leaf)){throw "Software input missing: $file"}
    }
    Write-Host "RTL: $RtlDir" -ForegroundColor Cyan
    Write-Host "Testbench: $TbFile" -ForegroundColor Cyan
    if ($TbFile -eq 'D:\1010\tb\tb_soc.sv') {
        Write-Host 'Note: 1014 has no tb_soc.sv; using the verified 1010 testbench.' -ForegroundColor Yellow
    }
    # Settings affect this process only. CPU RTL and port timing are retained verbatim.
    $env:PATH=(Split-Path -Parent $Cxx)+';'+$oldPath
    $env:VERILATOR_ROOT=$VerilatorRoot
    $record.compiler=(& $gcc --version | Select-Object -First 1)
    $record.verilator=(& $Verilator --version | Out-String).Trim()
    $record.host_cxx=(& $Cxx --version | Select-Object -First 1)
    $record.rtl_hashes = @($rtl | ForEach-Object {
        [ordered]@{file=$_;sha256=(Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash}
    })
    Write-Host '[1/3] Rebuilding simulator from current RTL...' -ForegroundColor Cyan
    $objDir=Join-Path $build 'obj_soc'
    $argsV=@('--cc','--main','--timing','-Wall','-Wno-DECLFILENAME',
        '-Wno-fatal','--timescale','1ns/1ps','--Mdir',$objDir,'--top-module','tb_soc')+$rtl+@($TbFile)
    Invoke-Logged $Verilator $argsV $source 'verilator.log'
    $cpp=@(Get-ChildItem -LiteralPath $objDir -Filter '*.cpp' | Where-Object {
        $_.Name -notlike '*__ALL.cpp'
    } | ForEach-Object FullName)
    $cpp+=@((Join-Path $VerilatorRoot 'include\verilated.cpp'),
        (Join-Path $VerilatorRoot 'include\verilated_timing.cpp'),
        (Join-Path $VerilatorRoot 'include\verilated_threads.cpp'))
    $sim=Join-Path $objDir 'tb_soc.exe'
    $argsC=@('-std=c++20','-O2','-fcoroutines','-DVL_TIME_CONTEXT',
        '-DVM_TIMING=1','-DVM_TRACE=0','-DVM_TRACE_FST=0',
        ('-I'+$objDir),('-I'+(Join-Path $VerilatorRoot 'include')),
        ('-I'+(Join-Path $VerilatorRoot 'include\vltstd')))+$cpp+@('-o',$sim)
    Invoke-Logged $Cxx $argsC $source 'simulator_cxx.log'
    Write-Host "[2/3] Building CoreMark $CoreMarkSource, rv32im -O2, $Iterations iterations..." -ForegroundColor Cyan
    $crt=Join-Path $build 'crt0.o'
    Invoke-Logged $gcc @('-march=rv32im_zicsr','-mabi=ilp32','-nostdlib','-nostartfiles',
        '-c','sw/crt0.S','-o',$crt) $source 'crt0_gcc.log'
    $port=Join-Path $source 'bench\coremark_port'
    $portFlags=@('-O2','-march=rv32im','-mabi=ilp32','-mno-relax','-nostdlib','-ffreestanding')
    $linkScript=(Join-Path $source 'sw\link.ld').Replace('\','/')
    # ee_printf uses an unbounded formatter with a 256-byte local buffer.
    # Windows absolute paths exceed it; keep only the actual compiler flags in the display string.
   $flagText = ($portFlags -join ' ') + ' -DITERATIONS=' + $Iterations + ' -lgcc'

$flagsHeader = Join-Path $build 'coremark_flags.h'
Set-Content -LiteralPath $flagsHeader -Value ('#define FLAGS_STR "' + $flagText + '"') -Encoding ASCII

$cflags = $portFlags + @(('-I'+$port),('-I'+$coremark),'-include',$flagsHeader,('-DITERATIONS='+$Iterations))
    $compileSources=@('core_list_join','core_main','core_matrix','core_state','core_util') | ForEach-Object {
        [ordered]@{name=$_;path=(Join-Path $coremark ($_+'.c'))}
    }
    $compileSources+=@([ordered]@{name='core_portme';path=(Join-Path $port 'core_portme.c')},
        [ordered]@{name='ee_printf';path=(Join-Path $port 'ee_printf.c')})
    $objects=@()
    foreach($item in $compileSources){
        $object=Join-Path $build ($item.name+'.o'); $objects+=@($object)
        Invoke-Logged $gcc ($cflags+@('-c',$item.path,'-o',$object)) $coremark ($item.name+'_gcc.log')
    }
    $elf=Join-Path $result 'coremark.elf'; $bin=Join-Path $build 'coremark.bin'; $hex=Join-Path $result 'coremark.hex'
    Invoke-Logged $gcc (@('-march=rv32im','-mabi=ilp32','-nostdlib',$crt,'-T',$linkScript)+$objects+@('-o',$elf,'-lgcc')) $source 'link_gcc.log'
    Invoke-Logged $objcopy @('-O','binary',$elf,$bin) $source 'objcopy.log'
    Invoke-Logged $Python @('scripts/bin2hex.py',$bin,$hex) $source 'bin2hex.log'
    Invoke-Logged $objdump @('-d',$elf) $source 'firmware_disassembly.txt'
    $record.firmware_sha256=(Get-FileHash -LiteralPath $hex).Hash
    $record.cpu_flags=$portFlags
    $record.ram_bytes=1048576
    $record.timeout_cycles=$TimeoutCycles
    $record.firmware_contains_mul=[bool](Select-String -LiteralPath (Join-Path $result 'firmware_disassembly.txt') -Pattern '\s+mul\s' -Quiet)
    Write-Host '[3/3] Running CoreMark...' -ForegroundColor Cyan
    Invoke-Logged $sim @(('+HEX='+$hex.Replace('\','/')),'+EXPECT=1',('+TIMEOUT='+$TimeoutCycles)) $source 'run.log'
    $logText=Get-Content -LiteralPath (Join-Path $result 'run.log') -Raw
    if($logText -notmatch 'Correct operation validated' -or $logText -notmatch 'tb_soc: PASS' -or
       $logText -match 'Errors detected|ERROR!'){throw 'CoreMark validation or normal halt failed; inspect run.log'}
    $ticksMatch=[regex]::Match($logText,'Total ticks\s*:\s*(\d+)')
    $iterationsMatch=[regex]::Match($logText,'Iterations\s*:\s*(\d+)')
    if(-not $ticksMatch.Success -or -not $iterationsMatch.Success){throw 'Missing ticks or iterations'}
    $ticks=[long]$ticksMatch.Groups[1].Value; $actualIterations=[int]$iterationsMatch.Groups[1].Value
    if($actualIterations -ne $Iterations){throw 'Iteration count differs from configuration'}
    foreach($pair in @(@('seedcrc','e9f5'),@('crclist','e714'),@('crcmatrix','1fd7'),@('crcstate','8e3a'))){
        if($logText -notmatch ($pair[0]+'\s*:\s*0x'+$pair[1]+'\b')){throw "CRC mismatch: $($pair[0])"}
    }
    $record.status='PASS'
    $record.total_ticks=$ticks
    $record.actual_iterations=$actualIterations
    $record.cycles_per_iter_exact=$ticks/[double]$actualIterations
    $record.cycles_per_iter_csv=[math]::Floor($ticks/[double]$actualIterations)
    $record.seconds_at_50mhz=$ticks/50000000.0
    $record.equivalent_coremark_per_mhz=1000000.0*$actualIterations/$ticks
    $record.crc_validated=$true
    $record.exact_historical_match=($actualIterations -eq 10 -and $ticks -eq 3547147)
    if($Iterations -eq 10){$record.delta_cycles_from_history=$ticks-3547147;$record.delta_percent_from_history=100.0*($ticks-3547147)/3547147}
    $cpi = $record.cycles_per_iter_exact
    $cmhz = $record.equivalent_coremark_per_mhz
    Write-Host ''
    Write-Host '======= CoreMark RV32IM RESULT =======' -ForegroundColor Green
    Write-Host 'Status: PASS  (CRC and normal halt validated)' -ForegroundColor Green
    Write-Host ('Iterations   : {0:N0}' -f $actualIterations)
    Write-Host ('Total cycles : {0:N0}' -f $ticks)
    Write-Host ('Cycles/Iter  : {0:N2}' -f $cpi)
    Write-Host ('CoreMark/MHz (estimated): {0:N4}' -f $cmhz)
    if($previousScore -and $previousScore.iterations -eq $actualIterations -and
       $previousScore.coremark_source -eq $CoreMarkSource -and $previousScore.cycles_per_iter -gt 0) {
        $changePct = 100.0*($cpi/[double]$previousScore.cycles_per_iter - 1.0)
        $trend = if($changePct -lt 0){'FASTER'}elseif($changePct -gt 0){'SLOWER'}else{'UNCHANGED'}
        Write-Host ('Vs last PASS : {0} {1:N2}%  (last: {2:N2} cycles/iter)' -f $trend,[math]::Abs($changePct),[double]$previousScore.cycles_per_iter)
    } else {
        Write-Host 'Vs last PASS : none yet (or iterations/source differ)'
    }
    if($actualIterations -eq 10) {
        Write-Host ('Vs verified 2026-10-13 snapshot (385,974 cycles/iter): {0:N2}%' -f (100.0*($cpi/385974.0 - 1.0)))
        Write-Host ('Vs teammate 10-iter record (354,714.7 cycles/iter): {0:N2}%' -f (100.0*($cpi/354714.7 - 1.0)))
    }
    Write-Host 'Note: This port is for cycle comparison, not official EEMBC scoring.' -ForegroundColor DarkYellow
    Write-Host '=====================================' -ForegroundColor Green
    # Store only the latest passing numeric score for the next comparison.
    [ordered]@{
        iterations=$actualIterations; total_cycles=$ticks;
        cycles_per_iter=$cpi; coremark_per_mhz=$cmhz;
        coremark_source=$CoreMarkSource; cpu_isa='rv32im'; compiler=$record.compiler;
        rtl_directory=$RtlDir; measured_at=(Get-Date).ToString('o')
    } | ConvertTo-Json -Depth 3 | Set-Content -LiteralPath $lastScoreFile -Encoding UTF8
} catch {
    $record.status='FAIL'; $record.error=$_.Exception.Message
    Write-Error $record.error -ErrorAction Continue
} finally {
    $commands|ConvertTo-Json -Depth 6|Set-Content -LiteralPath (Join-Path $result 'commands.json') -Encoding UTF8
    $record|ConvertTo-Json -Depth 6|Set-Content -LiteralPath (Join-Path $result 'summary.json') -Encoding UTF8
    $env:PATH=$oldPath; $env:VERILATOR_ROOT=$oldVerilatorRoot
    if($record.status -ne 'PASS'){Write-Host "Failure logs (overwritten next time): $result" -ForegroundColor Yellow}
}
if($record.status -ne 'PASS'){exit 1}
