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

#include "custom_i2c.h"

i2c_master_bus_handle_t master0_handle;
i2c_master_bus_handle_t master1_handle;

int custom_i2c_master_init(uint8_t i2c_port, uint8_t scl_pin, uint8_t sda_pin)
{
    i2c_master_bus_config_t i2c_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = i2c_port,
        .scl_io_num = scl_pin,
        .sda_io_num = sda_pin,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, (i2c_port == 0) ? &master0_handle : &master1_handle));
    
    return 0;
}

int custom_i2c_master_deinit(uint8_t i2c_port)
{
    i2c_del_master_bus((i2c_port == 0) ? master0_handle : master1_handle);
    
    return 0;
}
    
int custom_i2c_master_set(uint8_t i2c_port, uint8_t freq, uint8_t addr, uint8_t *data, uint8_t len)
{
    i2c_device_config_t i2c_dev_conf = {
        .scl_speed_hz = freq*1000,
        .device_address = addr,
    };

    i2c_master_dev_handle_t dev_handle;

    if (i2c_master_bus_add_device((i2c_port == 0) ? master0_handle : master1_handle, &i2c_dev_conf, &dev_handle) != ESP_OK) {
        return -1;
    }

    i2c_master_transmit(dev_handle, data, len, 100);

    if (i2c_master_bus_rm_device(dev_handle) != ESP_OK) {
        return -1;
    }

    return 0;
}

int custom_i2c_master_get(uint8_t i2c_port, uint8_t freq, uint8_t addr, uint8_t maddr, uint8_t *data, uint8_t len)
{
    uint8_t buf[512] = {0};

    i2c_device_config_t i2c_dev_conf = {
        .scl_speed_hz = freq*1000,
        .device_address = addr,
    };

    i2c_master_dev_handle_t dev_handle;

    if (i2c_master_bus_add_device((i2c_port == 0) ? master0_handle : master1_handle, &i2c_dev_conf, &dev_handle) != ESP_OK) {
        return -1;
    }

    i2c_master_transmit_receive(dev_handle, &maddr, 1, buf, len, 100);

    memcpy(data, buf, len*sizeof(uint8_t));

    if (i2c_master_bus_rm_device(dev_handle) != ESP_OK) {
        return -1;
    }

    return 0;
}
