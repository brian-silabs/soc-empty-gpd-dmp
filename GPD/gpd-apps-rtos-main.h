#ifndef __GPD_APPS_RTOS_MAIN__
#define __GPD_APPS_RTOS_MAIN__


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
/* Dummy flag to prevent cyclic execution of the proprietary task function code.
 * this flag will not be posted by default. */
#define INIT_FLAG               ((OS_FLAGS)0x01)
#define COMMISSIONING_FLAG      ((OS_FLAGS)0x02)
#define DECOMMISSIONING_FLAG    ((OS_FLAGS)0x04)
#define OPERATE_FLAG            ((OS_FLAGS)0x08)


///////////////////////////////////////////////////////////////////////////////
/////////////////////////////// NVM related ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


#define GREEN_POWER_PSSTORE_BASE            0x4050
#define GREEN_POWER_PSSTORE_TAG_KEY         0x4050
#define GREEN_POWER_PSSTORE_CONTEXT_KEY     0x4051

#define OS_TICK_RATE_HZ 					1024u //Check rtos_bluettoth.c - I have no idea where this is set since we are using dyn tick & sleeptimer

#define GREEN_POWER_TASK_TIMEOUT_MS         250u
#define GREEN_POWER_TASK_TIMEOUT_TICKS      ((uint32_t)((GREEN_POWER_TASK_TIMEOUT_MS/1000u)*OS_TICK_RATE_HZ))

#endif
