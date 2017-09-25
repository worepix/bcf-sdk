#ifndef _BC_MODULE_CO2_H
#define _BC_MODULE_CO2_H

#include <bc_tick.h>
#include <bc_co2_sensor.h>

//! @addtogroup bc_module_co2 bc_module_co2
//! @brief Driver for BigClown CO2 Module
//! @{

//! @brief Callback events

typedef enum
{
    //! @brief Error event
    BC_MODULE_CO2_EVENT_ERROR = BC_CO2_SENSOR_EVENT_ERROR,

    //! @brief Update event
    BC_MODULE_CO2_EVENT_UPDATE = BC_CO2_SENSOR_EVENT_UPDATE

} bc_module_co2_event_t;

//! @brief Initialize BigClown CO2 Module

void bc_module_co2_init(void);

//! @brief Set callback function
//! @param[in] event_handler Function address
//! @param[in] event_param Optional event parameter (can be NULL)

void bc_module_co2_set_event_handler(void (*event_handler)(bc_module_co2_event_t, void *), void *event_param);

//! @brief Set measurement interval
//! @param[in] interval Measurement interval

void bc_module_co2_set_update_interval(bc_tick_t interval);

//! @brief Start measurement manually
//! @return true On success
//! @return False When other measurement is in progress

bool bc_module_co2_measure(void);

//! @brief Get CO2 concentration in ppm (parts per million)
//! @param[out] ppm Pointer to variable where result will be stored
//! @return true On success
//! @return false On failure

bool bc_module_co2_get_concentration_ppm(float *ppm);

//! @brief Request sensor calibration
//! @param[in] calibration Calibration type

void bc_module_co2_calibration(bc_co2_sensor_calibration_t calibration);

//! @}

#endif // _BC_MODULE_CO2_H
