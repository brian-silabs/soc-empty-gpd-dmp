
#include "gpd-apps-rtos-main.h"

/**************************************************************************//**
 * Proprietary Application task.
 *
 * @param p_arg Pointer to an optional data area which can pass parameters to
 *              the task when the task executes.
 *
 * This is a minimal Proprietary Application task that only configures the
 * radio.
 *****************************************************************************/
static void greenPowerAppTask(void *p_arg)
{
  PP_UNUSED_PARAM(p_arg);
  RTOS_ERR err;

  

  while (DEF_TRUE) {
    // Wait for the dummy flag. Use this flag to stop waiting with the execution of your code.
    OSFlagPend(&proprietary_event_flags,
               DUMMY_FLAG,
               (OS_TICK)0,
               OS_OPT_PEND_BLOCKING       \
               + OS_OPT_PEND_FLAG_SET_ANY \
               + OS_OPT_PEND_FLAG_CONSUME,
               NULL,
               &err);
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
    //
    // Put your code here!
    //
  }
}