################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tests/h6_gain_table_test.c 

OBJS += \
./tests/h6_gain_table_test.o 

C_DEPS += \
./tests/h6_gain_table_test.d 


# Each subdirectory must supply rules for building sources it contributes
tests/%.o: ../tests/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__GTRY_IDE__ -U__GNUC__ -IE:/Ruiyi/A_SS/Xeno/ss_-xeno_-r5/Xeno_USB_CV_CC/TEST/hosc_optimize_0714/src/demo -IE:/Ruiyi/A_SS/Xeno/ss_-xeno_-r5/Xeno_USB_CV_CC/TEST/hosc_optimize_0714/src/h6 -IE:/Ruiyi/A_SS/Xeno/ss_-xeno_-r5/Xeno_USB_CV_CC/TEST/hosc_optimize_0714/src/demo/source_hal -IE:/Ruiyi/A_SS/Xeno/ss_-xeno_-r5/Xeno_USB_CV_CC/TEST/hosc_optimize_0714/src/demo/include_hal -IE:/Ruiyi/A_SS/Xeno/ss_-xeno_-r5/Xeno_USB_CV_CC/TEST/hosc_optimize_0714/src/demo/flash_driver -IE:/Ruiyi/A_SS/Xeno/ss_-xeno_-r5/Xeno_USB_CV_CC/TEST/hosc_optimize_0714/src/bsp -IE:/Ruiyi/A_SS/Xeno/ss_-xeno_-r5/Xeno_USB_CV_CC/TEST/hosc_optimize_0714/src/driver/source -IE:/Ruiyi/A_SS/Xeno/ss_-xeno_-r5/Xeno_USB_CV_CC/TEST/hosc_optimize_0714/src/driver/include -IE:/Ruiyi/A_SS/Xeno/ss_-xeno_-r5/Xeno_USB_CV_CC/TEST/hosc_optimize_0714/src/tu_driver -IE:/Ruiyi/A_SS/Xeno/ss_-xeno_-r5/Xeno_USB_CV_CC/TEST/hosc_optimize_0714/src/tu_driver/portable/gtry -O0 -mcmodel=medium -g3 -Wall -mcpu=n25f -ffunction-sections -fdata-sections -c -fmessage-length=0 -fno-builtin -fomit-frame-pointer -fno-strict-aliasing -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" $(TUNING_FLAGS) -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


