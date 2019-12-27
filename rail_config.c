/***************************************************************************//**
 * @brief RAIL Configuration
 * @details
 *   WARNING: Auto-Generated Radio Config  -  DO NOT EDIT
 *   Radio Configurator Version: 4.9.1
 *   RAIL Adapter Version: 2.4.5
 *   RAIL Compatibility: 2.x
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of  Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include "em_common.h"
#include "rail_config.h"

uint32_t RAILCb_CalcSymbolRate(RAIL_Handle_t railHandle)
{
  (void) railHandle;
  return 0U;
}

uint32_t RAILCb_CalcBitRate(RAIL_Handle_t railHandle)
{
  (void) railHandle;
  return 0U;
}

void RAILCb_ConfigFrameTypeLength(RAIL_Handle_t railHandle,
                                  const RAIL_FrameType_t *frameType)
{
  (void) railHandle;
  (void) frameType;
}

static const uint8_t irCalConfig[] = {
  25, 63, 1, 6, 4, 16, 1, 0, 0, 1, 1, 6, 0, 16, 39, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0
};

static RAIL_ChannelConfigEntryAttr_t channelConfigEntryAttr = {
  { 0xFFFFFFFFUL }
};

static const uint32_t phyInfo[] = {
  4UL,
  0x00666666UL, // 102.4
  (uint32_t) NULL,
  (uint32_t) irCalConfig,
#ifdef RADIO_CONFIG_ENABLE_TIMING
  (uint32_t) &Channel_Group_0_timing,
#else
  (uint32_t) NULL,
#endif
  0x00000000UL,
  8000000UL,
  20000000UL,
  1000000UL,
  (1UL << 8) | 1UL,
  0x02003911UL,
};

const uint32_t _2450M_1Mbps_modemConfigBase[] = {
  0x01041FF0UL, 0x0037003FUL,
  /*    1FF4 */ 0x00000000UL,
  /*    1FF8 */ (uint32_t) &phyInfo,
  /*    1FFC */ 0x00000000UL,
  0x00020004UL, 0x00000000UL,
  /*    0008 */ 0x00000000UL,
  0x00020018UL, 0x0000000FUL,
  /*    001C */ 0x00000000UL,
  0x00070028UL, 0x00000000UL,
  /*    002C */ 0x00000000UL,
  /*    0030 */ 0x00000000UL,
  /*    0034 */ 0x00000000UL,
  /*    0038 */ 0x00000000UL,
  /*    003C */ 0x00000000UL,
  /*    0040 */ 0x00000700UL,
  0x00010048UL, 0x00000000UL,
  0x00020054UL, 0x00000000UL,
  /*    0058 */ 0x00000000UL,
  0x000400A0UL, 0x00004CFFUL,
  /*    00A4 */ 0x00000000UL,
  /*    00A8 */ 0x00004DFFUL,
  /*    00AC */ 0x00000000UL,
  0x00012000UL, 0x00000744UL,
  0x00012010UL, 0x00000000UL,
  0x00012018UL, 0x0000A001UL,
  0x00013008UL, 0x0100AC13UL,
  0x00023030UL, 0x00103911UL,
  /*    3034 */ 0x00000001UL,
  0x00013040UL, 0x00000000UL,
  0x000140A0UL, 0x0F00277AUL,
  0x000140B8UL, 0x00F3C000UL,
  0x000140F4UL, 0x00001020UL,
  0x00024134UL, 0x00000880UL,
  /*    4138 */ 0x000087E6UL,
  0x00024140UL, 0x0088004BUL,
  /*    4144 */ 0x4D52E6C0UL,
  0x00044160UL, 0x00000000UL,
  /*    4164 */ 0x00000000UL,
  /*    4168 */ 0x00000006UL,
  /*    416C */ 0x00000006UL,
  0x00086014UL, 0x00000010UL,
  /*    6018 */ 0x04000000UL,
  /*    601C */ 0x0002C00FUL,
  /*    6020 */ 0x00003000UL,
  /*    6024 */ 0x000C5000UL,
  /*    6028 */ 0x03000000UL,
  /*    602C */ 0x00000000UL,
  /*    6030 */ 0x00000000UL,
  0x00066050UL, 0x00FF04C8UL,
  /*    6054 */ 0x00000841UL,
  /*    6058 */ 0x00020001UL,
  /*    605C */ 0x00140012UL,
  /*    6060 */ 0x0000B16FUL,
  /*    6064 */ 0x00000000UL,
  0x000C6078UL, 0x17800714UL,
  /*    607C */ 0x00000000UL,
  /*    6080 */ 0x002B03F1UL,
  /*    6084 */ 0x00000000UL,
  /*    6088 */ 0x00000000UL,
  /*    608C */ 0x22140A04UL,
  /*    6090 */ 0x4F4A4132UL,
  /*    6094 */ 0x00000000UL,
  /*    6098 */ 0x00000000UL,
  /*    609C */ 0x00000000UL,
  /*    60A0 */ 0x00000000UL,
  /*    60A4 */ 0x00000000UL,
  0x000760E4UL, 0xCBD70080UL,
  /*    60E8 */ 0x00000000UL,
  /*    60EC */ 0x07830464UL,
  /*    60F0 */ 0x3AC81388UL,
  /*    60F4 */ 0x0006209CUL,
  /*    60F8 */ 0x00206100UL,
  /*    60FC */ 0x208556B7UL,
  0x00036104UL, 0x0011C887UL,
  /*    6108 */ 0x00003020UL,
  /*    610C */ 0x0000BB88UL,
  0x00016120UL, 0x00000000UL,
  0x10017014UL, 0x0007F800UL,
  0x30017014UL, 0x000000F8UL,
  0x00067018UL, 0x00000300UL,
  /*    701C */ 0x84EA0060UL,
  /*    7020 */ 0x00000000UL,
  /*    7024 */ 0x00000082UL,
  /*    7028 */ 0x01800000UL,
  /*    702C */ 0x000000D5UL,
  0x00027048UL, 0x00003D3CUL,
  /*    704C */ 0x000019BCUL,
  0x00037070UL, 0x00220105UL,
  /*    7074 */ 0x00083010UL,
  /*    7078 */ 0x00552300UL,
  0xFFFFFFFFUL,
};

const RAIL_ChannelConfigEntry_t _2450M_1Mbps_channels[] = {
  {
    .phyConfigDeltaAdd = NULL,
    .baseFrequency = 2450000000,
    .channelSpacing = 1000000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 0,
    .channelNumberEnd = 20,
    .maxPower = RAIL_TX_POWER_MAX,
    .attr = &channelConfigEntryAttr
  },
};

const RAIL_ChannelConfig_t _2450M_1Mbps_channelConfig = {
  .phyConfigBase = _2450M_1Mbps_modemConfigBase,
  .phyConfigDeltaSubtract = NULL,
  .configs = _2450M_1Mbps_channels,
  .length = 1U,
  .signature = 0UL,
};

const RAIL_ChannelConfig_t *channelConfigs[] = {
  &_2450M_1Mbps_channelConfig,
  NULL
};

uint32_t AccelerationBuffer[181];
