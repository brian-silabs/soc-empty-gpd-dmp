/***************************************************************************//**
 * @brief Adaptation for running Green Power Device in RTOS
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

#ifndef RTOS_GPD_H
#define RTOS_GPD_H

#include <stdint.h>
#include <kernel/include/os.h>

#include "gpd_commands.h"
#include "gpd_events.h"

//Gpd event flag group
extern OS_FLAG_GRP gpd_event_flags;
//Gpd event flag definitions
#define GPD_EVENT_FLAG_STACK       ((OS_FLAGS)1)    //Gpd task needs an update
#define GPD_EVENT_FLAG_LL          ((OS_FLAGS)2)    //Linklayer task needs an update
#define GPD_EVENT_FLAG_CMD_WAITING ((OS_FLAGS)4)    //BGAPI command is waiting to be processed
#define GPD_EVENT_FLAG_RSP_WAITING ((OS_FLAGS)8)    //BGAPI response is waiting to be processed
#define GPD_EVENT_FLAG_EVT_WAITING ((OS_FLAGS)16)   //BGAPI event is waiting to be processed
#define GPD_EVENT_FLAG_EVT_HANDLED ((OS_FLAGS)32)   //BGAPI event is handled

// LL task to Stack Task data
typedef enum gpd_ll_event_type {
  gpd_ll_event_packet_received,
  gpd_ll_event_failure
} gpd_ll_event_type_t;

typedef struct gpd_ll_event_data
{
    uint16_t dataSize;
    uint8_t *data;
} gpd_ll_event_data_t;

typedef struct
{
    gpd_ll_event_type_t type;
    gpd_ll_event_data_t data;
} gpd_ll_event_t;

extern volatile gpd_ll_event_t gpd_ll_evt;//TODO make it a queue ?

// Stack Task <-> Application Task data
typedef struct gpd_event_data
{
    uint16_t dataSize;
    uint8_t *data;
} gpd_event_data_t;

typedef struct
{
    gpd_cmd_id_t id;
    gpd_event_data_t data;
} gpd_cmd_t;

typedef struct
{
    gpd_cmd_id_t id;
    uint16_t result;
} gpd_rsp_t;

typedef struct
{
    gpd_evt_id_t id;
    uint16_t result;
} gpd_evt_t;

extern volatile gpd_cmd_t global_gpd_cmd;//TODO make it a queue ?
extern volatile gpd_rsp_t global_gpd_rsp;//TODO make it a queue ?
extern volatile gpd_evt_t global_gpd_evt;//TODO make it a queue ?

// Function prototype for initializing GPD stack.
typedef uint8_t(*gpd_stack_init_func)();

/**
 * Start GPD tasks. The given GPD stack initialization function
 * will be called at a proper time. Application should not initialize
 * GPD stack anywhere else.
 *
 * @param ll_priority link layer task priority
 * @param stack_priority GPD stack task priority
 * @param initialize_gpd_stack The function for initializing Bluetooth stack
 */
void gpd_start(OS_PRIO ll_priority,
                     OS_PRIO stack_priority,
                     gpd_stack_init_func initialize_gpd_stack);

//Used to call GPD apis from any task
void gpd_cmd_rtos_delegate(void);

// Set the callback for wakeup, Bluetooth task will call this when it has a new event
// It must only used to wake up application task, for example by posting task semaphore
//typedef void (*wakeupCallback)();
//void BluetoothSetWakeupCallback(wakeupCallback cb);
//Bluetooth stack needs an update
void GpdUpdate();
//Linklayer is updated
void GpdLLCallback();

//Mutex functions for using Gpd from multiple tasks
void GpdPend(RTOS_ERR *err);
void GpdPost(RTOS_ERR *err);

#endif //RTOS_GPD_H
