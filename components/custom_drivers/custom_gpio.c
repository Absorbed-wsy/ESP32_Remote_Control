/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <string.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_types.h"
#include "esp_log.h"
#include "esp_check.h"

#include "custom_gpio.h"

int custom_gpio_init(int mode, int pin)
{
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set mode
    io_conf.mode = mode;
    //bit mask of the pins
    io_conf.pin_bit_mask = pin;
    //disable pull-down mode
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    return 0;
}

int custom_gpio_deinit(int pin)
{
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set mode
    io_conf.mode = GPIO_MODE_INPUT;
    //bit mask of the pins
    io_conf.pin_bit_mask = pin;
    //disable pull-down mode
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    return 0;
}

uint8_t custom_get_gpio_value(int pin)
{
    return gpio_get_level(pin);
}

uint8_t custom_set_gpio_value(int pin, int value)
{
    gpio_set_level(pin, value);
    return 0;
}

