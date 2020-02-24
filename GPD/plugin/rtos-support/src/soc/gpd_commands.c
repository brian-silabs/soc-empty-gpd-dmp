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

#include "gpd_commands.h"
#include "rtos_gpd.h"

uint16_t gpd_cmd_commission(void)
{

  global_gpd_cmd.id = gpd_cmd_id_commission;

  gpd_cmd_rtos_delegate();
  return global_gpd_rsp.result;
}

uint16_t gpd_cmd_decommission(void)
{

  global_gpd_cmd.id = gpd_cmd_id_decommission;

  gpd_cmd_rtos_delegate();
  return global_gpd_rsp.result;
}

uint16_t gpd_cmd_init(void)
{

  global_gpd_cmd.id = gpd_cmd_id_init;

  gpd_cmd_rtos_delegate();
  return global_gpd_rsp.result;
}

uint16_t gpd_cmd_send(uint8_t *data, uint16_t dataSize)
{

  global_gpd_cmd.id = gpd_cmd_id_send;
  global_gpd_cmd.data.data = data;
  global_gpd_cmd.data.dataSize = dataSize;

  gpd_cmd_rtos_delegate();
  return global_gpd_rsp.result;
}


