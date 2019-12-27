
#include <common/include/common.h>
#include <common/include/lib_def.h>
#include <common/include/rtos_utils.h>
#include <common/include/toolchains.h>
#include <common/include/rtos_prio.h>
#include <common/include/platform_mgr.h>
#include <cpu/include/cpu.h>
#include <kernel/include/os.h>

#include "gpd-apps-rtos-main.h"
#include "gpd-components-common.h"
#include "gpd-callbacks.h"

/**************************************************************************//**
 * Proprietary Application task.
 *
 * @param p_arg Pointer to an optional data area which can pass parameters to
 *              the task when the task executes.
 *
 * This is a minimal Proprietary Application task that only configures the
 * radio.
 *****************************************************************************/
void greenPowerAppTask(void *p_arg)
{
  PP_UNUSED_PARAM(p_arg);
  RTOS_ERR err;

  OSFlagPend(&proprietary_event_flags,
               INIT_FLAG,
               (OS_TICK)0,
               OS_OPT_PEND_BLOCKING       \
               + OS_OPT_PEND_FLAG_SET_ANY \
               + OS_OPT_PEND_FLAG_CONSUME,
               NULL,
               &err);

  // Initialise NV
  emberGpdNvInit();//TODO as we are relying on the BLE stack NVM, need to make sure it was init first

  // Initialise timer for rxOffset timing during rxAfterTx
  emberGpdLeTimerInit();

  // Initialise Radio
  emberGpdRadioInit();

  //Initialise the Gpd
  EmberGpd_t * gpdContext = emberGpdInit();

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
    
    // Call user to implement rest of the thing
    emberGpdAfPluginMainCallback(gpdContext);
    
  }
}
