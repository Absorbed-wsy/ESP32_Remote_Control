/*
* SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
*
* SPDX-License-Identifier: Apache-2.0
*/
#include <string.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_log.h"

#include "custom_adc.h"

adc_continuous_handle_t adc_handle;

int custom_adc_init(adc_channel_t *channel)
{
    uint8_t channel_num = 1;

    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = 256,
        .conv_frame_size = 64,
    };
    adc_continuous_new_handle(&adc_config, &adc_handle);

    adc_continuous_config_t dig_cfg = {
        .sample_freq_hz = 20 * 1000,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE2,
    };

    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
    dig_cfg.pattern_num = channel_num;
    for (int i = 0; i < channel_num; i++) {
        adc_pattern[i].atten = ADC_ATTEN_DB_0;
        adc_pattern[i].channel = channel[i] & 0x7;
        adc_pattern[i].unit = ADC_UNIT_1;
        adc_pattern[i].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;

        ESP_LOGI("Init", "adc_pattern[%d].atten is :%"PRIx8, i, adc_pattern[i].atten);
        ESP_LOGI("Init", "adc_pattern[%d].channel is :%"PRIx8, i, adc_pattern[i].channel);
        ESP_LOGI("Init", "adc_pattern[%d].unit is :%"PRIx8, i, adc_pattern[i].unit);
    }
    dig_cfg.adc_pattern = adc_pattern;

    adc_continuous_config(adc_handle, &dig_cfg);

    adc_continuous_start(adc_handle);

    return 0;
}
int custom_adc_deinit(void)
{
    adc_continuous_deinit(adc_handle);
    return 0;
}

int custom_adc_get(uint32_t *buf)
{
    uint32_t ret_num = 0;
    uint8_t result[64];
    char unit[] = ADC_UNIT_STR(ADC_UNIT_1);

    adc_continuous_read(adc_handle, result, 64, &ret_num, 1000);

    uint32_t data=0;
    uint32_t chan_num=0;

    for (int i = 0; i < ret_num; i += SOC_ADC_DIGI_RESULT_BYTES) {
        adc_digi_output_data_t *p = (adc_digi_output_data_t*)&result[i];
        chan_num = ADC_GET_CHANNEL(p);
        data = ADC_GET_DATA(p);
        /* Check the channel number validation, the data is invalid if the channel num exceed the maximum channel */
        if (chan_num < SOC_ADC_CHANNEL_NUM(ADC_UNIT_1)) {
            ESP_LOGI("main", "Unit: %s, Channel: %"PRIu32", Value: %"PRIx32, unit, chan_num, data);
        } else {
            ESP_LOGW("main", "Invalid data [%s_%"PRIu32"_%"PRIx32"]", unit, chan_num, data);
        }
    }

    memcpy(buf, &data, sizeof(uint32_t));

    return 0;
}

