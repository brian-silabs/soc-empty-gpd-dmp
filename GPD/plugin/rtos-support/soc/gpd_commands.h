/***************************************************************************//**
 * @brief Adaptation for running Green Power Device in RTOS
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

#ifndef GPD_COMMANDS_H
#define GPD_COMMANDS_H

#include <stdint.h>

typedef enum gpd_cmd_id_t {
  gpd_cmd_id_init = 0,
  gpd_cmd_id_commission,
  gpd_cmd_id_decommission,
  gpd_cmd_id_send

} gpd_cmd_id_t;

uint16_t gpd_cmd_commission(void);
uint16_t gpd_cmd_decommission(void);
uint16_t gpd_cmd_init(void);
uint16_t gpd_cmd_send(uint8_t *data, uint16_t dataSize);

#endif //GPD_EVENTS_H
