################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include util/third_party/mbedtls/sl_crypto/src/subdir.mk
-include util/third_party/mbedtls/library/subdir.mk
-include protocol/bluetooth/lib/EFR32MG12P/GCC/subdir.mk
-include protocol/bluetooth/ble_stack/src/soc/subdir.mk
-include platform/service/sleeptimer/src/subdir.mk
-include platform/micrium_os/ports/source/gnu/subdir.mk
-include platform/micrium_os/kernel/source/subdir.mk
-include platform/micrium_os/cpu/source/subdir.mk
-include platform/micrium_os/common/source/rtos/subdir.mk
-include platform/micrium_os/common/source/platform_mgr/subdir.mk
-include platform/micrium_os/common/source/lib/subdir.mk
-include platform/micrium_os/common/source/kal/subdir.mk
-include platform/micrium_os/common/source/common/subdir.mk
-include platform/micrium_os/common/source/collections/subdir.mk
-include platform/micrium_os/bsp/siliconlabs/generic/source/subdir.mk
-include platform/emlib/src/subdir.mk
-include platform/emdrv/sleep/src/subdir.mk
-include platform/emdrv/gpiointerrupt/src/subdir.mk
-include platform/common/src/subdir.mk
-include platform/Device/SiliconLabs/EFR32MG12P/Source/GCC/subdir.mk
-include platform/Device/SiliconLabs/EFR32MG12P/Source/subdir.mk
-include hardware/kit/common/drivers/subdir.mk
-include app/bluetooth/common/util/subdir.mk
-include GPD/plugin/apps/subdir.mk
-include GPD/components/subdir.mk
-include GPD/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: soc-empty-gpd-dmp.axf

# Tool invocations
soc-empty-gpd-dmp.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -T "C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\efr32mg12p332f1024gl125.ld" -Xlinker --gc-sections -Xlinker -Map="soc-empty-gpd-dmp.map" -mfpu=fpv4-sp-d16 -mfloat-abi=softfp --specs=nano.specs -o soc-empty-gpd-dmp.axf -Wl,--start-group "./protocol/bluetooth/ble_stack/src/soc/rtos_bluetooth.o" "./platform/service/sleeptimer/src/sl_sleeptimer.o" "./platform/service/sleeptimer/src/sl_sleeptimer_hal_rtc.o" "./platform/service/sleeptimer/src/sl_sleeptimer_hal_rtcc.o" "./platform/micrium_os/ports/source/gnu/armv7m_cpu_a.o" "./platform/micrium_os/ports/source/gnu/armv7m_cpu_c.o" "./platform/micrium_os/ports/source/gnu/armv7m_os_cpu_a.o" "./platform/micrium_os/ports/source/gnu/armv7m_os_cpu_c.o" "./platform/micrium_os/kernel/source/os_cfg_app.o" "./platform/micrium_os/kernel/source/os_core.o" "./platform/micrium_os/kernel/source/os_dbg.o" "./platform/micrium_os/kernel/source/os_flag.o" "./platform/micrium_os/kernel/source/os_mem.o" "./platform/micrium_os/kernel/source/os_mon.o" "./platform/micrium_os/kernel/source/os_msg.o" "./platform/micrium_os/kernel/source/os_mutex.o" "./platform/micrium_os/kernel/source/os_prio.o" "./platform/micrium_os/kernel/source/os_q.o" "./platform/micrium_os/kernel/source/os_sem.o" "./platform/micrium_os/kernel/source/os_stat.o" "./platform/micrium_os/kernel/source/os_task.o" "./platform/micrium_os/kernel/source/os_time.o" "./platform/micrium_os/kernel/source/os_tmr.o" "./platform/micrium_os/kernel/source/os_var.o" "./platform/micrium_os/cpu/source/cpu_core.o" "./platform/micrium_os/common/source/rtos/rtos_err_str.o" "./platform/micrium_os/common/source/platform_mgr/platform_mgr.o" "./platform/micrium_os/common/source/lib/lib_ascii.o" "./platform/micrium_os/common/source/lib/lib_math.o" "./platform/micrium_os/common/source/lib/lib_mem.o" "./platform/micrium_os/common/source/lib/lib_str.o" "./platform/micrium_os/common/source/kal/kal_kernel.o" "./platform/micrium_os/common/source/common/common.o" "./platform/micrium_os/common/source/collections/bitmap.o" "./platform/micrium_os/common/source/collections/map.o" "./platform/micrium_os/common/source/collections/slist.o" "./platform/micrium_os/bsp/siliconlabs/generic/source/bsp_cpu.o" "./platform/micrium_os/bsp/siliconlabs/generic/source/bsp_os.o" "./platform/emlib/src/em_assert.o" "./platform/emlib/src/em_burtc.o" "./platform/emlib/src/em_cmu.o" "./platform/emlib/src/em_core.o" "./platform/emlib/src/em_cryotimer.o" "./platform/emlib/src/em_crypto.o" "./platform/emlib/src/em_emu.o" "./platform/emlib/src/em_eusart.o" "./platform/emlib/src/em_gpio.o" "./platform/emlib/src/em_i2c.o" "./platform/emlib/src/em_letimer.o" "./platform/emlib/src/em_msc.o" "./platform/emlib/src/em_rmu.o" "./platform/emlib/src/em_rtc.o" "./platform/emlib/src/em_rtcc.o" "./platform/emlib/src/em_se.o" "./platform/emlib/src/em_system.o" "./platform/emlib/src/em_timer.o" "./platform/emlib/src/em_usart.o" "./platform/emdrv/sleep/src/sleep.o" "./platform/emdrv/gpiointerrupt/src/gpiointerrupt.o" "./platform/common/src/sl_slist.o" "./platform/Device/SiliconLabs/EFR32MG12P/Source/GCC/startup_efr32mg12p.o" "./platform/Device/SiliconLabs/EFR32MG12P/Source/system_efr32mg12p.o" "./hardware/kit/common/drivers/mx25flash_spi.o" "./hardware/kit/common/drivers/udelay.o" "./app/bluetooth/common/util/infrastructure.o" "./GPD/plugin/apps/gpd-apps-commission.o" "./GPD/plugin/apps/gpd-apps-crypto-test.o" "./GPD/plugin/apps/gpd-apps-main.o" "./GPD/plugin/apps/gpd-apps-node.o" "./GPD/plugin/apps/gpd-apps-utility.o" "./GPD/components/gpd-nv.o" "./GPD/components/gpd-nwk.o" "./GPD/components/gpd-rail.o" "./GPD/components/gpd-security.o" "./GPD/components/gpd-send-receive.o" "./GPD/gpd-callback-stubs.o" "./GPD/gpd-callbacks.o" "./application_properties.o" "./gatt_db.o" "./init_app.o" "./init_board.o" "./init_mcu.o" "./main.o" "./pti.o" "./rail_config.o" "C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\protocol\bluetooth\lib\EFR32MG12P\GCC\binapploader.o" "C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\protocol\bluetooth\lib\EFR32MG12P\GCC\libbluetooth.a" "C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\platform\radio\rail_lib\autogen\librail_release\librail_multiprotocol_efr32xg12_gcc_release.a" "C:\Users\Brian\SimplicityStudio\v4_workspace_Clean\soc-empty-gpd-dmp\protocol\bluetooth\lib\EFR32MG12P\GCC\libpsstore.a" -lm "./util/third_party/mbedtls/library/aes.o" "./util/third_party/mbedtls/library/bignum.o" "./util/third_party/mbedtls/library/ccm.o" "./util/third_party/mbedtls/library/cipher.o" "./util/third_party/mbedtls/library/cipher_wrap.o" "./util/third_party/mbedtls/library/cmac.o" "./util/third_party/mbedtls/library/ecdh.o" "./util/third_party/mbedtls/library/ecp.o" "./util/third_party/mbedtls/library/ecp_curves.o" "./util/third_party/mbedtls/library/threading.o" "./util/third_party/mbedtls/sl_crypto/src/crypto_aes.o" "./util/third_party/mbedtls/sl_crypto/src/crypto_ble.o" "./util/third_party/mbedtls/sl_crypto/src/crypto_ecp.o" "./util/third_party/mbedtls/sl_crypto/src/crypto_management.o" -Wl,--end-group -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: soc-empty-gpd-dmp.hex'
	arm-none-eabi-objcopy -O ihex "soc-empty-gpd-dmp.axf" "soc-empty-gpd-dmp.hex"
	@echo ' '

	@echo 'Building bin file: soc-empty-gpd-dmp.bin'
	arm-none-eabi-objcopy -O binary "soc-empty-gpd-dmp.axf" "soc-empty-gpd-dmp.bin"
	@echo ' '

	@echo 'Building s37 file: soc-empty-gpd-dmp.s37'
	arm-none-eabi-objcopy -O srec "soc-empty-gpd-dmp.axf" "soc-empty-gpd-dmp.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "soc-empty-gpd-dmp.axf" -A
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) soc-empty-gpd-dmp.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
