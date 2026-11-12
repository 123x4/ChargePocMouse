################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/flash_code/gtry6xx_flash_lib.c 

OBJS += \
./src/flash_code/gtry6xx_flash_lib.o 

C_DEPS += \
./src/flash_code/gtry6xx_flash_lib.d 


# Each subdirectory must supply rules for building sources it contributes
src/flash_code/%.o: ../src/flash_code/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__GTRY_IDE__ -U__GNUC__ -IE:/R5/R5_SDK_v100/usb_hasp_demo/R5_H6_Demo_thride_mode_v1.1_20260404_1509/src/demo -IE:/R5/R5_SDK_v100/usb_hasp_demo/R5_H6_Demo_thride_mode_v1.1_20260404_1509/src/bsp -IE:/R5/R5_SDK_v100/usb_hasp_demo/R5_H6_Demo_thride_mode_v1.1_20260404_1509/src/driver/source -IE:/R5/R5_SDK_v100/usb_hasp_demo/R5_H6_Demo_thride_mode_v1.1_20260404_1509/src/driver/include -IE:/R5/R5_SDK_v100/usb_hasp_demo/R5_H6_Demo_thride_mode_v1.1_20260404_1509/src/tu_driver -IE:/R5/R5_SDK_v100/usb_hasp_demo/R5_H6_Demo_thride_mode_v1.1_20260404_1509/src/tu_driver/portable/gtry -O0 -mcmodel=medium -g3 -Wall -mcpu=n25f -ffunction-sections -fdata-sections -c -fmessage-length=0 -fno-builtin -fomit-frame-pointer -fno-strict-aliasing -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" $(TUNING_FLAGS) -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


