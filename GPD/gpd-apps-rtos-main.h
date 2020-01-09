#ifndef __GPD_APPS_RTOS_MAIN__
#define __GPD_APPS_RTOS_MAIN__


#include <kernel/include/os.h>

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////// NVM related ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define GREEN_POWER_PSSTORE_BASE            0x4050u
#define GREEN_POWER_PSSTORE_TAG_KEY         0x4050u
#define GREEN_POWER_PSSTORE_CONTEXT_KEY     0x4051u

///////////////////////////////////////////////////////////////////////////////
////////////////////////////// RTOS related ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Proprietary Application task
#define GREEN_POWER_APP_TASK_PRIO         6u
#define GREEN_POWER_APP_TASK_STACK_SIZE   (1024 / sizeof(CPU_STK))

CPU_STK greenPowerAppTaskStk[GREEN_POWER_APP_TASK_STACK_SIZE];
OS_TCB  greenPowerAppTaskTCB;
void    greenPowerAppTask(void *p_arg);

/* OS Event Flag Group */
OS_FLAG_GRP  proprietary_event_flags;

///////////////////////////////////////////////////////////////////////////////
////////////////////// Inter task communication related ///////////////////////
///////////////////////////////////////////////////////////////////////////////

uint8_t GPD_GetGpdState(void);
int8_t GPD_Init(void);
int8_t GPD_StartCommissioning(void);
int8_t GPD_DeCommission(void);
int8_t GPD_Toggle(void);


typedef enum {
    GPD_EVENT_INIT_OVER = 0,
    GPD_EVENT_COMMISSIONING_OVER,
    GPD_EVENT_DECOMMISSIONING_OVER
} gpdEvent_t;

#endif
