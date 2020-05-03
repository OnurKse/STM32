echo off
set arg1=%1
start "" "D:\Program Files (x86)\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility\ST-LINK_CLI.exe" -c SWD -p "%1" -Rst -Run