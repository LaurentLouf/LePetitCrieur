# Power consumption

- [Power consumption](#power-consumption)
  - [Base consumption](#base-consumption)
  - [Consumption with code at commit fc5c498e](#consumption-with-code-at-commit-fc5c498e)

## Base consumption

In that case, the main function is limited to the following code :

```cpp
int main(void) {
  /* USER CODE BEGIN 1 */
  LCD_DeInit();
  while (1) {
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
  }
}
```

In that configuration, the power consumption measured with STM32CubeMonitor-Power is the following
![Power consumption in basic configuration](images/power_base.png)

The average consumption reported is around 560µA. In this configuration, the board is in its reset state since almost nothing has been initialized. It's a good basis for comparison for power consumption measured with the final hardware

## Consumption with code at commit fc5c498e

The power consumption optimization features implemented so far are :

- Enter Sleep mode when there's only the analog watchdog for the microphone running
- Deactivate the LCD module (backlight and LCD driver)
- Turn off all onboard LEDs when going into sleep mode
- Remove all timers since at this point none is used by the firmware
- Disable DMA interrupts for the DFSDM when going into sleep mode

In that configuration, the power consumption measured with STM32CubeMonitor-Power is the following
![Power consumption in basic configuration](images/power_fc5c498e.png)

- The average consumption reported is around 14.5mA
- The active phase (board awake and "saving" samples) oscillates between 14.5mA and 16.5mA
- The asleep phase oscillates betwwen 10.5mA and 16.5mA

The optimization efforts will focus on the asleep phase since the system will spend most of its time in this mode (100 noisy events of 15s per day : 1500s active out of 86400s in a day so a very conservative estimate is at least 95% of the time spent in this mode).

The code was compiled with the following flags

```
DEBUG = 1
OPT = -O0
```