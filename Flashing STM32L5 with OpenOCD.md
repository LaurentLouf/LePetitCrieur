# Flashing STM32L5 with OpenOCD

Trying to flash the STM32L5 from the official package of OpenOCD doesn't work.

The next solution to try is to use the OpenOCD version that is packaged with STM32CubeIDE. Refer to `Flashing STM32G0x with openOCD.pdf` to see how to setup everything. Unfortunately, it doesn't work out of the box. Trying to flash the STM32L5 -DK gives the following errors :
```
10:53 $ openocd -f openocd_interface_target_stlink.cfg -f openocd_stm32_flash.cfg
Open On-Chip Debugger 0.10.0+dev-g30d1303 (2020-06-18-09:11)
Licensed under GNU GPL v2
For bug reports, read
	http://openocd.org/doc/doxygen/bugs.html
Info : The selected transport took over low-level target control. The results might differ compared to plain JTAG/SWD
Info : clock speed 4000 kHz
Info : STLINK V3J7M2 (API v3) VID:PID 0483:374E
Info : Target voltage: 3.289421
Info : stm32l5.cpu: hardware has 8 breakpoints, 4 watchpoints
CPU in Non-Secure state
Error executing event halted on target stm32l5.cpu:
/opt/st/stm32cubeide_1.4.0/plugins/com.st.stm32cube.ide.mcu.debug.openocd_1.4.0.202007081208/resources/openocd/st_scripts/target/stm32l5x.cfg:127: Error: invalid command name "stm32l5.dap"
in procedure 'mmw' called at file "/opt/st/stm32cubeide_1.4.0/plugins/com.st.stm32cube.ide.mcu.debug.openocd_1.4.0.202007081208/resources/openocd/st_scripts/target/stm32l5x.cfg", line 102
in procedure 'ahb_ap_non_secure_access' called at file "/opt/st/stm32cubeide_1.4.0/plugins/com.st.stm32cube.ide.mcu.debug.openocd_1.4.0.202007081208/resources/openocd/st_scripts/target/stm32l5x.cfg", line 143
at file "/opt/st/stm32cubeide_1.4.0/plugins/com.st.stm32cube.ide.mcu.debug.openocd_1.4.0.202007081208/resources/openocd/st_scripts/target/stm32l5x.cfg", line 127
Info : starting gdb server for stm32l5.cpu on 3333
Info : Listening on port 3333 for gdb connections
CPU in Non-Secure state
Error executing event halted on target stm32l5.cpu:
/opt/st/stm32cubeide_1.4.0/plugins/com.st.stm32cube.ide.mcu.debug.openocd_1.4.0.202007081208/resources/openocd/st_scripts/target/stm32l5x.cfg:127: Error: invalid command name "stm32l5.dap"
in procedure 'ocd_process_reset'
in procedure 'ocd_process_reset_inner' called at file "embedded:startup.tcl", line 298
in procedure 'ahb_ap_non_secure_access' called at file "/opt/st/stm32cubeide_1.4.0/plugins/com.st.stm32cube.ide.mcu.debug.openocd_1.4.0.202007081208/resources/openocd/st_scripts/target/stm32l5x.cfg", line 143
at file "/opt/st/stm32cubeide_1.4.0/plugins/com.st.stm32cube.ide.mcu.debug.openocd_1.4.0.202007081208/resources/openocd/st_scripts/target/stm32l5x.cfg", line 127
target halted due to debug-request, current mode: Thread
xPSR: 0xf9000000 pc: 0xfffffffe msp: 0xfffffffc
Info : Unable to match requested speed 4000 kHz, using 3300 kHz
Info : Unable to match requested speed 4000 kHz, using 3300 kHz
Info : Device id = 0x20016472
Info : STM32L5xx flash size is 512kb, base address is 0x08000000
Error: No working memory available. Specify -work-area-phys to target.
Warn : not enough working area available(requested 82)
Warn : no working area available, can't do block memory writes
Error: No working memory available. Specify -work-area-phys to target.
Warn : not enough working area available(requested 52)
Error: checksum mismatch - attempting binary compare
diff 0 address 0x08000000. Was 0xff instead of 0x00
[...]
diff 127 address 0x0800007f. Was 0xff instead of 0x08
More than 128 errors, the rest are not printed.
```

A quick search online redirect us to a [patch](http://openocd.zylin.com/#/c/5344), containing another version of `stm32l5x.cfg`.

Using meld to spot the differences between the two versions, we see the following :

- The patch has this line that the file in STM32CubeIDE package does not have : `$_TARGETNAME configure -work-area-phys 0x20000000 -work-area-size $_WORKAREASIZE -work-area-backup 0` . Since this relates to work area size which is mentionned in the errors, this line is added to the configuration file (from patch to version coming with STM32CubeIDE)