@echo off
REM Build the application ELF with GCC 14.2 and the 90 MHz firmware configuration.
REM This command produces the application ELF only; merged HEX and bitstream are separate artifacts.
cd /d "%~dp0"
where riscv-none-elf-gcc >nul 2>nul
if errorlevel 1 (
  echo Add the GCC 14.2 RISC-V toolchain bin directory to PATH.
  pause
  exit /b 1
)
if not exist output mkdir output
riscv-none-elf-gcc -DAPP_RSP_PREVIEW_REGS=4 -march=rv32im_zicsr -mabi=ilp32 -nostdlib -nostartfiles -O2 -DMON_FCLK=90000000 -DMON_BITDIV_CFG=9375 -T link.ld crt0.S main.c proto.c -o output\app_mon_80m.elf -lgcc -DAPP_CHUNKED_TX=1 -DMON_FAST_BAUD=1 -DMON_FAST_BAUD_MAX=921600 -DMON_RX_DIAG=1 -DMON_SCAN_MIN_MS=20 -DAPP_VIB_TRIAL=1 vibration.c -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,--no-check-sections
if errorlevel 1 pause
