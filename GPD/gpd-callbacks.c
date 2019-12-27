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
 *.        in a non volatile context. Thsi can help the application to use any other
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
#if defined EFR32_SERIES1_CONFIG2_MICRO
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
  EMU_EnterEM2(true);
}
// ----------------------------------------------------------------------------
// ----------- END : Bidirectional timing with LE timer -----------------------
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// 3. Application main loop timing with application timer
//               -- Series 1 : Cryotimer
//               -- Series 2 : RTCC
// ----------------------------------------------------------------------------
#if defined EFR32_SERIES1_CONFIG2_MICRO
static volatile uint32_t appTimeCount;
#else
#define appTimeCount (RAIL_GetTime() / 250000)
#endif

static void emberGpdCryoTimerInit(void)
{
  #if EFR32_SERIES1_CONFIG2_MICRO
  CMU_ClockEnable(cmuClock_CRYOTIMER, true);
  CRYOTIMER_Init_TypeDef  cryoInit = CRYOTIMER_INIT_DEFAULT;
  cryoInit.enable = false;
  cryoInit.osc = cryotimerOscULFRCO;
  cryoInit.presc = cryotimerPresc_1;
  cryoInit.period = cryotimerPeriod_256; //about 0.25s
  cryoInit.em4Wakeup = true;
  CRYOTIMER_Init(&cryoInit);

  NVIC_ClearPendingIRQ(CRYOTIMER_IRQn);
  NVIC_EnableIRQ(CRYOTIMER_IRQn);
  CRYOTIMER_IntClear(CRYOTIMER_IF_PERIOD);
  CRYOTIMER_IntDisable(CRYOTIMER_IEN_PERIOD);
  #endif
}

// Low Power Mode with option to force EM4 mode.
static void gpdEnterLowPowerMode(bool forceEm4)
{
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  if (forceEm4) {
    em4Init.retainLfxo = true;
    em4Init.pinRetentionMode = emuPinRetentionLatch;
    em4Init.em4State = emuEM4Shutoff;
    EMU_EM4Init(&em4Init);
    SLEEP_ForceSleepInEM4();
  } else {
    SLEEP_Sleep();
  }
}
// Application base loop timing with sleep
static void gpdSleepWithTimer(bool forceEm4)
{
#if defined EFR32_SERIES1_CONFIG2_MICRO
  // Start the Timer
  CRYOTIMER_IntClear(CRYOTIMER_IEN_PERIOD);
  CRYOTIMER_IntEnable(CRYOTIMER_IEN_PERIOD);
  CRYOTIMER_Enable(true);
#endif
  //Sleep when the device is commissioned
  // TODO : pass the flag based on the GPD state
  gpdEnterLowPowerMode(forceEm4);
}

// Cryotimer Intterrupt
void CRYOTIMER_IRQHandler(void)
{
#if defined EFR32_SERIES1_CONFIG2_MICRO
  CRYOTIMER_IntClear(CRYOTIMER_IF_PERIOD);
  appTimeCount++;
  __DSB();
#endif
}
// ----------------------------------------------------------------------------
// ----------- END : Application main loop timing -----------------------------
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// -------------- 4. Application Button ---------------------------------------
// ----------------------------------------------------------------------------
#define BUTTON_PRESSED  1
#define BUTTON_RELEASED 0

// Type defines
typedef struct ButtonArray{
  GPIO_Port_TypeDef   port;
  unsigned int        pin;
} ButtonArray_t;

static const ButtonArray_t buttonArray[BSP_NO_OF_BUTTONS] = BSP_GPIO_BUTTONARRAY_INIT;
static bool buttonPressed = false;
static unsigned int buttonPin = 0;
static uint32_t button0LongPressTimerStartValue = 0;

static void gpioCallback(uint8_t pin)
{
  // Check if any of the button pressed
  uint8_t buttonState = (GPIO_PinInGet(buttonArray[0].port, buttonArray[0].pin)) \
                        ? BUTTON_RELEASED : BUTTON_PRESSED;
  buttonState |= (GPIO_PinInGet(buttonArray[1].port, buttonArray[1].pin)) \
                 ? BUTTON_RELEASED : BUTTON_PRESSED;
  if (buttonState == BUTTON_PRESSED) {
    if (BSP_BUTTON0_PIN == pin) {
      // load a timer to count long press
      button0LongPressTimerStartValue = appTimeCount;
      buttonPressed = true;
      buttonPin = BSP_BUTTON0_PIN;
    } else if (BSP_BUTTON1_PIN == pin) {
      buttonPressed = true;
      buttonPin = BSP_BUTTON1_PIN;
    }
  } else {
    // Button released
    if (BSP_BUTTON0_PIN == pin) {
      button0LongPressTimerStartValue = 0;
    } else if (BSP_BUTTON1_PIN == pin) {
    }
  }
}
// ----------------------------------------------------------------------------
// ----------- END : Application Button ---------------------------------------
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// ------------Sending an operational command ---------------------------------
// ----------------------------------------------------------------------------
static void sendToggle(EmberGpd_t * gpd)
{
  uint8_t command[] = { GP_CMD_TOGGLE };
  emberAfGpdfSend(EMBER_GPD_NWK_FC_FRAME_TYPE_DATA,
                  gpd,
                  command,
                  sizeof(command),
                  EMBER_AF_PLUGIN_APPS_CMD_RESEND_NUMBER);
}
// ----------------------------------------------------------------------------
// ------------ END : Sending an operational command --------------------------
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
  // Initialise timer for application state machine with sleep consideration
  emberGpdCryoTimerInit();

  // Enable the buttons on the board
  for (int i = 0; i < BSP_NO_OF_BUTTONS; i++) {
    GPIO_PinModeSet(buttonArray[i].port, buttonArray[i].pin, gpioModeInputPull, 1);
  }

  // Button Interrupt Config
  GPIOINT_Init();
  GPIOINT_CallbackRegister(buttonArray[0].pin, gpioCallback);
  GPIOINT_CallbackRegister(buttonArray[1].pin, gpioCallback);
  GPIO_IntConfig(buttonArray[0].port, buttonArray[0].pin, true, true, true);
  GPIO_IntConfig(buttonArray[1].port, buttonArray[1].pin, false, true, true);

  buttonPressed = false;
  buttonPin = 0;

  // Loop forever
  while (true) {
//    if (buttonPressed) {
//      if (buttonPin == BSP_BUTTON0_PIN) {
//        emberGpdAfPluginCommission(gpd);
//      } else if (buttonPin == BSP_BUTTON1_PIN) {
//        sendToggle(gpd);
//      }
//      emberGpdStoreSecDataToNV(gpd);
//      buttonPin = 0;
//      buttonPressed = false;
//    }
//    uint32_t expiredTime = appTimeCount - button0LongPressTimerStartValue;
//    if (button0LongPressTimerStartValue
//        && expiredTime > GPD_APP_BUTTON_LONG_PRESS_TIME_IN_QS) {
//      button0LongPressTimerStartValue = 0;
//      emberGpdAfPluginDeCommission(gpd);
//    }
//    // Enter sleep with timer untill time to enter EM4
//    bool enterEm4 = (((appTimeCount > GPD_APP_TIME_IN_QS_TO_ENTER_EM4) \
//                      && (GPD_APP_TIME_IN_QS_TO_ENTER_EM4 != 0xFF)) ? true : false);
//    gpdSleepWithTimer(enterEm4);
  }
}
// ----------------------------------------------------------------------------
// ------------ END : Application main loop -----------------------------------
// ----------------------------------------------------------------------------
