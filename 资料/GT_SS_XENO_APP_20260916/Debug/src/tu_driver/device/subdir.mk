################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/tu_driver/device/usbd.c \
../src/tu_driver/device/usbd_control.c 

OBJS += \
./src/tu_driver/device/usbd.o \
./src/tu_driver/device/usbd_control.o 

C_DEPS += \
./src/tu_driver/device/usbd.d \
./src/tu_driver/device/usbd_control.d 


# Each subdirectory must supply rules for building sources it contributes
src/tu_driver/device/%.o: ../src/tu_driver/device/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__GTRY_IDE__ -U__GNUC__ -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/demo -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/h6 -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/demo/source_hal -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/demo/include_hal -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/demo/flash_driver -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/bsp -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/driver/source -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/driver/include -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/tu_driver -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/tu_driver/portable/gtry -O0 -mcmodel=medium -g3 -Wall -mcpu=n25f -ffunction-sections -fdata-sections -c -fmessage-length=0 -fno-builtin -fomit-frame-pointer -fno-strict-aliasing -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" $(TUNING_FLAGS) -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


