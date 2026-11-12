################################################################################
# Automatically-generated file. Do not edit!
################################################################################

ifndef ANDESIGHT_ROOT
ANDESIGHT_ROOT=C:\Andestech\AndeSight_RDS_v542
endif

ifndef CROSS_COMPILE
CROSS_COMPILE=riscv32-elf-
endif

ifndef SECONDARY_OUTPUT_PATH
SECONDARY_OUTPUT_PATH=output
endif

$(shell mkdir -p $(SECONDARY_OUTPUT_PATH))

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include src/bsp/subdir.mk
-include src/demo/subdir.mk
-include src/demo/flash_driver/subdir.mk
-include src/demo/source_hal/subdir.mk
-include src/driver/source/subdir.mk
-include src/h6/subdir.mk
-include src/tu_driver/class/audio/subdir.mk
-include src/tu_driver/class/bth/subdir.mk
-include src/tu_driver/class/cdc/subdir.mk
-include src/tu_driver/class/dfu/subdir.mk
-include src/tu_driver/class/hid/subdir.mk
-include src/tu_driver/class/midi/subdir.mk
-include src/tu_driver/class/msc/subdir.mk
-include src/tu_driver/class/net/subdir.mk
-include src/tu_driver/class/usbtmc/subdir.mk
-include src/tu_driver/class/vendor/subdir.mk
-include src/tu_driver/class/video/subdir.mk
-include src/tu_driver/common/subdir.mk
-include src/tu_driver/device/subdir.mk
-include src/tu_driver/host/subdir.mk
-include src/tu_driver/portable/gtry/subdir.mk
-include src/tu_driver/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
BUILD_ARTIFACT_FILE_BASE_NAME = GT_SS_XENO_APP_20260916

LINKER_OUTPUTS = GT_SS_XENO_APP_20260916.adx

GPROF_OUT += \
../gmon.sum ../gmon.out \

LOADADDR_OUT += \
$(foreach subdir,$(SECONDARY_OUTPUT_PATH)/.loadedaddr,$(wildcard $(subdir)/*.*)) \

GCOV_OUT += \
$(foreach subdir,$(SUBDIRS),$(wildcard $(subdir)/*.gcda) $(wildcard $(subdir)/*.gcno)) \

SYMBOL_OUTPUTS += \
$(SECONDARY_OUTPUT_PATH)/symbol.txt \

READELF_OUTPUTS += \
$(SECONDARY_OUTPUT_PATH)/readelf.txt \

OBJCOPY_OUTPUTS += \
$(SECONDARY_OUTPUT_PATH)/$(BUILD_ARTIFACT_FILE_BASE_NAME).bin \

SIZE_OUTPUTS += \
$(SECONDARY_OUTPUT_PATH)/.PHONY.size \

LDSAG_OUT=$(SECONDARY_OUTPUT_PATH)/gtry_rv32-load.ld

# All Target
all: GT_SS_XENO_APP_20260916.adx secondary-outputs

# Tool invocations
GT_SS_XENO_APP_20260916.adx: $(OBJS) $(USER_OBJS) $(LDSAG_OUT)
	@echo 'Building target: $@'
	@echo 'Invoking: Andes C Linker'
	$(CROSS_COMPILE)gcc -O0 -nostartfiles -static -mcmodel=medium -T"$(LDSAG_OUT)" -Wl,--gc-sections -mcpu=n25f $(TUNING_FLAGS) -o "GT_SS_XENO_APP_20260916.adx" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

$(foreach subdir,$(SUBDIRS),$(wildcard $(subdir)/*.gcda) $(wildcard $(subdir)/*.gcno)): GT_SS_XENO_APP_20260916.adx $(OBJS) $(USER_OBJS) $(LDSAG_OUT)
../gmon.sum ../gmon.out: GT_SS_XENO_APP_20260916.adx $(OBJS) $(USER_OBJS) $(LDSAG_OUT)
$(foreach subdir,$(SECONDARY_OUTPUT_PATH)/.loadedaddr,$(wildcard $(subdir)/*.*)): GT_SS_XENO_APP_20260916.adx $(OBJS) $(USER_OBJS) $(LDSAG_OUT)

$(SECONDARY_OUTPUT_PATH)/symbol.txt: $(LINKER_OUTPUTS)
	@echo 'Invoking: NM (symbol listing)'
	$(CROSS_COMPILE)nm -n -l -C "GT_SS_XENO_APP_20260916.adx" > $(SECONDARY_OUTPUT_PATH)/symbol.txt
	@echo 'Finished building: $@'
	@echo ' '

$(SECONDARY_OUTPUT_PATH)/readelf.txt: $(LINKER_OUTPUTS)
	@echo 'Invoking: Readelf (ELF info listing)'
	$(CROSS_COMPILE)readelf -a "GT_SS_XENO_APP_20260916.adx" > $(SECONDARY_OUTPUT_PATH)/readelf.txt
	@echo 'Finished building: $@'
	@echo ' '

: $(LINKER_OUTPUTS)
	@echo 'Invoking: Objdump (disassembly)'
	$(CROSS_COMPILE)objdump -x -d -C "GT_SS_XENO_APP_20260916.adx" >
	@echo 'Finished building: $@'
	@echo ' '

$(SECONDARY_OUTPUT_PATH)/$(BUILD_ARTIFACT_FILE_BASE_NAME).bin: $(LINKER_OUTPUTS)
	@echo 'Invoking: Objcopy (object content copy)'
	$(CROSS_COMPILE)objcopy -S -O binary "GT_SS_XENO_APP_20260916.adx" $(SECONDARY_OUTPUT_PATH)/$(BUILD_ARTIFACT_FILE_BASE_NAME).bin
	@echo 'Finished building: $@'
	@echo ' '

$(SECONDARY_OUTPUT_PATH)/.PHONY.size: $(LINKER_OUTPUTS)
	@echo 'Invoking: Size (section size listing)'
	$(CROSS_COMPILE)size  "GT_SS_XENO_APP_20260916.adx" | tee $(SECONDARY_OUTPUT_PATH)/.PHONY.size
	@echo 'Finished building: $@'
	@echo ' '

$(SECONDARY_OUTPUT_PATH)/gtry_rv32-load.ld: $(SAG_SRCS)
	@echo 'Invoking: LdSaG Tool'
	"$(ANDESIGHT_ROOT)/utils/nds_ldsag" --version=v5 -t "$(ANDESIGHT_ROOT)/utils/nds32_template_v5.txt" "$(SAG_FILE)" -o $(LDSAG_OUT)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(GPROF_OUT) $(SIZE_OUTPUTS) $(READELF_OUTPUTS) $(LDSAG_OUT) $(EXECUTABLES) $(LOADADDR_OUT) $(OBJS) $(SYMBOL_OUTPUTS) $(S_UPPER_DEPS) $(GCOV_OUT) $(C_DEPS) $(OBJCOPY_OUTPUTS) GT_SS_XENO_APP_20260916.adx
	-@echo ' '

secondary-outputs: $(SYMBOL_OUTPUTS) $(READELF_OUTPUTS) $(OBJCOPY_OUTPUTS) $(SIZE_OUTPUTS)

.PHONY: all clean dependents main-build
.SECONDARY:

-include ../makefile.targets
