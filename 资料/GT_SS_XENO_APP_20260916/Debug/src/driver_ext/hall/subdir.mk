################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/driver_ext/hall/H5_hal_drv.c \
../src/driver_ext/hall/ry_H5_drv.c \
../src/driver_ext/hall/ry_H6_drv.c 

OBJS += \
./src/driver_ext/hall/H5_hal_drv.o \
./src/driver_ext/hall/ry_H5_drv.o \
./src/driver_ext/hall/ry_H6_drv.o 

C_DEPS += \
./src/driver_ext/hall/H5_hal_drv.d \
./src/driver_ext/hall/ry_H5_drv.d \
./src/driver_ext/hall/ry_H6_drv.d 


# Each subdirectory must supply rules for building sources it contributes
src/driver_ext/hall/%.o: ../src/driver_ext/hall/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__GTRY_IDE__ -U__GNUC__ -IE:/RY818/andes_ide/v532_818_ntoTN_rdy_V006/usb_hasp_demo/src/demo -IE:/RY818/andes_ide/v532_818_ntoTN_rdy_V006/usb_hasp_demo/src/demo/hal -IE:/RY818/andes_ide/v532_818_ntoTN_rdy_V006/usb_hasp_demo/src/bsp -IE:/RY818/andes_ide/v532_818_ntoTN_rdy_V006/usb_hasp_demo/src/driver/source -IE:/RY818/andes_ide/v532_818_ntoTN_rdy_V006/usb_hasp_demo/src/driver/include -IE:/RY818/andes_ide/v532_818_ntoTN_rdy_V006/usb_hasp_demo/src/tu_driver -IE:/RY818/andes_ide/v532_818_ntoTN_rdy_V006/usb_hasp_demo/src/tu_driver/portable/gtry -IE:/RY818/andes_ide/v532_818_ntoTN_rdy_V006/usb_hasp_demo/src/driver_ext/hall -O0 -mcmodel=medium -g3 -Wall -mcpu=n25f -ffunction-sections -fdata-sections -c -fmessage-length=0 -fno-builtin -fomit-frame-pointer -fno-strict-aliasing -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" $(TUNING_FLAGS) -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


