######################################
# target
######################################
TARGET = LePetitCrieur


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization (use O0 instead of Og for debugging)
OPT = -O0


#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
Src/debug.c \
Src/dfsdm.c \
Src/dma.c \
Src/fmc.c \
Src/gpio.c \
Src/gtzc.c \
Src/main.c \
Src/octospi.c \
Src/stm32l5xx_hal_msp.c \
Src/stm32l5xx_it.c \
Src/system_stm32l5xx.c \
Src/tim.c \
Src/st7789h2.c \
Src/st7789h2_reg.c \
Src/stm32l562e_discovery_lcd.c \
Src/stm32_lcd.c \
Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal.c \
Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_cortex.c \
Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_dfsdm.c \
Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_dfsdm_ex.c \
Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_dma.c \
Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_dma_ex.c \
Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_exti.c \
Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_flash.c \
Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_flash_ex.c \
Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_flash_ramfunc.c \
Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_gpio.c \
Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_gtzc.c \
Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_i2c.c \
Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_i2c_ex.c \
Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_icache.c \
Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_ospi.c \
Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_pwr.c \
Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_pwr_ex.c \
Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_rcc.c \
Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_rcc_ex.c \
Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_sram.c \
Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_tim.c \
Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_hal_tim_ex.c \
Drivers/STM32L5xx_HAL_Driver/Src/stm32l5xx_ll_fmc.c


# ASM sources
ASM_SOURCES =  \
startup_stm32l562qeixq.s


#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m33

# fpu
FPU = -mfpu=fpv5-sp-d16
# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS =

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32L562xx

# AS includes
AS_INCLUDES =

# C includes
C_INCLUDES =  \
-IInc \
-IDrivers/STM32L5xx_HAL_Driver/Inc \
-IDrivers/STM32L5xx_HAL_Driver/Inc/Legacy \
-IDrivers/CMSIS/Device/ST/STM32L5xx/Include \
-IDrivers/CMSIS/Include

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections -fstack-usage -std=gnu11

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections -fstack-usage -std=gnu11

ifeq ($(DEBUG), 1)
CFLAGS += -g3 -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32L562QEIXQ_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys
LIBDIR =
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir $@

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)

flash:
	#cp $(BUILD_DIR)/$(TARGET).bin /media/laurent/DIS_L562QE/
	openocd -f openocd_interface_target_stlink.cfg -f openocd_stm32_flash.cfg

#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***