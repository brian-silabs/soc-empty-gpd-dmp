
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

static void sendToggle(EmberGpd_t * gpd);

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

  OS_TICK taskTimeoutTicks = 0;

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

    //TODO write the loop for commissioning
    // That disappears in ope mode
  while (DEF_TRUE) {
    // Wait for the dummy flag. Use this flag to stop waiting with the execution of your code.
    // Call user to implement rest of the thing
    OSFlagPend(&proprietary_event_flags,
                COMMISSIONING_FLAG |
                DECOMMISSIONING_FLAG |
                OPERATE_FLAG,
                (OS_TICK)taskTimeoutTicks,
                OS_OPT_PEND_BLOCKING       \
                + OS_OPT_PEND_FLAG_SET_ANY \
                + OS_OPT_PEND_FLAG_CONSUME,
                NULL,
                &err);

    switch (gpdContext->gpdState)
    {
        case EMBER_GPD_APP_STATE_NOT_COMMISSIONED :
        case EMBER_GPD_APP_STATE_CHANNEL_REQUEST :
        case EMBER_GPD_APP_STATE_CHANNEL_RECEIVED :
        case EMBER_GPD_APP_STATE_COMMISSIONING_REQUEST :
        case EMBER_GPD_APP_STATE_COMMISSIONING_REPLY_RECIEVED :
        case EMBER_GPD_APP_STATE_COMMISSIONING_SUCCESS_REQUEST :
            emberGpdAfPluginCommission(gpd);
            emberGpdStoreSecDataToNV(gpd);
            taskTimeoutTicks = GREEN_POWER_TASK_TIMEOUT_TICKS;
            break;

        case EMBER_GPD_APP_STATE_OPERATIONAL :
        case EMBER_GPD_APP_STATE_OPERATIONAL_COMMAND_REQUEST :
        case EMBER_GPD_APP_STATE_OPERATIONAL_COMMAND_RECEIVED :
            taskTimeoutTicks = 0;
            sendToggle(gpd);
            emberGpdStoreSecDataToNV(gpd);
            break;

        case EMBER_GPD_APP_STATE_INVALID :
            emberGpdAfPluginDeCommission(gpd);
            break;

        default:
            //Wait for next external event
            break;
    }
    
    //emberGpdAfPluginMainCallback(gpdContext);

    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

  }
}

static void sendToggle(EmberGpd_t * gpd)
{
  uint8_t command[] = { GP_CMD_TOGGLE };
  emberAfGpdfSend(EMBER_GPD_NWK_FC_FRAME_TYPE_DATA,
                  gpd,
                  command,
                  sizeof(command),
                  EMBER_AF_PLUGIN_APPS_CMD_RESEND_NUMBER);
}