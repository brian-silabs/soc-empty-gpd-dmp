/***************************************************************************//**
 * @file
 * @brief A minimal project structure, used as a starting point for custom
 * Dynamic-Multiprotocol applications. The project has the basic
 * functionality enabling peripheral connectivity, without GATT
 * services. It runs on top of Micrium OS RTOS and multiprotocol RAIL.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "bsp/siliconlabs/generic/include/bsp_os.h"

#include <common/include/common.h>
#include <common/include/lib_def.h>
#include <common/include/rtos_utils.h>
#include <common/include/toolchains.h>
#include <common/include/rtos_prio.h>
#include <common/include/platform_mgr.h>
#include <cpu/include/cpu.h>
#include <kernel/include/os.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "hal-config.h"
#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "ble-configuration.h"
#include "board_features.h"
#include "infrastructure.h"

#include "bsphalconfig.h"
#include "bsp.h"

#include "em_core.h"
#include "em_cmu.h"
#include "sleep.h"

#include "rtos_bluetooth.h"
#include "rtos_gecko.h"
#include "gatt_db.h"

#include "rtos_gpd.h"

#include "rail.h"
#include "rail_types.h"
#if !defined(_SILICON_LABS_32B_SERIES_2)
#include "rail_config.h"
#endif

#if !defined(MBEDTLS_CONFIG_FILE)
	#include "mbedtls/config.h"
#else
	#include MBEDTLS_CONFIG_FILE
#endif

#include "mbedtls/threading.h"

//This file includes shared code between BLE and GP

// Ex Main Start task
#define EX_MAIN_START_TASK_PRIO           21u
#define EX_MAIN_START_TASK_STK_SIZE       512u
static CPU_STK exMainStartTaskStk[EX_MAIN_START_TASK_STK_SIZE];
static OS_TCB  exMainStartTaskTCB;
static void    exMainStartTask(void *p_arg);

#define APP_CFG_TASK_BLUETOOTH_LL_PRIO    3u
#define APP_CFG_TASK_BLUETOOTH_STACK_PRIO 4u

// Bluetooth Application task
#define BLUETOOTH_APP_TASK_PRIO           5u
#define BLUETOOTH_APP_TASK_STACK_SIZE     (1024 / sizeof(CPU_STK))
static CPU_STK bluetoothAppTaskStk[BLUETOOTH_APP_TASK_STACK_SIZE];
static OS_TCB  bluetoothAppTaskTCB;
static void    bluetoothAppTask(void *p_arg);


#define APP_CFG_TASK_GPD_LL_PRIO          6u
#define APP_CFG_TASK_GPD_STACK_PRIO 7u

// GPD Application task
#define GPD_APP_TASK_PRIO           8u
#define GPD_APP_TASK_STACK_SIZE     (1024 / sizeof(CPU_STK))
static CPU_STK gpdAppTaskStk[GPD_APP_TASK_STACK_SIZE];
static OS_TCB  gpdAppTaskTCB;
static void    gpdAppTask(void *p_arg);


static uint8_t initialize_gpd(void);

// Timer Task Configuration
#if (OS_CFG_TMR_EN == DEF_ENABLED)
#define TIMER_TASK_PRIO                   4u
#define TIMER_TASK_STK_SIZE               256u
static CPU_STK TimerTaskStk[TIMER_TASK_STK_SIZE];
#define TIMER_TASK_CFG                    .TmrTaskCfg = \
{                                                       \
    .StkBasePtr = &TimerTaskStk[0],                     \
    .StkSize    = TIMER_TASK_STK_SIZE,                  \
    .Prio       = TIMER_TASK_PRIO,                      \
    .RateHz     = 10u                                   \
},
#else
#define TIMER_TASK_CFG
#endif

// ISR Configuration
#define ISR_STK_SIZE                      256u
static CPU_STK ISRStk[ISR_STK_SIZE];
#define ISR_CFG                           .ISR = \
{                                                \
    .StkBasePtr = (CPU_STK*)&ISRStk[0],          \
    .StkSize    = (ISR_STK_SIZE)                 \
},

/* Define RTOS_DEBUG_MODE=DEF_ENABLED at the project level,
 * for enabling debug information for Micrium Probe.*/
#if (RTOS_DEBUG_MODE == DEF_ENABLED)
#define STAT_TASK_CFG          .StatTaskCfg = \
{                                             \
    .StkBasePtr = DEF_NULL,                   \
    .StkSize    = 256u,                       \
    .Prio       = KERNEL_STAT_TASK_PRIO_DFLT, \
    .RateHz     = 10u                         \
},

#define  OS_INIT_CFG_APP            { \
    ISR_CFG                           \
    TIMER_TASK_CFG                    \
    STAT_TASK_CFG                     \
    .MsgPoolSize     = 0u,            \
    .TaskStkLimit    = 0u,            \
    .MemSeg          = DEF_NULL,      \
    .TickRate        = 1000u          \
}
#else
#define OS_INIT_CFG_APP {      \
    ISR_CFG                    \
    TIMER_TASK_CFG             \
    .MsgPoolSize   = 0u,       \
    .TaskStkLimit  = 0u,       \
    .MemSeg        = DEF_NULL, \
    .TickRate      = 1000u     \
}
#endif

#define COMMON_INIT_CFG_APP {   \
    .CommonMemSegPtr = DEF_NULL \
}

#define PLATFORM_MGR_INIT_CFG_APP { \
    .PoolBlkQtyInit = 0u,           \
    .PoolBlkQtyMax  = 0u            \
}

const OS_INIT_CFG           OS_InitCfg          = OS_INIT_CFG_APP;
const COMMON_INIT_CFG       Common_InitCfg      = COMMON_INIT_CFG_APP;
const PLATFORM_MGR_INIT_CFG PlatformMgr_InitCfg = PLATFORM_MGR_INIT_CFG_APP;

// Maximum number of Bluetooth connections.
#define MAX_CONNECTIONS 1
uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS)];
// Configuration parameters (see gecko_configuration.h)
static const gecko_configuration_t bluetooth_config =
{
  .config_flags = GECKO_CONFIG_FLAG_RTOS,
  .sleep.flags = SLEEP_FLAGS_DEEP_SLEEP_ENABLE,
  .bluetooth.max_connections = MAX_CONNECTIONS,
  .bluetooth.heap = bluetooth_stack_heap,
  .bluetooth.heap_size = sizeof(bluetooth_stack_heap),
  .gattdb = &bg_gattdb_data,
  .ota.flags = 0,
  .ota.device_name_len = 3,
  .ota.device_name_ptr = "OTA",
  .scheduler_callback = BluetoothLLCallback,
  .stack_schedule_callback = BluetoothUpdate,
  .pa.config_enable = 1, // Set this to be a valid PA config
#if defined(FEATURE_PA_INPUT_FROM_VBAT)
  .pa.input = GECKO_RADIO_PA_INPUT_VBAT, // Configure PA input to VBAT
#else
  .pa.input = GECKO_RADIO_PA_INPUT_DCDC,
#endif // defined(FEATURE_PA_INPUT_FROM_VBAT)
  .rf.flags = GECKO_RF_CONFIG_ANTENNA,                 /* Enable antenna configuration. */
  .rf.antenna = GECKO_RF_ANTENNA,                      /* Select antenna path! */
  .mbedtls.flags = GECKO_MBEDTLS_FLAGS_NO_MBEDTLS_DEVICE_INIT,
  .mbedtls.dev_number = 0,
};
static uint8_t boot_to_dfu = 0;

/**************************************************************************//**
 * Main.
 *
 * @returns Returns 1.
 *
 * This is the standard entry point for C applications. It is assumed that your
 * code will call main() once you have performed all necessary initialization.
 *****************************************************************************/
int main(void)
{
  RTOS_ERR err;
  CORE_DECLARE_IRQ_STATE;

  // Initialize device.
  initMcu();
  // Initialize board.
  initBoard();
  // Initialize application.
  initApp();
  initVcomEnable();
  // Initialize the Kernel.
  OSInit(&err);
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
  // Initialize DMP Crypto block share.
  THREADING_setup();
  // Don't allow EM3, since we use LF clocks.
  CORE_ENTER_ATOMIC();
  SLEEP_SleepBlockBegin(sleepEM3);
  CORE_EXIT_ATOMIC();
  // Create the Ex Main Start task
  OSTaskCreate(&exMainStartTaskTCB,
               "Ex Main Start Task",
               exMainStartTask,
               DEF_NULL,
               EX_MAIN_START_TASK_PRIO,
               &exMainStartTaskStk[0],
               (EX_MAIN_START_TASK_STK_SIZE / 10u),
               EX_MAIN_START_TASK_STK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
  // Start the kernel.
  OSStart(&err);
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

  return (1);
}

/***************************************************************************//**
 * This is the idle hook.
 *
 * This will be called by the Micrium OS idle task when there is no other task
 * ready to run. We just enter the lowest possible energy mode.
 ******************************************************************************/
void SleepAndSyncProtimer(void);
void OSIdleContextHook(void)
{
  while (1) {
    /* Put MCU in the lowest sleep mode available, usually EM2 */
    SleepAndSyncProtimer();
  }
}

/***************************************************************************//**
 * Setup the bluetooth init function.
 *
 * @return error code for the gecko_init function
 *
 * All bluetooth specific initialization code should be here like gecko_init(),
 * gecko_init_whitelisting(), gecko_init_multiprotocol() and so on.
 ******************************************************************************/
static errorcode_t initialize_bluetooth()
{
  errorcode_t err = gecko_init(&bluetooth_config);
  if (err == bg_err_success) {
    gecko_init_multiprotocol(NULL);
  }
  APP_RTOS_ASSERT_DBG((err == bg_err_success), 1);
  return err;
}

/**************************************************************************//**
 * Task to initialise Bluetooth and Proprietary tasks.
 *
 * @param p_arg Pointer to an optional data area which can pass parameters to
 *              the task when the task executes.
 *
 * This is the task that will be called by from main() when all services are
 * initialized successfully.
 *****************************************************************************/
static void exMainStartTask(void *p_arg)
{
  PP_UNUSED_PARAM(p_arg);
  RTOS_ERR err;

#ifdef CPU_CFG_INT_DIS_MEAS_EN
  // Initialize interrupts disabled measurement.
  CPU_IntDisMeasMaxCurReset();
#endif

  // Create the Bluetooth Application task
  OSTaskCreate(&bluetoothAppTaskTCB,
               "Bluetooth App Task",
               bluetoothAppTask,
               0u,
               BLUETOOTH_APP_TASK_PRIO,
               &bluetoothAppTaskStk[0u],
               (BLUETOOTH_APP_TASK_STACK_SIZE / 10u),
               BLUETOOTH_APP_TASK_STACK_SIZE,
               0u,
               0u,
               0u,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               &err);
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

  // Create the Proprietary Application task
  OSTaskCreate(&gpdAppTaskTCB,
               "Green Power App Task",
               gpdAppTask,
               0u,
               GPD_APP_TASK_PRIO,
               &gpdAppTaskStk[0u],
               (GPD_APP_TASK_STACK_SIZE / 10u),
               GPD_APP_TASK_STACK_SIZE,
               0u,
               0u,
               0u,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               &err);
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

  // Done starting everyone else so let's exit.
  OSTaskDel((OS_TCB *)0, &err);
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
}

/**************************************************************************//**
 * Bluetooth Application task.
 *
 * @param p_arg Pointer to an optional data area which can pass parameters to
 *              the task when the task executes.
 *
 * This is a minimal Bluetooth Application task that starts advertising after
 * boot and supports OTA upgrade feature.
 *****************************************************************************/
static void bluetoothAppTask(void *p_arg)
{
  PP_UNUSED_PARAM(p_arg);
  RTOS_ERR osErr;
  errorcode_t initErr;
  uint16_t gpdResult;

  struct gecko_msg_le_gap_start_advertising_rsp_t* pRspAdv;
  struct gecko_msg_le_gap_set_advertise_timing_rsp_t* pRspAdvT;
  struct gecko_msg_gatt_server_send_user_write_response_rsp_t* pRspWrRsp;
  struct gecko_msg_le_connection_close_rsp_t* pRspConnCl;

  // Create Bluetooth Link Layer and stack tasks
  bluetooth_start(APP_CFG_TASK_BLUETOOTH_LL_PRIO,
                  APP_CFG_TASK_BLUETOOTH_STACK_PRIO,
                  initialize_bluetooth);

  while (DEF_TRUE) {
    OSFlagPend(&bluetooth_event_flags,
               (OS_FLAGS)BLUETOOTH_EVENT_FLAG_EVT_WAITING,
               (OS_TICK)0,
               OS_OPT_PEND_BLOCKING       \
               + OS_OPT_PEND_FLAG_SET_ANY \
               + OS_OPT_PEND_FLAG_CONSUME,
               NULL,
               &osErr);
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(osErr) == RTOS_ERR_NONE), 1);

    // Handle stack events
    switch (BGLIB_MSG_ID(bluetooth_evt->header)) {
      // This boot event is generated when the system boots up after reset.
      // Do not call any stack commands before receiving the boot event.
      // Here the system is set to start advertising immediately after boot
      // procedure.
      case gecko_evt_system_boot_id:
         //APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(osErr) == RTOS_ERR_NONE), 1);

        // Set advertising parameters. 100ms advertisement interval.
        // The first parameter is advertising set handle
        // The next two parameters are minimum and maximum advertising
        // interval, both in units of (milliseconds * 1.6).
        // The last two parameters are duration and maxevents left as default.
        pRspAdvT = gecko_cmd_le_gap_set_advertise_timing(0, 1600, 1600, 0, 0);
        APP_ASSERT_DBG((pRspAdvT->result == bg_err_success), pRspAdvT->result);
        // Start general advertising and enable connections.
        pRspAdv = gecko_cmd_le_gap_start_advertising(0,
                                                     le_gap_general_discoverable,
                                                     le_gap_connectable_scannable);
        APP_ASSERT_DBG((pRspAdv->result == bg_err_success), pRspAdv->result);
        break;

      case gecko_evt_le_connection_closed_id:
        // Check if need to boot to dfu mode.
        if (boot_to_dfu) {
          // Enter to DFU OTA mode.
          gecko_cmd_system_reset(2);
        } else {
          // Restart advertising after client has disconnected.
          pRspAdv = gecko_cmd_le_gap_start_advertising(0,
                                                       le_gap_general_discoverable,
                                                       le_gap_connectable_scannable);
          APP_ASSERT_DBG((pRspAdv->result == bg_err_success), pRspAdv->result);
        }
        break;

      case gecko_evt_le_connection_opened_id:

        break;

      case gecko_evt_gatt_server_user_write_request_id:
        if (bluetooth_evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_gpd_commissioning) {
         //Enables the GPD commissioning process
         gpdResult = gpd_cmd_commission();
         //APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(osErr) == RTOS_ERR_NONE), 1);
          // Send response to Write Request.
          pRspWrRsp = gecko_cmd_gatt_server_send_user_write_response(
            bluetooth_evt->data.evt_gatt_server_user_write_request.connection,
            gattdb_gpd_commissioning,
            bg_err_success);
          APP_ASSERT_DBG((pRspWrRsp->result == bg_err_success), pRspWrRsp->result);

        }

        if (bluetooth_evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_gpd_decommissioning) {
         //Enables the GPD commissioning process
          //gpdResult = gpd_cmd_decommission();
         //APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(osErr) == RTOS_ERR_NONE), 1);
          // Send response to Write Request.
           pRspWrRsp = gecko_cmd_gatt_server_send_user_write_response(
             bluetooth_evt->data.evt_gatt_server_user_write_request.connection,
             gattdb_gpd_decommissioning,
             bg_err_success);
           APP_ASSERT_DBG((pRspWrRsp->result == bg_err_success), pRspWrRsp->result);
        }

        if (bluetooth_evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_gpd_toggle) {
         //Enables the GPD commissioning process

          gpdResult = gpd_cmd_send(NULL,0);//Send a GP Toggle, generic implementation to be done
         //APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(osErr) == RTOS_ERR_NONE), 1);
          // Send response to Write Request.
           pRspWrRsp = gecko_cmd_gatt_server_send_user_write_response(
             bluetooth_evt->data.evt_gatt_server_user_write_request.connection,
             gattdb_gpd_toggle,
             bg_err_success);
           APP_ASSERT_DBG((pRspWrRsp->result == bg_err_success), pRspWrRsp->result);
        }

        // Events related to OTA upgrading
        // Check if the user-type OTA Control Characteristic was written.
        // If ota_control was written, boot the device into Device Firmware
        // Upgrade (DFU) mode.
        if (bluetooth_evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_ota_control) {
          // Set flag to enter to OTA mode.
          boot_to_dfu = 1;
          // Send response to Write Request.
          pRspWrRsp = gecko_cmd_gatt_server_send_user_write_response(
            bluetooth_evt->data.evt_gatt_server_user_write_request.connection,
            gattdb_ota_control,
            bg_err_success);
          APP_ASSERT_DBG((pRspWrRsp->result == bg_err_success), pRspWrRsp->result);
          // Close connection to enter to DFU OTA mode.
          pRspConnCl = gecko_cmd_le_connection_close(bluetooth_evt->data.evt_gatt_server_user_write_request.connection);
          APP_ASSERT_DBG((pRspConnCl->result == bg_err_success), pRspConnCl->result);
        }
        break;

      default:
        break;
    }

    OSFlagPost(&bluetooth_event_flags,
               (OS_FLAGS)BLUETOOTH_EVENT_FLAG_EVT_HANDLED,
               OS_OPT_POST_FLAG_SET,
               &osErr);
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(osErr) == RTOS_ERR_NONE), 1);
  }
}




/***************************************************************************//**
 * Setup the bluetooth init function.
 *
 * @return error code for the gecko_init function
 *
 * All bluetooth specific initialization code should be here like gecko_init(),
 * gecko_init_whitelisting(), gecko_init_multiprotocol() and so on.
 ******************************************************************************/
static uint8_t initialize_gpd(void)
{
  uint16_t err = gpd_init();
  APP_RTOS_ASSERT_DBG((err == 0), 1);
  return err;
}


/**************************************************************************//**
 * Bluetooth Application task.
 *
 * @param p_arg Pointer to an optional data area which can pass parameters to
 *              the task when the task executes.
 *
 * This is a minimal Bluetooth Application task that starts advertising after
 * boot and supports OTA upgrade feature.
 *****************************************************************************/
static void gpdAppTask(void *p_arg)
{
  PP_UNUSED_PARAM(p_arg);
  RTOS_ERR osErr;
  uint8_t initErr;

  // Create GPD Link Layer and stack tasks
  gpd_start(APP_CFG_TASK_GPD_LL_PRIO,
            APP_CFG_TASK_GPD_STACK_PRIO,
            initialize_gpd);

  while (DEF_TRUE) {
    OSFlagPend(&gpd_event_flags,
               (OS_FLAGS)GPD_EVENT_FLAG_EVT_WAITING,
               (OS_TICK)0,
               OS_OPT_PEND_BLOCKING       \
               + OS_OPT_PEND_FLAG_SET_ANY \
               + OS_OPT_PEND_FLAG_CONSUME,
               NULL,
               &osErr);
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(osErr) == RTOS_ERR_NONE), 1);
      //TODO Cleanup this evt.id at some point
     switch (global_gpd_evt.id)
     {
     case gpd_evt_id_commissioned:
       GPIO_PinModeSet(gpioPortF, 4, gpioModePushPull, 1);//LED 0
       break;
    
     default:
       break;
     }

    OSFlagPost(&gpd_event_flags,
               (OS_FLAGS)GPD_EVENT_FLAG_EVT_HANDLED,
               OS_OPT_POST_FLAG_SET,
               &osErr);
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(osErr) == RTOS_ERR_NONE), 1);
  }
}
