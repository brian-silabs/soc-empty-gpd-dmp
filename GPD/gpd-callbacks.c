/***************************************************************************//**
 * @file
 * @brief Application specific overrides of weak functions defined as part of
 * the test application.
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

#include "gpd-components-common.h"

#include "gpd-apps-rtos-main.h"
#include "rtos_gecko.h"


// ----------- GPD application Callbacks --------------------------------------
// This implements the following
// -- 1. NVM Storage (NVM3 or PSSTORE)for the application - NVM3 is used.
// -- 2. Bidirectional Send Receive Timing using
//         -- LETIMER0 running on LFRCO clock select on following branches
//           -- cmuClock_LFA for Series1
//           -- cmuClock_EM23GRPACLK for Series2
// -- 3. Application main loop timing using
//         -- Cryotimer for Series1
//         -- RTCC for Series 2
// -- 4. Application Button to run the main loop and command
// -- 5. Application main loop
//
// ----------- END :GPD application Callbacks ---------------------------------

// ----------------------------------------------------------------------------
// The following code is organised as per the above application functionality
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// ----------- 1. NVM usage in Application ------------------------------------
// ----------------------------------------------------------------------------
#define EMBER_GPD_NV_DATA_TAG 0xA9A1

#if defined EMBER_AF_PLUGIN_PSSTORE
extern void store_init(void);
static uint8_t *p;
static uint8_t length;
static uint8_t flags;
#elif defined EMBER_AF_PLUGIN_NVM3
#include "nvm3.h"
#endif
/** @brief This is called by framework to initialise the NVM system
 *
 */
void emberGpdAfPluginNvInitCallback(void)
{
  // Initialise the NV
#if defined EMBER_AF_PLUGIN_PSSTORE
  store_init();
#elif defined EMBER_AF_PLUGIN_NVM3
  // use defulalt NVM space and handle
  Ecode_t status = nvm3_open(nvm3_defaultHandle, nvm3_defaultInit);
  if (status != ECODE_NVM3_OK) {
    // Handle error
  }

  // Do repacking if needed
  if (nvm3_repackNeeded(nvm3_defaultHandle)) {
    status = nvm3_repack(nvm3_defaultHandle);
    if (status != ECODE_NVM3_OK) {
      // Handle error
    }
  }
#endif
}

/** @brief Called to the application to give a chance to load or store the GPD Context
 *.        in a non volatile context. This can help the application to use any other
 *         non volatile storage.
 *
 * @param nvmData The pointer to the data that needs saving or retrieving to or from
 *                the non-volatile memory.
 * @param sizeOfNvmData The size of the data non-volatile data.
 * @param loadStore indication wether to load or store the context.
 * Ver.: always
 *
 * @return true if application handled it.
 */
bool emberGpdAfPluginNvSaveAndLoadCallback(EmberGpd_t * gpd,
                                           uint8_t * nvmData,
                                           uint8_t sizeOfNvmData,
                                           EmebrGpdNvLoadStore_t loadStore)
{
	uint8_t * resp;

  if(sizeOfNvmData > 56)
    {
      while(1);
    }

  //Load data from NVM
  if (loadStore == EMEBER_GPD_AF_CALLBACK_LOAD_GPD_FROM_NVM) {
  #if defined EMBER_AF_PLUGIN_PSSTORE
    if (0 != store_read(EMBER_GPD_NV_DATA_TAG, &flags, &length, &p)) {
      // Fresh chip , erase, create a storage with default setting.
      store_eraseall();
      // First write to the NVM shadow so that it updated with default ones
      emberGpdCopyToShadow(gpd);
      // Write the data to NVM
      store_write(EMBER_GPD_NV_DATA_TAG,
                  flags,
                  sizeOfNvmData,
                  (void *)nvmData);
    } else {
      memcpy(nvmData, p, sizeOfNvmData);
    }
  #elif defined EMBER_AF_PLUGIN_NVM3
    uint32_t objectType;
    size_t dataLen;
    if (0 == nvm3_countObjects(nvm3_defaultHandle)
        || (nvm3_getObjectInfo(nvm3_defaultHandle,
                               EMBER_GPD_NV_DATA_TAG,
                               &objectType,
                               &dataLen) == ECODE_NVM3_OK
            && objectType != NVM3_OBJECTTYPE_DATA)) {
      // Fresh chip , erase, create a storage with default setting.
      // Erase all objects and write initial data to NVM3
      nvm3_eraseAll(nvm3_defaultHandle);
      // First write to the NVM shadow so that it updated with default ones
      emberGpdCopyToShadow(gpd);
      // Write the data to NVM
      nvm3_writeData(nvm3_defaultHandle,
                     EMBER_GPD_NV_DATA_TAG,
                     nvmData,
                     sizeOfNvmData);
    } else {
      nvm3_readData(nvm3_defaultHandle,
                    EMBER_GPD_NV_DATA_TAG,
                    nvmData,
                    sizeOfNvmData);
    }



  #endif

#ifdef MICRIUM_RTOS
resp = gecko_cmd_flash_ps_load(GREEN_POWER_PSSTORE_CONTEXT_KEY)->value.data;
#endif

  //Store data to NVM
  } else if (loadStore == EMEBER_GPD_AF_CALLBACK_STORE_GPD_TO_NVM) {
  #if defined EMBER_AF_PLUGIN_PSSTORE
    store_write(EMBER_GPD_NV_DATA_TAG,
                flags,
                sizeOfNvmData,
                (void *)nvmData);
  #elif defined EMBER_AF_PLUGIN_NVM3
    nvm3_writeData(nvm3_defaultHandle,
                   EMBER_GPD_NV_DATA_TAG,
                   nvmData,
                   sizeOfNvmData);
  #endif

#ifdef MICRIUM_RTOS
gecko_cmd_flash_ps_save(GREEN_POWER_PSSTORE_CONTEXT_KEY, sizeOfNvmData, nvmData);//56 bytes max
#endif

  } else {
    // bad command
  }
  return false;
}
// ----------------------------------------------------------------------------
// ----------- END : NVM usage in Application ---------------------------------
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// 2. Bidirectional timing with LE timer
// ----------------------------------------------------------------------------
// LE timer for the Low Energy Sleep during the RxOffset
static volatile bool leTimerRunning = false;

static const LETIMER_Init_TypeDef letimerInit = {
  .enable = false, /* Start counting when init completed*/
  .debugRun = false, /* Counter shall not keep running during debug halt. */
  .comp0Top = true, /* Load COMP0 register into CNT when counter underflows. COMP is used as TOP */
  .bufTop = false, /* Don't load COMP1 into COMP0 when REP0 reaches 0. */
  .out0Pol = 0, /* Idle value for output 0. */
  .out1Pol = 0, /* Idle value for output 1. */
  .ufoa0 = letimerUFOANone, /* PwM output on output 0 */
  .ufoa1 = letimerUFOANone, /* No output on output 1*/
  .repMode = letimerRepeatOneshot /* Count while REP != 0 */
};

bool emberGpdLeTimerRunning(void)
{
  return leTimerRunning;
}

void emberGpdLeTimerInit(void)
{
#if defined _SILICON_LABS_32B_SERIES_1_CONFIG_2
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
  CMU_ClockEnable(cmuClock_CORELE, true); /* Enable CORELE clock */
  CMU_ClockEnable(cmuClock_LETIMER0, true);
#elif defined EFR32_SERIES2_CONFIG1_MICRO || defined EFR32_SERIES2_CONFIG2_MICRO
  CMU_ClockSelectSet(cmuClock_EM23GRPACLK, cmuSelect_LFRCO);
  CMU_ClockEnable(cmuClock_LETIMER0, true);
#endif

  LETIMER_CompareSet(LETIMER0, 0, 0xFFFF);
  NVIC_EnableIRQ(LETIMER0_IRQn);
  LETIMER_IntEnable(LETIMER0, LETIMER_IF_UF);

  LETIMER_Init(LETIMER0, &letimerInit);
}

void LETIMER0_IRQHandler(void)
{
  LETIMER_IntClear(LETIMER0, LETIMER_IF_UF);
  leTimerRunning = false;
}

void emberGpdLoadLeTimer(uint32_t timeInUs)
{
  uint32_t matchValue = (timeInUs * 32768) / 1000000;
  LETIMER_Enable(LETIMER0, false);
  LETIMER_RepeatSet(LETIMER0, 0, 1);
  LETIMER_CompareSet(LETIMER0, 0, matchValue);
  LETIMER_IntClear(LETIMER0, LETIMER_IF_UF);
  LETIMER_Enable(LETIMER0, true);
  leTimerRunning = true;
}

void emberGpdAfPluginSleepCallback(void)
{
  // If Enters EM0 instead or awaken by other things, made to wait by following
  // code until the LE Timer expires to provide the exact rxOffset before receive.
  
  //TODO we remove it as this is implemented by the BLE stack
#ifndef MICRIUM_RTOS
  EMU_EnterEM2(true);
#endif
}
// ----------------------------------------------------------------------------
// ----------- END : Bidirectional timing with LE timer -----------------------
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// 5. Application main state machine.
// ----------------------------------------------------------------------------
/** @brief Called by framework from the application main enry to inform the user
 * as the first call to the main.
 *
 */
void emberGpdAfPluginMainCallback(EmberGpd_t * gpd)
{
 //Moved to gpd-apps-rtos-main.c
}
// ----------------------------------------------------------------------------
// ------------ END : Application main loop -----------------------------------
// ----------------------------------------------------------------------------
