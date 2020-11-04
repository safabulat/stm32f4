################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include startup/subdir.mk
-include src/subdir.mk
-include subdir.mk
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

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
LTDCexample_playground.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
LTDCexample_playground.list \

OBJCOPY_BIN += \
LTDCexample_playground.bin \


# All Target
all: LTDCexample_playground.elf secondary-outputs

# Tool invocations
LTDCexample_playground.elf: $(OBJS) $(USER_OBJS) C:\Users\Seffet\Desktop\PCWorkStuff\Embedded\CubeMX\try\LinkerScript.ld
	arm-none-eabi-gcc -o "LTDCexample_playground.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\Seffet\Desktop\PCWorkStuff\Embedded\CubeMX\try\LinkerScript.ld" --specs=nosys.specs -Wl,-Map="LTDCexample_playground.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

LTDCexample_playground.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "LTDCexample_playground.list"
	@echo 'Finished building: $@'
	@echo ' '

LTDCexample_playground.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "LTDCexample_playground.bin"
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
