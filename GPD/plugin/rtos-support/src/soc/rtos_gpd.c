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

void GpdUpdate();
//volatile struct gpd_cmd_packet*  gpd_evt;
volatile gpd_ll_event_t *gpd_ll_evt;

OS_MUTEX           GpdMutex;

//volatile static uint32_t command_header;
//volatile static void*    command_data;
//volatile static gpd_cmd_handler command_handler_func = NULL;

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

  while (DEF_TRUE) {
    //Command needs to be sent to Gpd stack
    if (flags & GPD_EVENT_FLAG_CMD_WAITING) {
      //uint32_t header = command_header;
      // gpd_cmd_handler cmd_handler = command_handler_func;
      // sli_bt_cmd_handler_delegate(header, cmd_handler, (void*)command_data);
      gpd_cmd_rtos_delegate();
      // command_handler_func = NULL;
      flags &= ~GPD_EVENT_FLAG_CMD_WAITING;
      OSFlagPost(&gpd_event_flags, (OS_FLAGS)GPD_EVENT_FLAG_RSP_WAITING, OS_OPT_POST_FLAG_SET, &os_err);
    }

    //Gpd stack needs updating, and evt can be used
    if (gecko_event_pending() && (flags & GPD_EVENT_FLAG_EVT_HANDLED)) {  //update bluetooth & read event
      //gpd_evt = gpd_wait_event();
      //APP_RTOS_ASSERT_DBG(gpd_evt, 1);
      OSFlagPost(&gpd_event_flags, (OS_FLAGS)GPD_EVENT_FLAG_EVT_WAITING, OS_OPT_POST_FLAG_SET, &os_err);
      flags &= ~(GPD_EVENT_FLAG_EVT_HANDLED);
      // if (wakeupCB != NULL) {
      //   wakeupCB();
      // }
    }

    // Ask from Bluetooth stack how long we can sleep
    // UINT32_MAX = sleep indefinitely
    // 0 = cannot sleep, stack needs update and we need to check if evt is handled that we can actually update it
    uint32_t timeout = 0;//gecko_can_sleep_ticks();
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
                        timeout,
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

    //TODO handle RAIL events here (i.e. decoding RX packets, handling RX data and Maintenance frames etc)
    switch (gpd_ll_evt->type) {
      case gpd_ll_event_packet_received:
        emberGpdIncomingMessageHandler(gpd_ll_evt->data.data, gpd_ll_evt->data.dataSize);
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

//   command_header = header;
//   command_handler_func = handler;
//   command_data = (void*)payload;
  //Command structure is filled, notify the stack
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
