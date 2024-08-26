################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F0xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := PushButton_EXTIInterrupt_LED
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
PushButton_EXTIInterrupt_LED.elf \

MAP_FILES += \
PushButton_EXTIInterrupt_LED.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
PushButton_EXTIInterrupt_LED.list \


# All Target
all: main-build

# Main-build Target
main-build: PushButton_EXTIInterrupt_LED.elf secondary-outputs

# Tool invocations
PushButton_EXTIInterrupt_LED.elf PushButton_EXTIInterrupt_LED.map: $(OBJS) $(USER_OBJS) W:\5w_Workshop_Works\gitRepos_projs_ws\STM32F0-Discovery-Board\PushButton_EXTIInterrupt_LED\STM32F051R8TX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "PushButton_EXTIInterrupt_LED.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m0 -T"W:\5w_Workshop_Works\gitRepos_projs_ws\STM32F0-Discovery-Board\PushButton_EXTIInterrupt_LED\STM32F051R8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="PushButton_EXTIInterrupt_LED.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

PushButton_EXTIInterrupt_LED.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "PushButton_EXTIInterrupt_LED.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) PushButton_EXTIInterrupt_LED.elf PushButton_EXTIInterrupt_LED.list PushButton_EXTIInterrupt_LED.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
