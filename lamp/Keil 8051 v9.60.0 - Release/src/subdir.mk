################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
A51_UPPER_SRCS += \
C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/SILABS_STARTUP.A51 

C_SRCS += \
../src/InitDevice.c \
../src/Interrupts.c \
../src/command_handler.c \
../src/hdlc_l.c \
../src/lamp_main.c \
../src/pwm.c \
../src/rtc_driver.c \
../src/state_machine.c \
../src/timer_4.c 

OBJS += \
./src/InitDevice.OBJ \
./src/Interrupts.OBJ \
./src/SILABS_STARTUP.OBJ \
./src/command_handler.OBJ \
./src/hdlc_l.OBJ \
./src/lamp_main.OBJ \
./src/pwm.OBJ \
./src/rtc_driver.OBJ \
./src/state_machine.OBJ \
./src/timer_4.OBJ 


# Each subdirectory must supply rules for building sources it contributes
src/%.OBJ: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Keil 8051 Compiler'
	C51 "@$(patsubst %.OBJ,%.__i,$@)" || $(RC)
	@echo 'Finished building: $<'
	@echo ' '

src/InitDevice.OBJ: C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/EFM8BB52/inc/SI_EFM8BB52_Register_Enums.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/InitDevice.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/EFM8BB52/inc/SI_EFM8BB52_Defs.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/si_toolchain.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/stdint.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/stdbool.h

src/Interrupts.OBJ: C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/EFM8BB52/inc/SI_EFM8BB52_Register_Enums.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/state_machine.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/command_handler.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/Interrupts.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/EFM8BB52/inc/SI_EFM8BB52_Defs.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/stdint.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/pwm.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/si_toolchain.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/stdbool.h

src/SILABS_STARTUP.OBJ: C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/SILABS_STARTUP.A51 src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Keil 8051 Assembler'
	AX51 "@$(patsubst %.OBJ,%.__ia,$@)" || $(RC)
	@echo 'Finished building: $<'
	@echo ' '

src/command_handler.OBJ: C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/EFM8BB52/inc/SI_EFM8BB52_Register_Enums.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/command_handler.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/pwm.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/hdlc_l.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/state_machine.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/rtc_driver.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/EFM8BB52/inc/SI_EFM8BB52_Defs.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/stdint.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/si_toolchain.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/stdbool.h

src/hdlc_l.OBJ: C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/EFM8BB52/inc/SI_EFM8BB52_Register_Enums.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/hdlc_l.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/state_machine.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/EFM8BB52/inc/SI_EFM8BB52_Defs.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/si_toolchain.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/stdint.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/stdbool.h

src/lamp_main.OBJ: C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/state_machine.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/EFM8BB52/inc/SI_EFM8BB52_Register_Enums.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/InitDevice.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/timer_4.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/pwm.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/rtc_driver.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/EFM8BB52/inc/SI_EFM8BB52_Defs.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/si_toolchain.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/stdint.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/stdbool.h

src/pwm.OBJ: C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/state_machine.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/EFM8BB52/inc/SI_EFM8BB52_Register_Enums.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/pwm.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/command_handler.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/EFM8BB52/inc/SI_EFM8BB52_Defs.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/stdint.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/si_toolchain.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/stdbool.h

src/rtc_driver.OBJ: C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/EFM8BB52/inc/SI_EFM8BB52_Register_Enums.h C:/SiliconLabs/SimplicityStudio/v5/developer/toolchains/keil_8051/9.60/INC/MATH.H C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/EFM8BB52/peripheral_driver/inc/smb_0.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/rtc_driver.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/pwm.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/state_machine.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/command_handler.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/EFM8BB52/inc/SI_EFM8BB52_Defs.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/efm8_config.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/stdint.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/si_toolchain.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/stdbool.h

src/state_machine.OBJ: C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/state_machine.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/pwm.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/rtc_driver.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/command_handler.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/EFM8BB52/inc/SI_EFM8BB52_Register_Enums.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/stdint.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/EFM8BB52/inc/SI_EFM8BB52_Defs.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/si_toolchain.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/stdbool.h

src/timer_4.OBJ: C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/EFM8BB52/inc/SI_EFM8BB52_Register_Enums.h C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/timer_4.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/EFM8BB52/inc/SI_EFM8BB52_Defs.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/si_toolchain.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/stdint.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/stdbool.h


