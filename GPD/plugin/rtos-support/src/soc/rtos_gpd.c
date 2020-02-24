/***************************************************************************//**
 * @brief Adaptation for running Bluetooth in RTOS
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

#include <stdint.h>
#include <string.h>
#include <em_device.h>
#include <kernel/include/os.h>
#include  <common/include/rtos_utils.h>

#include "rtos_gpd.h"
#include "gpd-components-common.h"
#include <rtos_utils.h>

#ifdef DEBUG_RADIO
#include "em_prs.h"
#include "em_gpio.h"
#endif

void GpdUpdate();

volatile gpd_cmd_t global_gpd_cmd;
volatile gpd_rsp_t global_gpd_rsp;
volatile gpd_evt_t global_gpd_evt;

volatile gpd_ll_event_t gpd_ll_evt;//Holds Radio -> Stack data

OS_MUTEX           GpdMutex;

//Gpd task
#ifndef GPD_STACK_SIZE
#define GPD_STACK_SIZE (1500 / sizeof(CPU_STK))
#endif
static  void  GpdTask (void  *p_arg);
static  OS_TCB            GpdTaskTCB;
static  CPU_STK           GpdTaskStk[GPD_STACK_SIZE];

//Linklayer task
#ifndef GPD_LINKLAYER_STACK_SIZE
#define GPD_LINKLAYER_STACK_SIZE (1000 / sizeof(CPU_STK))
#endif
static  void  GpdLinklayerTask (void  *p_arg);
static  OS_TCB            GpdLinklayerTaskTCB;
static  CPU_STK           GpdLinklayerTaskStk[GPD_LINKLAYER_STACK_SIZE];
//
// #define RTOS_TICK_HZ  1024
// #define BLUETOOTH_TICK_HZ  32768
// #define BLUETOOTH_TO_RTOS_TICK  (BLUETOOTH_TICK_HZ / RTOS_TICK_HZ)

//static volatile wakeupCallback wakeupCB = NULL;
// Set the task to post semaphore
// void BluetoothSetWakeupCallback(wakeupCallback cb)
// {
//   wakeupCB = (volatile wakeupCallback)cb;
// }

#ifdef DEBUG_RADIO
static void debug_init(void);
#endif

OS_FLAG_GRP gpd_event_flags;

void gpd_start(OS_PRIO ll_priority,
                     OS_PRIO stack_priority,
                     gpd_stack_init_func initialize_gpd_stack)
{
  RTOS_ERR os_err;

  OSFlagCreate(&gpd_event_flags,
               "Gpd Flags",
               (OS_FLAGS)0,
               &os_err);
  OSMutexCreate(&GpdMutex,
                "Gpd Mutex",
                &os_err);

  APP_RTOS_ASSERT_DBG((initialize_gpd_stack != NULL), 1);
  initialize_gpd_stack();

  //create tasks for Bluetooth host stack
  OSTaskCreate(&GpdTaskTCB,
               "Gpd Task",
               GpdTask,
               0u,
               stack_priority,
               &GpdTaskStk[0u],
               GPD_STACK_SIZE / 10u,
               GPD_STACK_SIZE,
               0u,
               0u,
               0u,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               &os_err);
  //create tasks for Linklayer
  OSTaskCreate(&GpdLinklayerTaskTCB,
               "GpdLinklayer Task",
               GpdLinklayerTask,
               0u,
               ll_priority,
               &GpdLinklayerTaskStk[0u],
               GPD_LINKLAYER_STACK_SIZE / 10u,
               GPD_LINKLAYER_STACK_SIZE,
               0u,
               0u,
               0u,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               &os_err);
}

//This callback is called from interrupt context (Kernel Aware)
//sets flag to trigger Link Layer Task
void GpdLLCallback()
{
  RTOS_ERR os_err;
  OSFlagPost(&gpd_event_flags, (OS_FLAGS)GPD_EVENT_FLAG_LL, OS_OPT_POST_FLAG_SET, &os_err);
}

//This callback is called from Gpd stack
//Called from kernel aware interrupt context (RTCC interrupt) and from Gpd task
//sets flag to trigger running Gpd stack
void GpdUpdate()
{
  RTOS_ERR os_err;
  OSFlagPost(&gpd_event_flags, (OS_FLAGS)GPD_EVENT_FLAG_STACK, OS_OPT_POST_FLAG_SET, &os_err);
}

//Gpd task, it waits for events from gpd and handles them
void GpdTask(void *p)
{
  RTOS_ERR      os_err;
  OS_FLAGS      flags = GPD_EVENT_FLAG_EVT_HANDLED;
  uint32_t      taskTimeoutTicks = 0;//gecko_can_sleep_ticks();
  EmberGpd_t *  gpdContext;

  while (DEF_TRUE) {
    //Command needs to be sent to Gpd stack
    if (flags & GPD_EVENT_FLAG_CMD_WAITING) {
      //Command received from Application
      //Handle it here
      switch (global_gpd_cmd.id) {
        case gpd_cmd_id_commission:
          emberGpdSetState(EMBER_GPD_APP_STATE_CHANNEL_REQUEST);
          break;
        default:
          break;
      }

      //Return result
      global_gpd_rsp.id = global_gpd_cmd.id;
      global_gpd_rsp.result = 0;

      flags &= ~GPD_EVENT_FLAG_CMD_WAITING;
      OSFlagPost(&gpd_event_flags, (OS_FLAGS)GPD_EVENT_FLAG_RSP_WAITING, OS_OPT_POST_FLAG_SET, &os_err);
    }

    //Gpd stack needs updating, and evt can be used
    if ((flags & GPD_EVENT_FLAG_STACK) ) {  //update bluetooth & read event
      gpdContext = emberGpdGetGpd();
      switch (emberGpdGetState())
      {
          case EMBER_GPD_APP_STATE_NOT_COMMISSIONED :
            taskTimeoutTicks = 0;//Falling here we wait for ever
            break;

          case EMBER_GPD_APP_STATE_CHANNEL_REQUEST :
          case EMBER_GPD_APP_STATE_CHANNEL_RECEIVED :
          case EMBER_GPD_APP_STATE_COMMISSIONING_REQUEST :
          case EMBER_GPD_APP_STATE_COMMISSIONING_REPLY_RECIEVED :
            taskTimeoutTicks = 900;
            emberGpdAfPluginCommission(gpdContext);
            break;

          case EMBER_GPD_APP_STATE_COMMISSIONING_SUCCESS_REQUEST :
            global_gpd_evt.id = gpd_evt_id_init_done;
            global_gpd_evt.result = 0x00;
            emberGpdSetState(EMBER_GPD_APP_STATE_OPERATIONAL);
            break;

          case EMBER_GPD_APP_STATE_OPERATIONAL :
            taskTimeoutTicks = 0;//Falling here we wait for ever
            break;
          case EMBER_GPD_APP_STATE_OPERATIONAL_COMMAND_REQUEST :
            taskTimeoutTicks = 0;
            //sendToggle(gpdContext);
            emberGpdSetState(EMBER_GPD_APP_STATE_OPERATIONAL);
            break;
          //case EMBER_GPD_APP_STATE_OPERATIONAL_COMMAND_RECEIVED :
          case EMBER_GPD_APP_STATE_INVALID :
            emberGpdAfPluginDeCommission(gpdContext);
            break;

          default:
            //Wait for next external event
            break;
      }
      emberGpdStoreSecDataToNV(gpdContext);

      OSFlagPost(&gpd_event_flags, (OS_FLAGS)GPD_EVENT_FLAG_EVT_WAITING, OS_OPT_POST_FLAG_SET, &os_err);
      flags &= ~(GPD_EVENT_FLAG_EVT_HANDLED);
      // if (wakeupCB != NULL) {
      //   wakeupCB();
      // }
    }

    // Ask from Bluetooth stack how long we can sleep
    // UINT32_MAX = sleep indefinitely
    // 0 = cannot sleep, stack needs update and we need to check if evt is handled that we can actually update it
    // if (timeout == 0 && (flags & GPD_EVENT_FLAG_EVT_HANDLED)) {
    //   continue;
    // }
    // //OSFlagPend expects 0 to be indefinite
    // if (timeout == UINT32_MAX) {
    //   timeout = 0;
    // } else {
    //   //round up to RTOS ticks
    //   timeout = (timeout + BLUETOOTH_TO_RTOS_TICK - 1) / BLUETOOTH_TO_RTOS_TICK;
    // }
    flags |= OSFlagPend(&gpd_event_flags,
                        (OS_FLAGS)GPD_EVENT_FLAG_STACK + GPD_EVENT_FLAG_EVT_HANDLED + GPD_EVENT_FLAG_CMD_WAITING,
                        taskTimeoutTicks,
                        OS_OPT_PEND_BLOCKING + OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME,
                        NULL,
                        &os_err);
    // Use of cmd_waiting and evt_handled flags should be fine, as they must happen one-by-one
    // flag_stack should not be used, as these can come frequently, possibly multiple at a time.
    // Events will be checked regardless of whether timeout happened or not
    // if (RTOS_ERR_CODE_GET(os_err) == RTOS_ERR_TIMEOUT) {}
  }
}

static  void  GpdLinklayerTask(void *p_arg)
{
  RTOS_ERR      os_err;
  (void)p_arg;

  while (DEF_TRUE) {
    OSFlagPend(&gpd_event_flags, (OS_FLAGS)GPD_EVENT_FLAG_LL,
               0,
               OS_OPT_PEND_BLOCKING + OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME,
               NULL,
               &os_err);

    switch (gpd_ll_evt.type) {
      case gpd_ll_event_packet_received:
        emberGpdIncomingMessageHandler(gpd_ll_evt.data.data, gpd_ll_evt.data.dataSize);
        break;
      default:
        break;
    }
  }
}

//hooks for API
//called from tasks using BGAPI
void gpd_cmd_rtos_delegate(void)
{
  RTOS_ERR os_err;

  OSFlagPost(&gpd_event_flags, (OS_FLAGS)GPD_EVENT_FLAG_CMD_WAITING, OS_OPT_POST_FLAG_SET, &os_err);
  //wait for response
  OSFlagPend(&gpd_event_flags, (OS_FLAGS)GPD_EVENT_FLAG_RSP_WAITING,
             0,
             OS_OPT_PEND_BLOCKING + OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME,
             NULL,
             &os_err);
}

void GpdPend(RTOS_ERR *err)
{
  OSMutexPend((OS_MUTEX *)&GpdMutex,
              (OS_TICK   )0,
              (OS_OPT    )OS_OPT_PEND_BLOCKING,
              (CPU_TS   *)NULL,
              (RTOS_ERR *)err);
}
void GpdPost(RTOS_ERR *err)
{
  OSMutexPost((OS_MUTEX *)&GpdMutex,
              (OS_OPT    )OS_OPT_POST_NONE,
              (RTOS_ERR *)err);
}


uint16_t gpd_init(void)
{
  // Initialise NV
  emberGpdNvInit();//TODO as we are relying on the BLE stack NVM, need to make sure it was init first
  // Initialise timer for rxOffset timing during rxAfterTx
  emberGpdLeTimerInit();
  // Initialise Radio
  emberGpdRadioInit();
  //Initialise the Gpd
  EmberGpd_t *  gpdContext = emberGpdInit();
#ifdef DEBUG_RADIO
  debug_init();
#endif
  return 0;
}

#ifdef DEBUG_RADIO
static void debug_init(void)
{
  // Turn on the PRS and GPIO clocks so we can access their registers
  CMU_ClockEnable(cmuClock_PRS, true);
  CMU_ClockEnable(cmuClock_GPIO, true);

  GPIO_PinModeSet(gpioPortC, 10, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortC, 11, gpioModePushPull, 0);

  PRS_SourceSignalSet(9, PRS_RAC_RX & 0xFF00,
                          ((PRS_RAC_RX & _PRS_CH_CTRL_SIGSEL_MASK) >> _PRS_CH_CTRL_SIGSEL_SHIFT),
                                prsEdgeOff);
  PRS_SourceSignalSet(10,
                      (PRS_RAC_TX & 0xFF00),
                      ((PRS_RAC_TX & _PRS_CH_CTRL_SIGSEL_MASK) >> _PRS_CH_CTRL_SIGSEL_SHIFT),
                      prsEdgeOff);

  PRS->ROUTELOC2 &= ~_PRS_ROUTELOC2_CH9LOC_MASK;
  PRS->ROUTELOC2 |= PRS_ROUTELOC2_CH9LOC_LOC15;
  PRS->ROUTELOC2 &= ~_PRS_ROUTELOC2_CH10LOC_MASK;
  PRS->ROUTELOC2 |= PRS_ROUTELOC2_CH10LOC_LOC5;
  PRS->ROUTEPEN |= (PRS_ROUTEPEN_CH9PEN | PRS_ROUTEPEN_CH10PEN);
}
#endif
