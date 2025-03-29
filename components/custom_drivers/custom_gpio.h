/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdint.h>
#include "esp_err.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

int custom_gpio_init(int mode, int pin);
int custom_gpio_deinit(int pin);
uint8_t custom_get_gpio_value(int pin);
uint8_t custom_set_gpio_value(int pin, int value);



#ifdef __cplusplus
}
#endif
