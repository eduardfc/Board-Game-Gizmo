################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Src/subdir.mk
-include Drivers/STM32F1xx_HAL_Driver/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
BGB2019.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
BGB2019.list \

OBJCOPY_BIN += \
BGB2019.bin \


# All Target
all: BGB2019.elf secondary-outputs

# Tool invocations
BGB2019.elf: $(OBJS) $(USER_OBJS) C:\Users\edfca\Desktop\BGB\BGB2019\STM32F103C8TX_FLASH.ld
	arm-none-eabi-gcc -o "BGB2019.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"C:\Users\edfca\Desktop\BGB\BGB2019\STM32F103C8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="BGB2019.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

BGB2019.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "BGB2019.list"
	@echo 'Finished building: $@'
	@echo ' '

BGB2019.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "BGB2019.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
