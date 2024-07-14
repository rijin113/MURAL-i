
#include "rc_setup.h"

#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *RC_TAG = "RC TASK";
static const int RX_BUF_SIZE = 1024;

void RCSetup(void) {
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    ESP_LOGI(RC_TAG, "UART initialized.");
}

void parse_ibus_data(uint8_t* data, int len) {
    if (data[0] != 0x20) {
        ESP_LOGE(RC_TAG, "Invalid IBus packet");
        return;
    }

    // TODO: UNDERSTAND HOW THIS PARSING/BITSHIFTING IS DONE (seen from online forums)
    uint16_t channels[6];
    for (int i = 0; i < 6; i++) {
        channels[i] = data[2 + i * 2] | (data[3 + i * 2] << 8);
        ESP_LOGI(RC_TAG, "Channel %d: %d", i + 1, channels[i]);
    }

    uint16_t checksum = data[30] | (data[31] << 8);
    uint16_t computed_checksum = 0xFFFF;
    for (int i = 0; i < 30; i++) {
        computed_checksum -= data[i];
    }

    if (checksum != computed_checksum) {
        ESP_LOGE(RC_TAG, "Checksum mismatch: received 0x%04X, computed 0x%04X", checksum, computed_checksum);
    } else {
        ESP_LOGI(RC_TAG, "Checksum valid: 0x%04X", checksum);
    }
}

void rx_rc_task(void *arg) {
    ESP_LOGE(RC_TAG, "LISTENING TO RECIEVER");
    esp_log_level_set(RC_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    while (1) {
        const int rxBytes = uart_read_bytes(UART_NUM_1, data, RX_BUF_SIZE, 1000 / portTICK_PERIOD_MS);
        if (rxBytes > 0) {
            ESP_LOGI(RC_TAG, "Read %d bytes", rxBytes);
            ESP_LOGI(RC_TAG, "BYTES %d DATA", *data);
            parse_ibus_data(data, rxBytes);
            vTaskDelay(pdMS_TO_TICKS(200));
        }
    }
    free(data);
}