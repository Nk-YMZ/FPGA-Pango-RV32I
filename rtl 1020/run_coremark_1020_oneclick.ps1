param(
    # Optional check only. The iteration count is already compiled into the HEX.
    [ValidateRange(0,2147483647)][int]$Iterations = 0,
    [ValidateRange(1,2000000000)][int]$TimeoutCycles = 100000000,
    [string]$HexFile = 'D:\1014\coremark.hex',
    [string]$Verilator = 'D:\msys64\ucrt64\bin\verilator_bin.exe',
    [string]$VerilatorRoot = 'D:\msys64\ucrt64\share\verilator',
    [string]$Cxx = 'D:\MinGW\MinGW\bin\g++.exe',
    [string]$RtlDir = $(if (Test-Path -LiteralPath (Join-Path $PSScriptRoot 'core.v')) { $PSScriptRoot } else { 'D:\repo_main\rtl 1020' }),
    [string]$TbFile = '',
    [string]$WorkRoot = (Join-Path $PSScriptRoot 'coremark_runs')
)
$ErrorActionPreference = 'Stop'
# Simulation only: no RISC-V compiler, objcopy, bin2hex, synthesis or board tool.
# Each invocation gets a new directory. No previous files are deleted.
foreach ($required in @($HexFile,$Verilator,$Cxx)) {
    if (-not (Test-Path -LiteralPath $required -PathType Leaf)) {
        throw "Required file missing: $required"
    }
}
$HexFile = (Resolve-Path -LiteralPath $HexFile).ProviderPath
$RtlDir = (Resolve-Path -LiteralPath $RtlDir).ProviderPath
$Verilator = (Resolve-Path -LiteralPath $Verilator).ProviderPath
$Cxx = (Resolve-Path -LiteralPath $Cxx).ProviderPath
$VerilatorRoot = (Resolve-Path -LiteralPath $VerilatorRoot).ProviderPath
if ([string]::IsNullOrWhiteSpace($TbFile)) {
    $candidates = @(
        'D:\1014\fixed_project\source\tb\tb_soc.sv',
        'D:\1014\fixed_project\tb\tb_soc.sv',
        'D:\1014\tb\tb_soc.sv',
        'D:\1010\tb\tb_soc.sv'
    )
    $TbFile = $candidates | Where-Object { Test-Path -LiteralPath $_ -PathType Leaf } |
        Select-Object -First 1
    if (-not $TbFile) { throw 'No tb_soc.sv was found. Specify -TbFile.' }
}
$TbFile = (Resolve-Path -LiteralPath $TbFile).ProviderPath
$rtlModules = @('alu','muldiv','regfile','decoder','bht','btb','ras','csr_file',
    'core','imem','dmem','uart','gpio','cyclectr','mtimer','soc_top','uart_mon')
$rtlOriginal = @($rtlModules | ForEach-Object { Join-Path $RtlDir ($_+'.v') })
foreach ($file in $rtlOriginal) {
    if (-not (Test-Path -LiteralPath $file -PathType Leaf)) { throw "RTL missing: $file" }
}
$hexHash = (Get-FileHash -LiteralPath $HexFile -Algorithm SHA256).Hash
$runName = (Get-Date -Format 'yyyyMMdd_HHmmss_fff') + '_' + [guid]::NewGuid().ToString('N')
$runRoot = Join-Path (Join-Path $WorkRoot 'existing_hex_runs') $runName
New-Item -ItemType Directory -Path $runRoot -ErrorAction Stop | Out-Null
$runRoot = (Resolve-Path -LiteralPath $runRoot).ProviderPath
$build = Join-Path $runRoot 'build'
$result = Join-Path $runRoot 'logs'
$rtlCopy = Join-Path $build 'rtl'
foreach ($folder in @($build,$result,$rtlCopy)) {
    New-Item -ItemType Directory -Path $folder -ErrorAction Stop | Out-Null
}
$oldPath = $env:PATH
$oldVerilatorRoot = $env:VERILATOR_ROOT
$commands = [Collections.Generic.List[object]]::new()
$record = [ordered]@{
    status='RUNNING'; mode='existing-hex'; firmware_compiled=$false;
    firmware_file=$HexFile; firmware_sha256=$hexHash;
    requested_iterations=$Iterations; timeout_cycles=$TimeoutCycles;
    rtl_directory=$RtlDir; testbench=$TbFile; rtl_modified=$false;
    simulation_rtl_directory=$rtlCopy; initialization_redirects=@();
    build=$build; logs=$result; official_duration_valid=$false;
    score_assumption='Total ticks are CPU clock cycles, as in the original port'
}
function Invoke-Logged(
    [string]$Executable, [string[]]$Arguments, [string]$Cwd, [string]$LogName
) {
    $log = Join-Path $result $LogName
    $commands.Add([ordered]@{executable=$Executable; arguments=$Arguments; cwd=$Cwd; log=$log})
    Push-Location -LiteralPath $Cwd
    $oldPreference = $ErrorActionPreference
    try {
        $ErrorActionPreference = 'Continue'
        & $Executable @Arguments > $log 2>&1
        $rc = $LASTEXITCODE
    } finally {
        $ErrorActionPreference = $oldPreference
        Pop-Location
    }
    if ($rc -ne 0) {
        $tail = (Get-Content -LiteralPath $log -Tail 16) -join "`n"
        throw "Process failed ($rc): $LogName`n$tail"
    }
}
try {
    Write-Host "HEX (read only): $HexFile" -ForegroundColor Cyan
    Write-Host "SHA256: $hexHash"
    Write-Host "RTL: $RtlDir"
    Write-Host "Testbench: $TbFile"
    Write-Host "Logs: $result"
    $env:PATH = (Split-Path -Parent $Cxx) + ';' + $oldPath
    $env:VERILATOR_ROOT = $VerilatorRoot
    $record.rtl_hashes = @($rtlOriginal | ForEach-Object {
        [ordered]@{file=$_;sha256=(Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash}
    })
    # Work on private copies only. Some current RTL initializes dmem from
    # firmware.hex even under Verilator. Redirect those loads to the SAME HEX
    # that the testbench loads, avoiding conflicting time-zero initialization.
    $hexSv = $HexFile.Replace('\','/').Replace('"','\"')
    $readmemPattern = '(\$readmemh\s*\(\s*)"[^"\r\n]*firmware\.hex"'
    $rtl = @()
    foreach ($file in $rtlOriginal) {
        $destination = Join-Path $rtlCopy (Split-Path -Leaf $file)
        $text = [IO.File]::ReadAllText($file)
        if ([regex]::IsMatch($text,$readmemPattern)) {
            $text = [regex]::Replace($text,$readmemPattern,
                [System.Text.RegularExpressions.MatchEvaluator]{ param($m)
                    $m.Groups[1].Value + '"' + $hexSv + '"'
                })
            [IO.File]::WriteAllText($destination,$text,[Text.UTF8Encoding]::new($false))
            $record.initialization_redirects += (Split-Path -Leaf $file)
        } else {
            Copy-Item -LiteralPath $file -Destination $destination -ErrorAction Stop
        }
        $rtl += $destination
    }
    # 仿真备用库按缺失模块加载，不直接加入 RTL 列表，避免与已有定义冲突。
    # 模型仅覆盖本项目寄存器堆的初值与使能写入；不模拟芯片全局复位和器件时序。
    $simPrimitiveDir = Join-Path $build 'sim_primitives'
    New-Item -ItemType Directory -Path $simPrimitiveDir -ErrorAction Stop | Out-Null
    $dffModelFile = Join-Path $simPrimitiveDir 'GTP_DFF_E.v'
    $dffModel = @'
`timescale 1ns/1ps
`default_nettype none
module GTP_DFF_E #(
    /* verilator lint_off UNUSEDPARAM */
    parameter GRS_EN = "TRUE",
    /* verilator lint_on UNUSEDPARAM */
    parameter INIT = 1'b0
) (
    output reg Q,
    input wire D,
    input wire CLK,
    input wire CE
);
    initial Q = INIT;
    always @(posedge CLK) begin
        if (CE)
            Q <= D;
    end
endmodule
`default_nettype wire
'@
    [IO.File]::WriteAllText($dffModelFile,($dffModel + "`n"),[Text.UTF8Encoding]::new($false))
    $record.simulation_primitive_library = [ordered]@{
        directory=$simPrimitiveDir; resolution='missing-modules-only';
        models=@('GTP_DFF_E'); global_reset_modeled=$false; device_timing_modeled=$false;
        sha256=(Get-FileHash -LiteralPath $dffModelFile -Algorithm SHA256).Hash
    }
    $tbCopy = Join-Path $build 'tb_soc.sv'
    # The converted .v DUT contains inlined package definitions. Adapt only
    # the private testbench copy; halt_code_t remains an unsigned 3-bit type.
    $tbText = [IO.File]::ReadAllText($TbFile)
    $tbText = [regex]::Replace($tbText, 'import\s+core_pkg::\*\s*;', 'typedef logic [2:0] halt_code_t;')
    [IO.File]::WriteAllText($tbCopy,$tbText,[Text.UTF8Encoding]::new($false))
    Write-Host '[1/2] Building simulator from current RTL...' -ForegroundColor Cyan
    $objDir = Join-Path $build 'obj_soc'
    $argsV = @('--cc','--main','--timing','-Wall','-Wno-DECLFILENAME',
        '-Wno-fatal','--timescale','1ns/1ps','--Mdir',$objDir,'--top-module','tb_soc',
        '-y',$simPrimitiveDir) +
        $rtl + @($tbCopy)
    Invoke-Logged $Verilator $argsV $build 'verilator.log'
    $cpp = @(Get-ChildItem -LiteralPath $objDir -Filter '*.cpp' | Where-Object {
        $_.Name -notlike '*__ALL.cpp'
    } | ForEach-Object FullName)
    $cpp += @((Join-Path $VerilatorRoot 'include\verilated.cpp'),
        (Join-Path $VerilatorRoot 'include\verilated_timing.cpp'),
        (Join-Path $VerilatorRoot 'include\verilated_threads.cpp'))
    $sim = Join-Path $objDir 'tb_soc.exe'
    $argsC = @('-std=c++20','-O2','-fcoroutines','-DVL_TIME_CONTEXT',
        '-DVM_TIMING=1','-DVM_TRACE=0','-DVM_TRACE_FST=0',
        ('-I'+$objDir),('-I'+(Join-Path $VerilatorRoot 'include')),
        ('-I'+(Join-Path $VerilatorRoot 'include\vltstd'))) + $cpp + @('-o',$sim)
    Invoke-Logged $Cxx $argsC $build 'simulator_cxx.log'
    if ((Get-FileHash -LiteralPath $HexFile -Algorithm SHA256).Hash -ne $hexHash) {
        throw 'Input HEX changed during simulator build. Run again with a stable HEX.'
    }
    Write-Host '[2/2] Running the existing HEX (no firmware compilation)...' -ForegroundColor Cyan
    Invoke-Logged $sim @(('+HEX='+$HexFile.Replace('\','/')),
        '+EXPECT=1',('+TIMEOUT='+$TimeoutCycles)) $build 'run.log'
    $logText = Get-Content -LiteralPath (Join-Path $result 'run.log') -Raw
    if ($logText -notmatch 'Correct operation validated' -or
        $logText -notmatch 'tb_soc: PASS' -or $logText -match 'Errors detected|ERROR!') {
        throw 'CoreMark validation or normal halt failed; inspect run.log'
    }
    $ticksMatch = [regex]::Match($logText,'Total ticks\s*:\s*(\d+)')
    $iterationsMatch = [regex]::Match($logText,'Iterations\s*:\s*(\d+)')
    if (-not $ticksMatch.Success -or -not $iterationsMatch.Success) {
        throw 'Missing ticks or iterations in HEX output; inspect run.log'
    }
    $ticks = [long]$ticksMatch.Groups[1].Value
    $actualIterations = [int]$iterationsMatch.Groups[1].Value
    if ($ticks -le 0 -or $actualIterations -le 0) { throw 'Invalid ticks or iterations' }
    if ($Iterations -gt 0 -and $actualIterations -ne $Iterations) {
        throw "HEX reports $actualIterations iterations; expected $Iterations. -Iterations only checks; it cannot change an existing HEX."
    }
    # The existing binary performs its own CoreMark CRC validation. Do not
    # impose the old binary's seed/CRC constants on a different valid binary.
    $record.total_ticks = $ticks
    $record.actual_iterations = $actualIterations
    $record.cycles_per_iter_exact = $ticks / [double]$actualIterations
    $record.equivalent_coremark_per_mhz = 1000000.0 * $actualIterations / $ticks
    $record.validation = 'CoreMark self-validation message and tb_soc PASS'
    $record.status = 'PASS'
    Write-Host ''
    Write-Host '======= CoreMark existing HEX RESULT =======' -ForegroundColor Green
    Write-Host 'Status: PASS (CoreMark self-validation and normal halt)' -ForegroundColor Green
    Write-Host ('Iterations   : {0:N0}' -f $actualIterations)
    Write-Host ('Total ticks  : {0:N0}' -f $ticks)
    Write-Host ('Cycles/Iter  : {0:N2}' -f $record.cycles_per_iter_exact)
    Write-Host ('CoreMark/MHz (estimated): {0:N4}' -f $record.equivalent_coremark_per_mhz)
    Write-Host 'Estimate assumes CPU-cycle ticks from the original port; not official EEMBC scoring.' -ForegroundColor DarkYellow
} catch {
    $record.status = 'FAIL'
    $record.error = $_.Exception.Message
    Write-Error $record.error -ErrorAction Continue
} finally {
    $env:PATH = $oldPath
    $env:VERILATOR_ROOT = $oldVerilatorRoot
    if (Test-Path -LiteralPath $HexFile -PathType Leaf) {
        $record.firmware_sha256_after = (Get-FileHash -LiteralPath $HexFile -Algorithm SHA256).Hash
        $record.firmware_unchanged = ($record.firmware_sha256_after -eq $hexHash)
    } else { $record.firmware_unchanged = $false }
    if (-not $record.firmware_unchanged) {
        $record.status = 'FAIL'
        $record.error = 'Input HEX changed or disappeared during this run.'
        Write-Error $record.error -ErrorAction Continue
    }
    ConvertTo-Json -InputObject @($commands.ToArray()) -Depth 6 |
        Set-Content -LiteralPath (Join-Path $result 'commands.json') -Encoding UTF8
    $record | ConvertTo-Json -Depth 6 |
        Set-Content -LiteralPath (Join-Path $result 'summary.json') -Encoding UTF8
    Write-Host "Logs: $result" -ForegroundColor Cyan
}
if ($record.status -ne 'PASS') { exit 1 }
