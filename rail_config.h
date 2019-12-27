/***************************************************************************//**
 * @brief RAIL Configuration
 * @details
 *   WARNING: Auto-Generated Radio Config Header  -  DO NOT EDIT
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

#ifndef __RAIL_CONFIG_H__
#define __RAIL_CONFIG_H__

#include <stdint.h>
#include "rail_types.h"

#define _ACCELERATION_BUFFER AccelerationBuffer
extern uint32_t AccelerationBuffer[];

#define RADIO_CONFIG_XTAL_FREQUENCY 38400000UL

extern const RAIL_ChannelConfig_t *channelConfigs[];

#endif // __RAIL_CONFIG_H__
