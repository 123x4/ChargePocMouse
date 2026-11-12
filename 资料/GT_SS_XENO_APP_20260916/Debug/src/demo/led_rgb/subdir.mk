################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/demo/led_rgb/led_rgb.c 

OBJS += \
./src/demo/led_rgb/led_rgb.o 

C_DEPS += \
./src/demo/led_rgb/led_rgb.d 


# Each subdirectory must supply rules for building sources it contributes
src/demo/led_rgb/led_rgb.o: ../src/demo/led_rgb/led_rgb.c
	@echo '正在构建文件： $<'
	@echo '正在调用： Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__GTRY_IDE__ -U__GNUC__ -IE:/R5/R5_SDK_v100/usb_hasp_demo/usb_hasp_demo/src/demo -IE:/R5/R5_SDK_v100/usb_hasp_demo/usb_hasp_demo/src/bsp -IE:/R5/R5_SDK_v100/usb_hasp_demo/usb_hasp_demo/src/driver/source -IE:/R5/R5_SDK_v100/usb_hasp_demo/usb_hasp_demo/src/driver/include -IE:/R5/R5_SDK_v100/usb_hasp_demo/usb_hasp_demo/src/tu_driver -IE:/R5/R5_SDK_v100/usb_hasp_demo/usb_hasp_demo/src/tu_driver/portable/gtry -I"E:\R5\R5_SDK_v100\usb_hasp_demo\usb_hasp_demo\src\h6" -O0 -mcmodel=medium -g3 -Wall -mcpu=n25f -ffunction-sections -fdata-sections -c -fmessage-length=0 -fno-builtin -fomit-frame-pointer -fno-strict-aliasing -MMD -MP -MF"$(@:%.o=%.d)" -MT"src/demo/led_rgb/led_rgb.d" $(TUNING_FLAGS) -o "$@" "$<"
	@echo '已结束构建： $<'
	@echo ' '


