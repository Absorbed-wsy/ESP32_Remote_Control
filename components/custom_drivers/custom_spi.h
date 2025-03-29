/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdint.h>
#include "esp_err.h"
#include "driver/spi_master.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SPI_HOST    SPI3_HOST

int custom_spi_init(uint8_t clk_pin, uint8_t mosi_pin, uint8_t miso_pin);
int custom_spi_deinit(void);
int custom_spi_transmit(uint8_t cs_pin, uint8_t mode, uint8_t clk_freq, spi_transaction_t *msgs);


#ifdef __cplusplus
}
#endif
