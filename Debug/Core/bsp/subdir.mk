################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/bsp/ac_current.c \
../Core/bsp/ac_voltage.c \
../Core/bsp/backup.c \
../Core/bsp/bsp_adc.c \
../Core/bsp/bsp_mpu6050.c \
../Core/bsp/bsp_sensor.c \
../Core/bsp/cc1120.c \
../Core/bsp/clock.c \
../Core/bsp/global.c \
../Core/bsp/input.c \
../Core/bsp/led.c \
../Core/bsp/ntc_sensor.c \
../Core/bsp/packet.c \
../Core/bsp/pressur.c \
../Core/bsp/ringbuf.c \
../Core/bsp/running_avg.c \
../Core/bsp/sensor_drv.c \
../Core/bsp/sleep.c \
../Core/bsp/sleep_init.c \
../Core/bsp/spi_function.c \
../Core/bsp/tdma_mgt.c \
../Core/bsp/tdma_n.c \
../Core/bsp/tdma_table.c \
../Core/bsp/timerB.c \
../Core/bsp/urm37_distance.c \
../Core/bsp/water_det.c 

OBJS += \
./Core/bsp/ac_current.o \
./Core/bsp/ac_voltage.o \
./Core/bsp/backup.o \
./Core/bsp/bsp_adc.o \
./Core/bsp/bsp_mpu6050.o \
./Core/bsp/bsp_sensor.o \
./Core/bsp/cc1120.o \
./Core/bsp/clock.o \
./Core/bsp/global.o \
./Core/bsp/input.o \
./Core/bsp/led.o \
./Core/bsp/ntc_sensor.o \
./Core/bsp/packet.o \
./Core/bsp/pressur.o \
./Core/bsp/ringbuf.o \
./Core/bsp/running_avg.o \
./Core/bsp/sensor_drv.o \
./Core/bsp/sleep.o \
./Core/bsp/sleep_init.o \
./Core/bsp/spi_function.o \
./Core/bsp/tdma_mgt.o \
./Core/bsp/tdma_n.o \
./Core/bsp/tdma_table.o \
./Core/bsp/timerB.o \
./Core/bsp/urm37_distance.o \
./Core/bsp/water_det.o 

C_DEPS += \
./Core/bsp/ac_current.d \
./Core/bsp/ac_voltage.d \
./Core/bsp/backup.d \
./Core/bsp/bsp_adc.d \
./Core/bsp/bsp_mpu6050.d \
./Core/bsp/bsp_sensor.d \
./Core/bsp/cc1120.d \
./Core/bsp/clock.d \
./Core/bsp/global.d \
./Core/bsp/input.d \
./Core/bsp/led.d \
./Core/bsp/ntc_sensor.d \
./Core/bsp/packet.d \
./Core/bsp/pressur.d \
./Core/bsp/ringbuf.d \
./Core/bsp/running_avg.d \
./Core/bsp/sensor_drv.d \
./Core/bsp/sleep.d \
./Core/bsp/sleep_init.d \
./Core/bsp/spi_function.d \
./Core/bsp/tdma_mgt.d \
./Core/bsp/tdma_n.d \
./Core/bsp/tdma_table.d \
./Core/bsp/timerB.d \
./Core/bsp/urm37_distance.d \
./Core/bsp/water_det.d 


# Each subdirectory must supply rules for building sources it contributes
Core/bsp/%.o Core/bsp/%.su Core/bsp/%.cyclo: ../Core/bsp/%.c Core/bsp/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -c -I../Core/Inc -I"C:/Dev_Work/UHF_GRIP_OLD/slave_wakeup_fix/Core/bsp" -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-bsp

clean-Core-2f-bsp:
	-$(RM) ./Core/bsp/ac_current.cyclo ./Core/bsp/ac_current.d ./Core/bsp/ac_current.o ./Core/bsp/ac_current.su ./Core/bsp/ac_voltage.cyclo ./Core/bsp/ac_voltage.d ./Core/bsp/ac_voltage.o ./Core/bsp/ac_voltage.su ./Core/bsp/backup.cyclo ./Core/bsp/backup.d ./Core/bsp/backup.o ./Core/bsp/backup.su ./Core/bsp/bsp_adc.cyclo ./Core/bsp/bsp_adc.d ./Core/bsp/bsp_adc.o ./Core/bsp/bsp_adc.su ./Core/bsp/bsp_mpu6050.cyclo ./Core/bsp/bsp_mpu6050.d ./Core/bsp/bsp_mpu6050.o ./Core/bsp/bsp_mpu6050.su ./Core/bsp/bsp_sensor.cyclo ./Core/bsp/bsp_sensor.d ./Core/bsp/bsp_sensor.o ./Core/bsp/bsp_sensor.su ./Core/bsp/cc1120.cyclo ./Core/bsp/cc1120.d ./Core/bsp/cc1120.o ./Core/bsp/cc1120.su ./Core/bsp/clock.cyclo ./Core/bsp/clock.d ./Core/bsp/clock.o ./Core/bsp/clock.su ./Core/bsp/global.cyclo ./Core/bsp/global.d ./Core/bsp/global.o ./Core/bsp/global.su ./Core/bsp/input.cyclo ./Core/bsp/input.d ./Core/bsp/input.o ./Core/bsp/input.su ./Core/bsp/led.cyclo ./Core/bsp/led.d ./Core/bsp/led.o ./Core/bsp/led.su ./Core/bsp/ntc_sensor.cyclo ./Core/bsp/ntc_sensor.d ./Core/bsp/ntc_sensor.o ./Core/bsp/ntc_sensor.su ./Core/bsp/packet.cyclo ./Core/bsp/packet.d ./Core/bsp/packet.o ./Core/bsp/packet.su ./Core/bsp/pressur.cyclo ./Core/bsp/pressur.d ./Core/bsp/pressur.o ./Core/bsp/pressur.su ./Core/bsp/ringbuf.cyclo ./Core/bsp/ringbuf.d ./Core/bsp/ringbuf.o ./Core/bsp/ringbuf.su ./Core/bsp/running_avg.cyclo ./Core/bsp/running_avg.d ./Core/bsp/running_avg.o ./Core/bsp/running_avg.su ./Core/bsp/sensor_drv.cyclo ./Core/bsp/sensor_drv.d ./Core/bsp/sensor_drv.o ./Core/bsp/sensor_drv.su ./Core/bsp/sleep.cyclo ./Core/bsp/sleep.d ./Core/bsp/sleep.o ./Core/bsp/sleep.su ./Core/bsp/sleep_init.cyclo ./Core/bsp/sleep_init.d ./Core/bsp/sleep_init.o ./Core/bsp/sleep_init.su ./Core/bsp/spi_function.cyclo ./Core/bsp/spi_function.d ./Core/bsp/spi_function.o ./Core/bsp/spi_function.su ./Core/bsp/tdma_mgt.cyclo ./Core/bsp/tdma_mgt.d ./Core/bsp/tdma_mgt.o ./Core/bsp/tdma_mgt.su ./Core/bsp/tdma_n.cyclo ./Core/bsp/tdma_n.d ./Core/bsp/tdma_n.o ./Core/bsp/tdma_n.su ./Core/bsp/tdma_table.cyclo ./Core/bsp/tdma_table.d ./Core/bsp/tdma_table.o ./Core/bsp/tdma_table.su ./Core/bsp/timerB.cyclo ./Core/bsp/timerB.d ./Core/bsp/timerB.o ./Core/bsp/timerB.su ./Core/bsp/urm37_distance.cyclo ./Core/bsp/urm37_distance.d ./Core/bsp/urm37_distance.o ./Core/bsp/urm37_distance.su ./Core/bsp/water_det.cyclo ./Core/bsp/water_det.d ./Core/bsp/water_det.o ./Core/bsp/water_det.su

.PHONY: clean-Core-2f-bsp

