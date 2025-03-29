#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "custom_gpio.h"
#include "custom_i2c.h"
#include "custom_spi.h"
#include "custom_adc.h"
#include "custom_pwm.h"

// 将数组转换为表格格式的字符串数组
char **dump_array_to_strings(const uint8_t *data, size_t len, size_t *num_rows) 
{
    // 计算行数
    *num_rows = (len / 16) + (len % 16 != 0 ? 1 : 0);

    // 分配字符串数组
    char **rows = (char **)malloc(*num_rows * sizeof(char *));
    if (rows == NULL) {
        perror("Failed to allocate memory for rows");
        return NULL;
    }

    // 每行最多需要 80 字节（地址 + 16 个数据 + 换行符 + 结束符）
    const size_t max_row_size = 80;

    // 填充字符串数组
    for (size_t i = 0; i < *num_rows; i++) {
        rows[i] = (char *)malloc(max_row_size);
        if (rows[i] == NULL) {
            perror("Failed to allocate memory for row");
            // 释放之前分配的内存
            for (size_t j = 0; j < i; j++) {
                free(rows[j]);
            }
            free(rows);
            return NULL;
        }

        // 格式化当前行
        size_t offset = i * 16;
        size_t row_len = (len - offset) > 16 ? 16 : (len - offset);
        char *row = rows[i];
        int pos = 0;

        // 打印地址
        pos += sprintf(row + pos, "%04zx: ", offset);

        // 打印数据
        for (size_t j = 0; j < row_len; j++) {
            pos += sprintf(row + pos, "%02x ", data[offset + j]);
        }

        // 添加换行符
        sprintf(row + pos, "\n");
    }

    return rows;
}

static char* cmd_gpio(char *cmd)
{
    char *response = NULL;

    char *token = strtok(cmd, " ");
    token = strtok(NULL, " ");
    if (token == NULL)
        return response = "Command error: No operation type provided";

    if (strcmp(token, "init") == 0) {
        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No index provided";
        int index = atoi(token);

        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No mode provided";
        int mode = (strcmp(token, "in") == 0) ? GPIO_MODE_INPUT : GPIO_MODE_OUTPUT;

        custom_gpio_init(mode, (1<<index));
        response = "Command success: GPIO init successfully";

    } else if (strcmp(token, "deinit") == 0) {
        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No index provided";
        int index = atoi(token);

        custom_gpio_deinit(1<<index);
        response = "Command success: GPIO deinit successfully";

    } else if (strcmp(token, "set") == 0) {
        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No index provided";
        int index = atoi(token);

        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No value provided";
        int value = atoi(token);

        custom_set_gpio_value((index), value);
        response = "Command success: GPIO set successfully";

    } else if (strcmp(token, "get") == 0) {
        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No index provided";
        int index = atoi(token);

        asprintf(&response, "Command success: GPIO value retrieved = %d ", custom_get_gpio_value(index));

    } else {
        response = "Command error: Invalid operation type";
    }

    return response;
}

static char* cmd_i2c(char *cmd)
{
    char *response = NULL;

    char *token = strtok(cmd, " ");
    token = strtok(NULL, " ");
    if (token == NULL)
        return response = "Command error: No operation type provided";

    if (strcmp(token, "init") == 0) {
        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No index provided";
        int index = atoi(token);

        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No scl pin provided";
        int scl = atoi(token);

        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No sda pin provided";
        int sda = atoi(token);

        custom_i2c_master_init(index, scl, sda);
        asprintf(&response, "Command success: i2c-%d init successfully", index);

    } else if (strcmp(token, "deinit") == 0) {
        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No index provided";
        int index = atoi(token);

        custom_i2c_master_deinit(index);
        asprintf(&response, "Command success: i2c-%d deinit successfully", index);

    } else if (strcmp(token, "set") == 0) {
        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No index provided";
        int index = atoi(token);

        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No daddr provided";
        char *endptr;
        int daddr = strtol(token, &endptr, 16);

        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No len provided";
        int len = atoi(token);

        uint8_t *data = (uint8_t *)pvPortMalloc(len * sizeof(uint8_t));
        for(size_t i=0; i<len; i++) {
            token = strtok(NULL, " ");
            if (token == NULL) {
                return response = "Command error: No value provided";
                break;
            }
            data[i] = strtol(token, &endptr, 16);
        }

        int freq = 100;
        token = strtok(NULL, " ");
        if (token != NULL)
            freq = atoi(token);

        custom_i2c_master_set(index, freq, daddr, data, len);

        response = "Command success: i2c set successfully";
        vPortFree(data);

    } else if (strcmp(token, "get") == 0) {
        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No index provided";
        int index = atoi(token);

        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No daddr provided";
        char *endptr;
        int daddr = strtol(token, &endptr, 16);

        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No len provided";
        int len = atoi(token);

        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No value/maddr provided";
        int maddr = strtol(token, &endptr, 16);

        int freq = 100;
        token = strtok(NULL, " ");
        if (token != NULL)
            freq = atoi(token);

        uint8_t *data = (uint8_t *)pvPortMalloc(len * sizeof(uint8_t));
        memset(data, 0, len);

        custom_i2c_master_get(index, freq, daddr, maddr, data, len);

        size_t num_rows;
        char **rows = dump_array_to_strings(data, len, &num_rows);

        char *concatenated_rows = NULL;

        for (size_t i = 0; i < num_rows; i++) {
            if (concatenated_rows == NULL) {
                asprintf(&concatenated_rows, "%s", rows[i]);
            } else {
                char *temp = concatenated_rows;
                asprintf(&concatenated_rows, "%s%s", temp, rows[i]);
                vPortFree(temp);
            }
        }

        asprintf(&response, "Command success: i2c value retrieved =\n%s", concatenated_rows);

        vPortFree(data);
        for (size_t i = 0; i < num_rows; i++) {
            vPortFree(rows[i]);
        }
        vPortFree(rows);

    } else {
        response = "Command error: Invalid operation type";
    }

    return response;
}

static char* cmd_spi(char *cmd)
{
    char *response = NULL;

    char *token = strtok(cmd, " ");
    token = strtok(NULL, " ");
    if (token == NULL)
        return response = "Command error: No operation type provided";

    if (strcmp(token, "init") == 0) {
        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No clk pin provided";
        int clk_pin = atoi(token);

        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No mosi pin provided";
        int mosi_pin = atoi(token);

        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No miso pin provided";
        int miso_pin = atoi(token);

        custom_spi_init(clk_pin, mosi_pin, miso_pin);
        asprintf(&response, "Command success: spi-3 init successfully");

    } else if (strcmp(token, "deinit") == 0) {
        custom_spi_deinit();
        asprintf(&response, "Command success: spi-3 deinit successfully");

    } else if (strcmp(token, "ts") == 0) {
        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No cs pin provided";
        int cs_pin = atoi(token);

        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No spi mode provided";
        int mode = atoi(token);

        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No rx len provided";
        int rx_len = atoi(token);

        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No tx len provided";
        int tx_len = atoi(token); 

        char *endptr;
        uint8_t *tx_buf = (uint8_t *)pvPortMalloc(tx_len * sizeof(uint8_t));
        for(size_t i=0; i<tx_len; i++) {
            token = strtok(NULL, " ");
            if (token == NULL) {
                return response = "Command error: No value provided";
                break;
            }
            tx_buf[i] = strtol(token, &endptr, 16);
        }

        int freq = 10;
        token = strtok(NULL, " ");
        if (token != NULL)
            freq = atoi(token);

        uint8_t *rx_buf = (uint8_t *)pvPortMalloc(rx_len * sizeof(uint8_t));

        spi_transaction_t msg = {0};
        msg.length = rx_len *8;
        msg.rxlength = rx_len *8;
        msg.tx_buffer = tx_buf;
        msg.rx_buffer = rx_buf;

        custom_spi_transmit(cs_pin, mode, freq, &msg);

        size_t num_rows;
        char **rows = dump_array_to_strings(rx_buf, rx_len, &num_rows);

        char *concatenated_rows = NULL;

        for (size_t i = 0; i < num_rows; i++) {
            if (concatenated_rows == NULL) {
                asprintf(&concatenated_rows, "%s", rows[i]);
            } else {
                char *temp = concatenated_rows;
                asprintf(&concatenated_rows, "%s%s", temp, rows[i]);
                vPortFree(temp);
            }
        }

        asprintf(&response, "Command success: spi value retrieved =\n%s", concatenated_rows);

        for (size_t i = 0; i < num_rows; i++) {
            vPortFree(rows[i]);
        }
        vPortFree(rows);
        vPortFree(tx_buf);
        vPortFree(rx_buf);

    } else {
        response = "Command error: Invalid operation type";
    }

    return response;
}

static char* cmd_adc(char *cmd)
{
    char *response = NULL;

    char *token = strtok(cmd, " ");
    token = strtok(NULL, " ");
    if (token == NULL)
        return response = "Command error: No operation type provided";

    if (strcmp(token, "init") == 0) {
        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No channel provided";
        adc_channel_t channel = atoi(token);

        custom_adc_init(&channel);
        asprintf(&response, "Command success: ADC1 Channel-%d init successfully",channel);

    } else if (strcmp(token, "deinit") == 0) {
        custom_adc_deinit();
        asprintf(&response, "Command success: ADC1 deinit successfully");

    } else if (strcmp(token, "get") == 0) {
        uint32_t data;
        custom_adc_get(&data);

        float v = (data*3.3)/4096;

        asprintf(&response, "Command success: ADC retrieved = %f V", v);

    } else {
        response = "Command error: Invalid operation type";
    }

    return response;
}

static char* cmd_pwm(char *cmd)
{
    char *response = NULL;

    char *token = strtok(cmd, " ");
    token = strtok(NULL, " ");
    if (token == NULL)
        return response = "Command error: No operation type provided";

    if (strcmp(token, "init") == 0) {
        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No channel provided";
        uint8_t channel = atoi(token);

        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No out pin provided";
        uint8_t pin = atoi(token);

        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No freq provided";
        uint32_t freq = atoi(token);

        custom_pwm_init(channel, pin, freq);
        asprintf(&response, "Command success: PWM Channel-%d Pin-%d Freq-%ld init successfully",channel, pin, freq);

    } else if (strcmp(token, "set") == 0) {
        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No channel provided";
        uint8_t channel = atoi(token);

        token = strtok(NULL, " ");
        if (token == NULL)
            return response = "Command error: No duty provided";
        uint8_t duty = atoi(token);

        custom_pwm_set(channel, (duty*2048)/25);
        asprintf(&response, "Command success: PWM set duty = %d ", duty);

    } else {
        response = "Command error: Invalid operation type";
    }

    return response;
}

char* handle_command(const char *cmd) 
{
    char *response = NULL;
    char cmd_copy[10];
    char cmd_back_copy[1440];

    strncpy(cmd_copy, cmd, sizeof(cmd_copy) - 1);
    cmd_copy[sizeof(cmd_copy) - 1] = '\0';

    strncpy(cmd_back_copy, cmd, sizeof(cmd_back_copy) - 1);
    cmd_back_copy[sizeof(cmd_back_copy) - 1] = '\0';

    // Tokenize the command by spaces
    char *token = strtok(cmd_copy, " ");
    if (token == NULL) {
        response = "Command error: No command provided";
        return response;
    }

    if (strcmp(token, "gpio") == 0) {
        response = cmd_gpio(cmd_back_copy);
    } else if (strcmp(token, "i2c") == 0) {
        response = cmd_i2c(cmd_back_copy);
    } else if (strcmp(token, "spi") == 0) {
        response = cmd_spi(cmd_back_copy);
    } else if (strcmp(token, "adc") == 0) {
        response = cmd_adc(cmd_back_copy);
    } else if (strcmp(token, "pwm") == 0) {
        response = cmd_pwm(cmd_back_copy);
    } else if (strcmp(token, "uart") == 0) {
        //response = cmd_uart(cmd_back_copy);
    } else {
        response = "Command error: Unknown command";
    }

    return response;
}


