################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include utilities/subdir.mk
-include usb/phy/subdir.mk
-include usb/device/source/lpcip3511/subdir.mk
-include usb/device/source/subdir.mk
-include usb/device/class/hid/subdir.mk
-include usb/device/class/subdir.mk
-include startup/subdir.mk
-include source/subdir.mk
-include drivers/subdir.mk
-include device/subdir.mk
-include component/uart/subdir.mk
-include component/timer/subdir.mk
-include component/osa/subdir.mk
-include component/lists/subdir.mk
-include component/gpio/subdir.mk
-include board/subdir.mk
-include LPC55S28/drivers/subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := Marshall_20260425
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: Marshall_20260425.axf

# Tool invocations
Marshall_20260425.axf: $(OBJS) $(USER_OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\cong.che\Documents\MCUXpressoIDE_11.9.0_2144\workspace\key_light\Marshall_20260425\LPC55S28\mcuxpresso" -Xlinker -Map="Marshall_20260425.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -Xlinker --sort-section=alignment -Xlinker --cref -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -T Marshall_20260425_Debug.ld -o "Marshall_20260425.axf" $(OBJS) $(USER_OBJS) $(LIBS) -lpower_hardabi
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) Marshall_20260425.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "Marshall_20260425.axf"; # arm-none-eabi-objcopy -v -O binary "Marshall_20260425.axf" "Marshall_20260425.bin" ; # checksum -p LPC55S28 -d "Marshall_20260425.bin";
	-@echo ' '

.PHONY: all clean dependents main-build post-build

-include ../makefile.targets
