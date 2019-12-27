################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../platform/common/src/sl_slist.c 

OBJS += \
./platform/common/src/sl_slist.o 

C_DEPS += \
./platform/common/src/sl_slist.d 


# Each subdirectory must supply rules for building sources it contributes
platform/common/src/sl_slist.o: ../platform/common/src/sl_slist.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-D__StackLimit=0x20000000' '-DMBEDTLS_MICRIUMOS=1' '-DMBEDTLS_CONFIG_FILE="config-multiprotocol.h"' '-D__STACK_SIZE=0x800' '-DHAL_CONFIG=1' '-D__HEAP_SIZE=0xD00' '-DEFR32MG12P332F1024GL125=1' -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\emlib\inc" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\util\third_party\mbedtls\include" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\util\third_party\mbedtls\include\mbedtls" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\util\third_party\mbedtls\sl_crypto\include" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\util\silicon_labs\silabs_core\memory_manager" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\emlib\src_inc" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\GPD" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\GPD\components" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\radio\rail_lib\chip\efr32\efr32xg1x" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\emlib\src" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\Device\SiliconLabs\EFR32MG12P\Include" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\CMSIS\Include" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\micrium_os\common\source\rtos" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\radio\rail_lib\protocol\ble" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\hardware\kit\common\drivers" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\micrium_os\kernel\source" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\micrium_os\kernel\include" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\micrium_os\cpu\include" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\protocol\bluetooth\ble_stack\inc\common" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\halconfig\inc\hal-config" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\hardware\kit\common\halconfig" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\micrium_os\bsp\siliconlabs\generic\include" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\radio\rail_lib\common" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\hardware\kit\EFR32MG12_BRD4162A\config" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\service\sleeptimer\config" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\micrium_os\ports\source\gnu" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\Device\SiliconLabs\EFR32MG12P\Source" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\app\bluetooth\common\util" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\micrium_os\common\include" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\service\sleeptimer\src" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\hardware\kit\common\bsp" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\Device\SiliconLabs\EFR32MG12P\Source\GCC" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\micrium_os\bsp\siliconlabs\generic\source" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\common\inc" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\emdrv\gpiointerrupt\inc" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\bootloader\api" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\micrium_os\common\source\collections" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\radio\rail_lib\protocol\ieee802154" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\emdrv\gpiointerrupt\src" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\micrium_os\common\source\platform_mgr" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\micrium_os\common\source\ring_buf" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\micrium_os\common\source\lib" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\micrium_os\common\source\common" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\micrium_os\cpu\source" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\service\sleeptimer\inc" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\protocol\bluetooth\ble_stack\inc\soc" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\micrium_os\common\source\logging" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\protocol\bluetooth\ble_stack\src\soc" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\micrium_os\common\source\kal" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\emdrv\common\inc" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\emdrv\sleep\inc" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\micrium_os\common\source\preprocessor" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\common\src" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\emdrv\sleep\src" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\bootloader" -I"C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\micrium_os" -O2 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"platform/common/src/sl_slist.d" -MT"platform/common/src/sl_slist.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


