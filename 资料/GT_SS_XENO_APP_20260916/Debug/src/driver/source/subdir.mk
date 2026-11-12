################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/driver/source/gtry_adc_drv.c \
../src/driver/source/gtry_aes_drv.c \
../src/driver/source/gtry_argb_drv.c \
../src/driver/source/gtry_dma_drv.c \
../src/driver/source/gtry_dvp_drv.c \
../src/driver/source/gtry_gpio_drv.c \
../src/driver/source/gtry_i2c_drv.c \
../src/driver/source/gtry_ioam_drv.c \
../src/driver/source/gtry_ltimer_drv.c \
../src/driver/source/gtry_pwm_drv.c \
../src/driver/source/gtry_qspi_flash_drv.c \
../src/driver/source/gtry_r8080_drv.c \
../src/driver/source/gtry_rtc_drv.c \
../src/driver/source/gtry_spi_drv.c \
../src/driver/source/gtry_timer_drv.c \
../src/driver/source/gtry_trng_drv.c \
../src/driver/source/gtry_tsc_drv.c \
../src/driver/source/gtry_uart_drv.c \
../src/driver/source/gtry_wdt_drv.c 

OBJS += \
./src/driver/source/gtry_adc_drv.o \
./src/driver/source/gtry_aes_drv.o \
./src/driver/source/gtry_argb_drv.o \
./src/driver/source/gtry_dma_drv.o \
./src/driver/source/gtry_dvp_drv.o \
./src/driver/source/gtry_gpio_drv.o \
./src/driver/source/gtry_i2c_drv.o \
./src/driver/source/gtry_ioam_drv.o \
./src/driver/source/gtry_ltimer_drv.o \
./src/driver/source/gtry_pwm_drv.o \
./src/driver/source/gtry_qspi_flash_drv.o \
./src/driver/source/gtry_r8080_drv.o \
./src/driver/source/gtry_rtc_drv.o \
./src/driver/source/gtry_spi_drv.o \
./src/driver/source/gtry_timer_drv.o \
./src/driver/source/gtry_trng_drv.o \
./src/driver/source/gtry_tsc_drv.o \
./src/driver/source/gtry_uart_drv.o \
./src/driver/source/gtry_wdt_drv.o 

C_DEPS += \
./src/driver/source/gtry_adc_drv.d \
./src/driver/source/gtry_aes_drv.d \
./src/driver/source/gtry_argb_drv.d \
./src/driver/source/gtry_dma_drv.d \
./src/driver/source/gtry_dvp_drv.d \
./src/driver/source/gtry_gpio_drv.d \
./src/driver/source/gtry_i2c_drv.d \
./src/driver/source/gtry_ioam_drv.d \
./src/driver/source/gtry_ltimer_drv.d \
./src/driver/source/gtry_pwm_drv.d \
./src/driver/source/gtry_qspi_flash_drv.d \
./src/driver/source/gtry_r8080_drv.d \
./src/driver/source/gtry_rtc_drv.d \
./src/driver/source/gtry_spi_drv.d \
./src/driver/source/gtry_timer_drv.d \
./src/driver/source/gtry_trng_drv.d \
./src/driver/source/gtry_tsc_drv.d \
./src/driver/source/gtry_uart_drv.d \
./src/driver/source/gtry_wdt_drv.d 


# Each subdirectory must supply rules for building sources it contributes
src/driver/source/%.o: ../src/driver/source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Andes C Compiler'
	$(CROSS_COMPILE)gcc -D__GTRY_IDE__ -U__GNUC__ -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/demo -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/h6 -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/demo/source_hal -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/demo/include_hal -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/demo/flash_driver -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/bsp -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/driver/source -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/driver/include -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/tu_driver -ID:/Users/cong.che/AndeSight5/workspace/GT_SS_XENO_APP_20260916/src/tu_driver/portable/gtry -O0 -mcmodel=medium -g3 -Wall -mcpu=n25f -ffunction-sections -fdata-sections -c -fmessage-length=0 -fno-builtin -fomit-frame-pointer -fno-strict-aliasing -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $(@:%.o=%.o)" $(TUNING_FLAGS) -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


