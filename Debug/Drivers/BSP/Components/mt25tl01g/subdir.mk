################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/Components/mt25tl01g/mt25tl01g.c 

OBJS += \
./Drivers/BSP/Components/mt25tl01g/mt25tl01g.o 

C_DEPS += \
./Drivers/BSP/Components/mt25tl01g/mt25tl01g.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/mt25tl01g/%.o Drivers/BSP/Components/mt25tl01g/%.su: ../Drivers/BSP/Components/mt25tl01g/%.c Drivers/BSP/Components/mt25tl01g/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H750xx -c -I"/Users/patriciobulic/STM32CubeIDE/STM32H750B/H7-BSP-LCD-OS/Drivers/BSP" -I"/Users/patriciobulic/STM32CubeIDE/STM32H750B/H7-BSP-LCD-OS/Utilities/Fonts" -I"/Users/patriciobulic/STM32CubeIDE/STM32H750B/H7-BSP-LCD-OS/Utilities/lcd" -I"/Users/patriciobulic/STM32CubeIDE/STM32H750B/H7-BSP-LCD-OS/Drivers/BSP/Components/ft5336" -I"/Users/patriciobulic/STM32CubeIDE/STM32H750B/H7-BSP-LCD-OS/Drivers/BSP/Components/mt25tl01g" -I"/Users/patriciobulic/STM32CubeIDE/STM32H750B/H7-BSP-LCD-OS/Drivers/BSP/Components/mt48lc4m32b2" -I"/Users/patriciobulic/STM32CubeIDE/STM32H750B/H7-BSP-LCD-OS/Drivers/BSP/Components/rk043fn48h" -I"/Users/patriciobulic/STM32CubeIDE/STM32H750B/H7-BSP-LCD-OS/Drivers/BSP/Components/wm8994" -I"/Users/patriciobulic/STM32CubeIDE/STM32H750B/H7-BSP-LCD-OS/Drivers/BSP/Components/Common" -I"/Users/patriciobulic/STM32CubeIDE/STM32H750B/H7-BSP-LCD-OS/Middlewares/ST/STM32_Audio/Addons/PDM/Inc" -I"/Users/patriciobulic/STM32CubeIDE/STM32H750B/H7-BSP-LCD-OS/Drivers/BSP/Components" -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-Components-2f-mt25tl01g

clean-Drivers-2f-BSP-2f-Components-2f-mt25tl01g:
	-$(RM) ./Drivers/BSP/Components/mt25tl01g/mt25tl01g.d ./Drivers/BSP/Components/mt25tl01g/mt25tl01g.o ./Drivers/BSP/Components/mt25tl01g/mt25tl01g.su

.PHONY: clean-Drivers-2f-BSP-2f-Components-2f-mt25tl01g

