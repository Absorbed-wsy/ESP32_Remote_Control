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

#include "custom_spi.h"

int custom_spi_init(uint8_t clk_pin, uint8_t mosi_pin, uint8_t miso_pin)
{
    spi_bus_config_t buscfg = {
        .miso_io_num = miso_pin,
        .mosi_io_num = mosi_pin,
        .sclk_io_num = clk_pin,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 1024
    };

    //Initialize the SPI bus
    spi_bus_initialize(SPI3_HOST, &buscfg, SPI_DMA_CH_AUTO);

    return 0;
}

int custom_spi_deinit(void)
{
    spi_bus_free(SPI3_HOST);
    return 0;
}

int custom_spi_transmit(uint8_t cs_pin, uint8_t mode, uint8_t clk_freq, spi_transaction_t *msgs)
{
    spi_device_handle_t spi_handle;
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = clk_freq * 1000 * 1000,       //Clock out MHz
        .mode = mode,                                   //SPI mode
        .spics_io_num = cs_pin,                         //CS pin
        .queue_size = 1,                                //We want to be able to queue 1 transactions at a time
    };

    //Attach the device to the SPI bus
    if(spi_bus_add_device(SPI3_HOST, &devcfg, &spi_handle) != ESP_OK) {
        return -1;
    }

    spi_transaction_t *msg = msgs;

    spi_device_polling_transmit(spi_handle, msg);       //Transmit!

    if(spi_bus_remove_device(spi_handle) != ESP_OK) {
        return -1;
    }

    return 0;
}

