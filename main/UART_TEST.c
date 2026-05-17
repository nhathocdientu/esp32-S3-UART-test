#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/uart.h"
#include "esp_log.h"

#define UART_PORT      UART_NUM_0
#define BUF_SIZE       1024

static const char *TAG = "UART";

void app_main(void)
{
    uart_config_t uart_config =
    {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT
    };

    // Config UART
    ESP_ERROR_CHECK(
        uart_param_config(UART_PORT, &uart_config)
    );

    // Install driver
    ESP_ERROR_CHECK(
        uart_driver_install(
            UART_PORT,
            BUF_SIZE,
            BUF_SIZE,
            0,
            NULL,
            0
        )
    );

    uint8_t rx_data[BUF_SIZE];

    while (1)
    {
        memset(rx_data, 0, sizeof(rx_data));

        int len = uart_read_bytes(
            UART_PORT,
            rx_data,
            BUF_SIZE - 1,
            pdMS_TO_TICKS(1000)
        );

        if (len > 0)
        {
            rx_data[len] = '\0';

            // Xóa enter
            char *p;

            p = strchr((char *)rx_data, '\r');
            if (p) *p = '\0';

            p = strchr((char *)rx_data, '\n');
            if (p) *p = '\0';

            ESP_LOGI(TAG, "RX: %s", rx_data);

            // Chuỗi phản hồi
            char tx_data[BUF_SIZE];

            snprintf(
                tx_data,
                sizeof(tx_data),
                "Xin chao %.1000s\r\n",
                rx_data
            );

            // Gửi UART
            uart_write_bytes(
                UART_PORT,
                tx_data,
                strlen(tx_data)
            );
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}