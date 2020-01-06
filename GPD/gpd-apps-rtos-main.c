
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

#include "em_prs.h"
#include "em_gpio.h"

static void sendToggle(EmberGpd_t * gpd);
void debug_init(void);
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

  volatile OS_TICK taskTimeoutTicks = 0;

  OSFlagPend(&proprietary_event_flags,
               INIT_FLAG,
               (OS_TICK)0,
               OS_OPT_PEND_BLOCKING       \
               + OS_OPT_PEND_FLAG_SET_ANY \
               + OS_OPT_PEND_FLAG_CONSUME,
               NULL,
               &err);

  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_PinModeSet(gpioPortF, 5, gpioModePushPull, 0);

  // Initialise NV
  emberGpdNvInit();//TODO as we are relying on the BLE stack NVM, need to make sure it was init first

  // Initialise timer for rxOffset timing during rxAfterTx
  emberGpdLeTimerInit();

  // Initialise Radio
  emberGpdRadioInit();

  //Initialise the Gpd
  EmberGpd_t * gpdContext = emberGpdInit();

  debug_init();

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
            emberGpdAfPluginCommission(gpdContext);
            emberGpdStoreSecDataToNV(gpdContext);
            taskTimeoutTicks = 900;
            break;

#ifdef MICRIUM_RTOS
        case EMBER_GPD_APP_STATE_COMMISSIONING_REPLY_RECIEVED_DECRYPT_KEY:
        	emberGpdAfPluginDecryptReceivedKey(gpdContext);
        	emberGpdStoreSecDataToNV(gpdContext);
        	taskTimeoutTicks = 900;
        	break;
#endif

        case EMBER_GPD_APP_STATE_OPERATIONAL :
        case EMBER_GPD_APP_STATE_OPERATIONAL_COMMAND_REQUEST :
        case EMBER_GPD_APP_STATE_OPERATIONAL_COMMAND_RECEIVED :
            taskTimeoutTicks = 0;
            sendToggle(gpdContext);
            emberGpdStoreSecDataToNV(gpdContext);
            break;

        case EMBER_GPD_APP_STATE_INVALID :
            emberGpdAfPluginDeCommission(gpdContext);
            break;

        default:
            //Wait for next external event
            break;
    }

    //TODO handle flag post timeout "error" here too
    //APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

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

void debug_init(void)
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

