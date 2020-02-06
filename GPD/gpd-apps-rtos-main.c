
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

#include "rtos_gecko.h"

#ifdef DEBUG_RADIO
#include "em_prs.h"
#include "em_gpio.h"
#endif

///////////////////////////////////////////////////////////////////////////////
////////////////////////////// RTOS related ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/* Dummy flag to prevent cyclic execution of the proprietary task function code.
 * this flag will not be posted by default. */
#define INIT_FLAG                           ((OS_FLAGS)0x01)
#define COMMISSIONING_FLAG                  ((OS_FLAGS)0x02)
#define DECOMMISSIONING_FLAG                ((OS_FLAGS)0x04)
#define OPERATE_FLAG                        ((OS_FLAGS)0x08)

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////// NVM related ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define OS_TICK_RATE_HZ 					          1024u //Check rtos_bluetooth.c - I have no idea where this is set since we are using dyn tick & sleeptimer

#define GREEN_POWER_TASK_TIMEOUT_MS         250u
#define GREEN_POWER_TASK_TIMEOUT_TICKS      ((uint32_t)((GREEN_POWER_TASK_TIMEOUT_MS/1000u)*OS_TICK_RATE_HZ))


static uint8_t  gpd_Command     (OS_FLAGS flags);

static void     sendToggle      (EmberGpd_t * gpd);

#ifdef DEBUG_RADIO
static void debug_init(void);
#endif

static EmberGpd_t * gpdContext;
///////////////////////////////////////////////////////////////////////////////
/////////////////////////// Public API definition /////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int8_t GPD_Init(void)
{
  int8_t error = 0;

  // Initialise NV
  emberGpdNvInit();//TODO as we are relying on the BLE stack NVM, need to make sure it was init first

  // Initialise timer for rxOffset timing during rxAfterTx
  emberGpdLeTimerInit();

  // Initialise Radio
  emberGpdRadioInit();

  //Initialise the Gpd
  gpdContext = emberGpdInit();

#ifdef DEBUG_RADIO
  debug_init();
#endif
  return error;
}

int8_t GPD_StartCommissioning(void)
{
  int8_t error = 0;
  if(gpdContext->gpdState != EMBER_GPD_APP_STATE_NOT_COMMISSIONED)
  {
    //Error device already in commissioning process or commissioned
    error = (-1);
  } else {
    emberGpdSetState(EMBER_GPD_APP_STATE_CHANNEL_REQUEST);
    error = gpd_Command(COMMISSIONING_FLAG);
  }
  
  return error;
}

//Can be called anytime, and be used as a reset in case of bad state
int8_t GPD_DeCommission(void)
{
  int8_t error = 0;
  if(gpdContext->gpdState != EMBER_GPD_APP_STATE_OPERATIONAL)
  {
    //Error device already in commissioning process or commissioned
    error = (-1);
  } else {
    emberGpdSetState(EMBER_GPD_APP_STATE_INVALID);
    error = gpd_Command(DECOMMISSIONING_FLAG);
  }
  return error;
}

int8_t GPD_Toggle(void)
{
  int8_t error = 0;
  emberGpdSetState(EMBER_GPD_APP_STATE_OPERATIONAL_COMMAND_REQUEST);
  error = gpd_Command(OPERATE_FLAG);
  return error;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////// Private API definition /////////////////////////////
///////////////////////////////////////////////////////////////////////////////

//static uint8_t gpd_Command(OS_FLAGS flags)
//{
//	RTOS_ERR osErr;
//  //Enables the GPD commissioning process
//  OSFlagPost(&proprietary_event_flags,
//        (OS_FLAGS)flags,
//        OS_OPT_POST_FLAG_SET,
//        &osErr);
//
//  return 0;
//}
//
//static void gpd_SignalEvent(gpdEvent_t event)
//{
//  gecko_external_signal(event);
//}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////// Private API definition /////////////////////////////
///////////////////////////////////////////////////////////////////////////////


static void sendToggle(EmberGpd_t * gpd)
{
  uint8_t command[] = { GP_CMD_TOGGLE };
  emberAfGpdfSend(EMBER_GPD_NWK_FC_FRAME_TYPE_DATA,
                  gpd,
                  command,
                  sizeof(command),
                  EMBER_AF_PLUGIN_APPS_CMD_RESEND_NUMBER);
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////// Private API definition /////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG_RADIO
static void debug_init(void)
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
#endif
