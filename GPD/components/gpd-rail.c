/***************************************************************************//**
 * @file
 * @brief Routines built on RAIL functions used by the GPD.
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

#ifdef MICRIUM_RTOS
#include "rtos_gpd.h"
#include "os.h"
#include "em_core.h"
#endif

// Function prototype
static void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events);

static RAIL_Handle_t railHandle = NULL;

#ifdef BLE_DMP_SUPPORT
static RAILSched_Config_t railSchedState;
static RAIL_SchedulerInfo_t rxSchedInfo = 
{
  .priority = 0,
  .slipTime = 0,
  .transactionTime = EMBER_AF_PLUGIN_APPS_RX_WINDOW * 1000
};

static RAIL_SchedulerInfo_t txSchedInfo = 
{
  .priority = 0,
  .slipTime = 10000,
  .transactionTime = 2000
};
#endif

static bool     rfReady = false;
static RAIL_Config_t railCfg = {
  .eventsCallback = &RAILCb_Generic,
  .protocol = NULL,
#ifdef BLE_DMP_SUPPORT
  .scheduler = &railSchedState
#endif
};

static uint8_t railTxFifo[GP_FIFO_SIZE];
#ifndef BLE_DMP_SUPPORT
static uint8_t railRxFifo[GP_FIFO_SIZE];
#endif

static void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events)
{
  (void)railHandle;

#ifdef MICRIUM_RTOS
  CORE_irqState_t irqState = CORE_EnterCritical();
  OSIntEnter();
  CORE_ExitCritical(irqState);
#endif

  if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
    CORE_irqState_t c = CORE_EnterCritical();
    uint16_t bytesAvailable = RAIL_GetRxFifoBytesAvailable(railHandle);
    uint16_t rxReceived = RAIL_ReadRxFifo(railHandle, emberGpdGetRxMpdu(), bytesAvailable);
#ifdef MICRIUM_RTOS
    gpd_ll_evt.type = gpd_ll_event_packet_received;//TODO Create an API for this one
    gpd_ll_evt.data.data = emberGpdGetRxMpdu();
    gpd_ll_evt.data.dataSize = rxReceived;

    GpdLLCallback();
#else
    emberGpdIncomingMessageHandler(emberGpdGetRxMpdu(), rxReceived);
#endif
    CORE_ExitCritical(c);
#ifdef BLE_DMP_SUPPORT
    RAIL_YieldRadio(railHandle);
#endif
  }
  if (events & RAIL_EVENT_TX_PACKET_SENT) {
#ifdef BLE_DMP_SUPPORT
    RAIL_YieldRadio(railHandle);
#endif
  }
#ifdef BLE_DMP_SUPPORT
  if (events & RAIL_EVENT_SCHEDULER_STATUS)
  {

  }
#endif

#ifdef MICRIUM_RTOS
OSIntExit();
#endif

}

static void RAIL_CbRfReady(RAIL_Handle_t railHandle)
{
  rfReady = true;
}

void emberGpdRailStartRxWrapper(uint8_t channel)
{
  RAIL_StartRx(railHandle, channel, &rxSchedInfo);
}

void emberGpdRailIdleWrapper(void)
{
  RAIL_Idle(railHandle, RAIL_IDLE, true);
}

void emberGpdRailStartTxWrapper(bool skipCca,
                                uint16_t channel)
{
  RAIL_Status_t status;
  if (skipCca) {
    status = RAIL_StartTx(railHandle,
                          channel,
                          RAIL_TX_OPTIONS_DEFAULT,
                          &txSchedInfo);
  } else {
    RAIL_CsmaConfig_t txCsmaOptions = RAIL_CSMA_CONFIG_802_15_4_2003_2p4_GHz_OQPSK_CSMA;
    status = RAIL_StartCcaCsmaTx(railHandle,
                                 channel,
                                 RAIL_TX_OPTIONS_DEFAULT,
                                 &txCsmaOptions,
                                 &txSchedInfo);
  }
  if (status != 0) {
    while (true) ; // Error - Trap it
  }
}

void emberGpdRailWriteTxFifoWrapper(const uint8_t *dataPtr,
                                    uint16_t writeLength)
{
  uint16_t writtenLength = RAIL_WriteTxFifo(railHandle,
                                            dataPtr,
                                            writeLength,
                                            true);
  if (writtenLength != writeLength) {
    while (true) ;
  }
}

void emberGpdRadioInit(void)
{
  railHandle = RAIL_Init(&railCfg, RAIL_CbRfReady);
  if (railHandle == NULL) {
    while (true) ;
  }

  RAIL_TxPowerConfig_t txPowerConfig = {
#if HAL_PA_2P4_LOWPOWER
    .mode = RAIL_TX_POWER_MODE_2P4_LP,
#else
    .mode = RAIL_TX_POWER_MODE_2P4_HP,
#endif
    .voltage = BSP_PA_VOLTAGE,
    .rampTime = HAL_PA_RAMP,
  };

  // Initialise the PA now that the HFXO is up and the timing is correct
  if (RAIL_ConfigTxPower(railHandle, &txPowerConfig) != RAIL_STATUS_NO_ERROR) {
    // Error: The PA could not be initialised due to an improper configuration.
    // Please ensure your configuration is valid for the selected part.
    while (true) ;
  }
  RAIL_TxPower_t txDefaultPower = 0; // 100 deci-dBm, 10 dBm
  // Set default TX power in raw
  if (RAIL_SetTxPowerDbm(railHandle, txDefaultPower) != RAIL_STATUS_NO_ERROR) {
    while (true) ;
  }

  // Set TX FIFO, and verify that the size is correct
  uint16_t fifoSize = RAIL_SetTxFifo(railHandle, railTxFifo, 0, GP_FIFO_SIZE);
  if (fifoSize != GP_FIFO_SIZE) {
    while (true) ;
  }

#ifndef BLE_DMP_SUPPORT
  //In multiprotocol, RAIL currently shares one receive FIFO across all protocols. This function will return RAIL_STATUS_INVALID_STATE if the requested RAIL_Handle_t is not active.
  // Set RX FIFO, and verify that the size is correct
  RAIL_Status_t status = RAIL_SetRxFifo(railHandle, railRxFifo, &fifoSize);
  if (fifoSize != GP_FIFO_SIZE
      || !(status == RAIL_STATUS_NO_ERROR)) {
    while (true) ;
  }
#endif

  // Initialise Radio Calibrations
  if (RAIL_ConfigCal(railHandle, RAIL_CAL_ALL) != RAIL_STATUS_NO_ERROR) {
    while (true) ;
  }

  // Configure all RAIL events with appended info
  RAIL_Events_t events =    RAIL_EVENTS_RX_COMPLETION
                          | RAIL_EVENTS_TX_COMPLETION;
  if (RAIL_ConfigEvents(railHandle, RAIL_EVENTS_ALL, events) != RAIL_STATUS_NO_ERROR) {
    while (true) ;
  }

  RAIL_StateTransitions_t transitions = {
    .success = RAIL_RF_STATE_IDLE,
    .error = RAIL_RF_STATE_IDLE
  };
  if (RAIL_SetRxTransitions(railHandle, &transitions) != RAIL_STATUS_NO_ERROR) {
    while (true) ;
  }
  // Initialise IEEE
  if (!RAIL_IEEE802154_IsEnabled(railHandle)) {
    //This initialises the radio for 2.4GHz operation.
    if (RAIL_IEEE802154_Config2p4GHzRadio(railHandle) != RAIL_STATUS_NO_ERROR) {
      while (true) ;
    }
    RAIL_StateTiming_t timings = {
      .idleToTx = 100,
      .idleToRx = 100,
      .rxToTx = 192,
      // Make txToRx slightly lower than desired to make sure we get to
      // RX in time
      .txToRx = 192 - 10,
      .rxSearchTimeout = 0,
      .txToRxSearchTimeout = 0
    };
    RAIL_IEEE802154_Config_t rail802154_init = {
      .addresses = NULL,
      .ackConfig = {
        .enable = false,
        .ackTimeout = 0,
        .rxTransitions = {
          .success = RAIL_RF_STATE_IDLE,
          .error = RAIL_RF_STATE_IDLE // ignored
        },
        .txTransitions = {
          .success = RAIL_RF_STATE_IDLE,
          .error = RAIL_RF_STATE_IDLE // ignored
        }
      },
      .timings = timings,
      .framesMask = RAIL_IEEE802154_ACCEPT_STANDARD_FRAMES  \
                    | RAIL_IEEE802154_ACCEPT_ACK_FRAMES     \
                    | RAIL_IEEE802154_ACCEPT_DATA_FRAMES    \
                    | RAIL_IEEE802154_ACCEPT_COMMAND_FRAMES \
                    | RAIL_IEEE802154_ACCEPT_BEACON_FRAMES,
      .promiscuousMode = true,
      .isPanCoordinator = false
    };
    if (RAIL_IEEE802154_Init(railHandle, &rail802154_init) != RAIL_STATUS_NO_ERROR) {
      while (true) ;
    }
    // Write the PAN ID as 0xFFFF
    if (RAIL_IEEE802154_SetPanId(railHandle, 0xFFFF, 0x00) != RAIL_STATUS_NO_ERROR) {
      while (true) ;
    }
#if defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_IEEE_ID)
    // Write the IEEE address if any
    #if defined EMBER_AF_PLUGIN_APPS_IEEE
    uint8_t gpdIeee[] = EMBER_AF_PLUGIN_APPS_IEEE;
    #else
    uint8_t *gpdIeee = (uint8_t*)(&SYSTEM_GetUnique());
    #endif
    if (RAIL_IEEE802154_SetLongAddress(railHandle, gpdIeee, 0x00) != RAIL_STATUS_NO_ERROR) {
      while (true) ;
    }
#endif
  }
  while (!rfReady) ;
}
