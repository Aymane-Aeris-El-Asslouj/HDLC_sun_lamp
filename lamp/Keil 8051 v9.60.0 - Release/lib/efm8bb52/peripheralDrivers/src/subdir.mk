################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/EFM8BB52/peripheral_driver/src/smb_0.c 

OBJS += \
./lib/efm8bb52/peripheralDrivers/src/smb_0.OBJ 


# Each subdirectory must supply rules for building sources it contributes
lib/efm8bb52/peripheralDrivers/src/smb_0.OBJ: C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/EFM8BB52/peripheral_driver/src/smb_0.c lib/efm8bb52/peripheralDrivers/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Keil 8051 Compiler'
	C51 "@$(patsubst %.OBJ,%.__i,$@)" || $(RC)
	@echo 'Finished building: $<'
	@echo ' '

lib/efm8bb52/peripheralDrivers/src/smb_0.OBJ: C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/EFM8BB52/peripheral_driver/inc/smb_0.h C:/SiliconLabs/SimplicityStudio/v5/developer/toolchains/keil_8051/9.60/INC/ASSERT.H C:/Users/elass/SimplicityStudio/v5_workspace/lamp_2/inc/efm8_config.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/EFM8BB52/inc/SI_EFM8BB52_Register_Enums.h C:/SiliconLabs/SimplicityStudio/v5/developer/toolchains/keil_8051/9.60/INC/STDIO.H C:/SiliconLabs/SimplicityStudio/v5/developer/toolchains/keil_8051/9.60/INC/STDLIB.H C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/EFM8BB52/inc/SI_EFM8BB52_Defs.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/si_toolchain.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/stdint.h C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/8051/v4.2.5/Device/shared/si8051Base/stdbool.h


