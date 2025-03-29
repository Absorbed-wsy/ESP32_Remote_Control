/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdint.h>
#include "esp_err.h"
#include "esp_adc/adc_continuous.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADC_GET_CHANNEL(p_data)     ((p_data)->type2.channel)
#define ADC_GET_DATA(p_data)        ((p_data)->type2.data)
#define _ADC_UNIT_STR(unit)         #unit
#define ADC_UNIT_STR(unit)          _ADC_UNIT_STR(unit)

int custom_adc_init(adc_channel_t *channel);
int custom_adc_deinit(void);
int custom_adc_get(uint32_t *buf);


#ifdef __cplusplus
}
#endif
