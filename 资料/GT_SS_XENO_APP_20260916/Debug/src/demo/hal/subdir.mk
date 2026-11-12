################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/demo/hal/hasp_hal_drv.c 

OBJS += \
./src/demo/hal/hasp_hal_drv.o 

C_DEPS += \
./src/demo/hal/hasp_hal_drv.d 


# Each subdirectory must supply rules for building sources it contributes
src/demo/hal/%.o: ../src/demo/hal/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__GTRY_IDE__ -U__GNUC__ -IE:/RY818/andes_ide/v532_818_ntoTN_rdy_V006/usb_hasp_demo/src/demo -IE:/RY818/andes_ide/v532_818_ntoTN_rdy_V006/usb_hasp_demo/src/demo/hal -IE:/RY818/andes_ide/v532_818_ntoTN_rdy_V006/usb_hasp_demo/src/bsp -IE:/RY818/andes_ide/v532_818_ntoTN_rdy_V006/usb_hasp_demo/src/driver/source -IE:/RY818/andes_ide/v532_818_ntoTN_rdy_V006/usb_hasp_demo/src/driver/include -IE:/RY818/andes_ide/v532_818_ntoTN_rdy_V006/usb_hasp_demo/src/tu_driver -IE:/RY818/andes_ide/v532_818_ntoTN_rdy_V006/usb_hasp_demo/src/tu_driver/portable/gtry -IE:/RY818/andes_ide/v532_818_ntoTN_rdy_V006/usb_hasp_demo/src/driver_ext/hall -O0 -mcmodel=medium -g3 -Wall -mcpu=n25f -ffunction-sections -fdata-sections -c -fmessage-length=0 -fno-builtin -fomit-frame-pointer -fno-strict-aliasing -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" $(TUNING_FLAGS) -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


