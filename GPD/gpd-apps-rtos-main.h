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
#define INIT_FLAG  ((OS_FLAGS)0x01)
#define DUMMY_FLAG  ((OS_FLAGS)0x02)


///////////////////////////////////////////////////////////////////////////////
/////////////////////////////// NVM related ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


#define GREEN_POWER_PSSTORE_BASE 			0x4050
#define GREEN_POWER_PSSTORE_TAG_KEY  		0x4050
#define GREEN_POWER_PSSTORE_CONTEXT_KEY  	0x4051

//// Proprietary radio
//static RAIL_Handle_t railHandle = NULL;
//static RAILSched_Config_t railSchedState;
//static void radioEventHandler(RAIL_Handle_t railHandle,
//                              RAIL_Events_t events);
//static RAIL_Config_t railCfg = {
//  .eventsCallback = &radioEventHandler,
//  .protocol = NULL,
//  .scheduler = &railSchedState,
//};

#endif
