/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdint.h>
#include "esp_err.h"
#include "driver/i2c_master.h"
#include "driver/i2c_slave.h"

#ifdef __cplusplus
extern "C" {
#endif

int custom_i2c_master_init(uint8_t i2c_port, uint8_t scl_pin, uint8_t sda_pin);
int custom_i2c_master_deinit(uint8_t i2c_port);
int custom_i2c_master_set(uint8_t i2c_port, uint8_t freq, uint8_t addr, uint8_t *data, uint8_t len);
int custom_i2c_master_get(uint8_t i2c_port, uint8_t freq, uint8_t addr, uint8_t maddr, uint8_t *data, uint8_t len);


#ifdef __cplusplus
}
#endif
