#include "nvs_flash.h"
#include "esp_sleep.h"
#include "network.hpp"
#include "server.hpp"

const int uart_buffer_size = 1024*2; // https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/uart.html

extern "C" void app_main() {
    if (!strlen(WIFI_SSID)) {
        ESP_LOGE(TAG, "WiFi credentials not set! Halting...");
        esp_deep_sleep_start();
    } else {
        nvs_flash_init();
        wifi_init();
        // vTaskDelay(pdMS_TO_TICKS(1500));
        start_mdns_service();
        uart_driver_install(UART_NUM_2, uart_buffer_size, uart_buffer_size, 0, nullptr, 0);
        uart_param_config(uart_num, &uart_config);
        uart_set_pin(uart_num, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        server = start_webserver();
    }
}