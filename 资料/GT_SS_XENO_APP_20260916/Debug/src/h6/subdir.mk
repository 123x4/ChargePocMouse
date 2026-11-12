################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/h6/h6_drv.c \
../src/h6/h6_gain_table.c \
../src/h6/h6_hal.c \
../src/h6/h6_hw_tab.c \
../src/h6/h6_offset_cali.c \
../src/h6/hasp_drv.c 

OBJS += \
./src/h6/h6_drv.o \
./src/h6/h6_gain_table.o \
./src/h6/h6_hal.o \
./src/h6/h6_hw_tab.o \
./src/h6/h6_offset_cali.o \
./src/h6/hasp_drv.o 

C_DEPS += \
./src/h6/h6_drv.d \
./src/h6/h6_gain_table.d \
./src/h6/h6_hal.d \
./src/h6/h6_hw_tab.d \
./src/h6/h6_offset_cali.d \
./src/h6/hasp_drv.d 


# Each subdirectory must supply rules for building sources it contributes
src/h6/%.o: ../src/h6/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__GTRY_IDE__ -U__GNUC__ -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/demo -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/h6 -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/demo/source_hal -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/demo/include_hal -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/demo/flash_driver -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/bsp -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/driver/source -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/driver/include -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/tu_driver -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/tu_driver/portable/gtry -O0 -mcmodel=medium -g3 -Wall -mcpu=n25f -ffunction-sections -fdata-sections -c -fmessage-length=0 -fno-builtin -fomit-frame-pointer -fno-strict-aliasing -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" $(TUNING_FLAGS) -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


