#ifndef SERVER_HPP
#define SERVER_HPP

#include "esp_http_server.h"
#include "driver/uart.h"

static httpd_handle_t server = nullptr;

const uart_port_t uart_num = UART_NUM_2; // Default GPIO17 (U2_TXD) and GPIO16 (U2_RXD)
extern uart_config_t uart_config;

esp_err_t index_get_handler(httpd_req_t* req);
esp_err_t favicon_get_handler(httpd_req_t* req);
esp_err_t upload_post_handler(httpd_req_t* req);
httpd_handle_t start_webserver(); // Start webserver

#endif