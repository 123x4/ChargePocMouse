################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/bsp/gtry_cache.c \
../src/bsp/gtry_initfini.c \
../src/bsp/gtry_loader.c \
../src/bsp/gtry_regmap.c \
../src/bsp/gtry_reset.c \
../src/bsp/gtry_smu.c \
../src/bsp/gtry_trap.c 

S_UPPER_SRCS += \
../src/bsp/gtry_start.S 

OBJS += \
./src/bsp/gtry_cache.o \
./src/bsp/gtry_initfini.o \
./src/bsp/gtry_loader.o \
./src/bsp/gtry_regmap.o \
./src/bsp/gtry_reset.o \
./src/bsp/gtry_smu.o \
./src/bsp/gtry_start.o \
./src/bsp/gtry_trap.o 

S_UPPER_DEPS += \
./src/bsp/gtry_start.d 

C_DEPS += \
./src/bsp/gtry_cache.d \
./src/bsp/gtry_initfini.d \
./src/bsp/gtry_loader.d \
./src/bsp/gtry_regmap.d \
./src/bsp/gtry_reset.d \
./src/bsp/gtry_smu.d \
./src/bsp/gtry_trap.d 


# Each subdirectory must supply rules for building sources it contributes
src/bsp/%.o: ../src/bsp/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__GTRY_IDE__ -U__GNUC__ -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/demo -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/h6 -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/demo/source_hal -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/demo/include_hal -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/demo/flash_driver -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/bsp -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/driver/source -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/driver/include -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/tu_driver -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/tu_driver/portable/gtry -O0 -mcmodel=medium -g3 -Wall -mcpu=n25f -ffunction-sections -fdata-sections -c -fmessage-length=0 -fno-builtin -fomit-frame-pointer -fno-strict-aliasing -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" $(TUNING_FLAGS) -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/bsp/%.o: ../src/bsp/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__GTRY_IDE__ -U__GNUC__ -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/demo -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/h6 -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/demo/source_hal -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/demo/include_hal -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/demo/flash_driver -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/bsp -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/driver/source -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/driver/include -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/tu_driver -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/tu_driver/portable/gtry -O0 -mcmodel=medium -g3 -Wall -mcpu=n25f -ffunction-sections -fdata-sections -c -fmessage-length=0 -fno-builtin -fomit-frame-pointer -fno-strict-aliasing -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" $(TUNING_FLAGS) -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


