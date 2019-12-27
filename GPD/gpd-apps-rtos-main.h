#ifndef __GPD_APPS_RTOS_MAIN__
#define __GPD_APPS_RTOS_MAIN__

// Proprietary Application task
#define GREEN_POWER_APP_TASK_PRIO         6u
#define GREEN_POWER_APP_TASK_STACK_SIZE   (1024 / sizeof(CPU_STK))


static CPU_STK greenPowerAppTaskStk[GREEN_POWER_APP_TASK_STACK_SIZE];
static OS_TCB  greenPowerAppTaskTCB;
static void    greenPowerAppTask(void *p_arg);

/* OS Event Flag Group */
OS_FLAG_GRP  proprietary_event_flags;
/* Dummy flag to prevent cyclic execution of the proprietary task function code.
 * this flag will not be posted by default. */
#define INIT_FLAG  ((OS_FLAGS)0x01)
#define DUMMY_FLAG  ((OS_FLAGS)0x02)


// Proprietary radio
static RAIL_Handle_t railHandle = NULL;
static RAILSched_Config_t railSchedState;
static void radioEventHandler(RAIL_Handle_t railHandle,
                              RAIL_Events_t events);
static RAIL_Config_t railCfg = {
  .eventsCallback = &radioEventHandler,
  .protocol = NULL,
  .scheduler = &railSchedState,
};

#endif