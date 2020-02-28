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

#ifndef GPD_EVENTS_H
#define GPD_EVENTS_H

typedef enum gpd_evt_id_t {
  gpd_evt_id_none = 0,
  gpd_evt_id_commissioned,
  gpd_evt_id_decommissioned,
  gpd_evt_id_sent

} gpd_evt_id_t;


#endif //GPD_EVENTS_H
