#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include "esp_log.h"
#include "edge_path_planning.hpp"
#include "server.hpp"

// Store uploaded image in RAM
static char image_buffer[127*127];

struct command {
    unsigned short x,y;
    uint8_t z;
    uint8_t colour;
};

uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh = 122,
};

// Root page
esp_err_t index_get_handler(httpd_req_t* req) {
    extern const unsigned char index_html_start[] asm("_binary_index_html_start");
    extern const unsigned char index_html_end[] asm("_binary_index_html_end");

    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, (const char*)index_html_start, index_html_end - index_html_start);
}

// Favicon
esp_err_t favicon_get_handler(httpd_req_t* req) { // https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/protocols/esp_http_server.html#file-serving
    extern const unsigned char favicon_ico_start[] asm("_binary_favicon_ico_start");
    extern const unsigned char favicon_ico_end[] asm("_binary_favicon_ico_end");

    httpd_resp_set_type(req, "image/x-icon");
    return httpd_resp_send(req, (const char*)favicon_ico_start, favicon_ico_end - favicon_ico_start);
}

// Upload handler
esp_err_t upload_post_handler(httpd_req_t* req) {
    char width_buffer[2];
    char height_buffer[2];
    char width_offset_buffer[2];
    char height_offset_buffer[2];

    // Read all bytes from POST
    httpd_req_recv(req, width_buffer, 2);
    httpd_req_recv(req, height_buffer, 2);
    httpd_req_recv(req, width_offset_buffer, 2);
    httpd_req_recv(req, height_offset_buffer, 2);
    unsigned short width = *(unsigned short*)width_buffer; // Width "N"
    unsigned short height = *(unsigned short*)height_buffer; // Height "M"
    unsigned short width_offset = *(unsigned short*)width_offset_buffer;
    unsigned short height_offset = *(unsigned short*)height_offset_buffer;

    httpd_req_recv(req, image_buffer, width*height);
    cv::Mat viewMN(height,width,CV_8UC1,image_buffer); // MxN-dimensional view of buffer
    cv::Canny(viewMN,viewMN,100,250);
    Path edge_path = edge_path_coordinates(viewMN, cv::Point(width_offset,height_offset)); // Process the image in-place
    command uart_out;
    uart_out.colour = 0;
    char uart_in = 0x15;
    for (int i=0; i<edge_path.size(); i++) {
        for (int j=0; j<edge_path[i].size(); j++) {
            uart_out.x = edge_path[i][j].x;
            uart_out.y = edge_path[i][j].y;
            uart_out.z = 2;
            if (!j) {
                uart_out.z = 1;
                if (j==edge_path[i].size()-1) {
                    uart_write_bytes(uart_num, (const char*)&uart_out, sizeof(uart_out));
                    ESP_LOGI("uart_out", "(%d,%d,%d,%d)", uart_out.x, uart_out.y, uart_out.z, uart_out.colour);
                    uart_read_bytes(uart_num, &uart_in, 1, 100/portTICK_PERIOD_MS);
                    if (uart_in!=0x06) {
                        ESP_LOGE("uart_in", "Failure to acknowledge");
                    } else {
                        ESP_LOGI("uart_in", "Acknowledged");
                    } uart_in = 0x15;
                }
            }
            if (j==edge_path[i].size()-1) {
                uart_out.z = 0;
            }
            uart_write_bytes(uart_num, (const char*)&uart_out, sizeof(uart_out));
            ESP_LOGI("uart_out", "(%d,%d,%d,%d)", uart_out.x, uart_out.y, uart_out.z, uart_out.colour);
            uart_read_bytes(uart_num, &uart_in, 1, 100/portTICK_PERIOD_MS);
            if (uart_in!=0x06) {
                ESP_LOGE("uart_in", "Failure to acknowledge");
            } else {
                ESP_LOGI("uart_in", "Acknowledged");
            } uart_in = 0x15;
        }
    }

    return httpd_resp_send(req, "OK", 2);
}

// Start webserver
httpd_handle_t start_webserver() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 16384; // Increase stack size to prevent overflow with OpenCV processing
    config.recv_wait_timeout = 10;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t index_uri = { "/", HTTP_GET, index_get_handler, nullptr };
        httpd_register_uri_handler(server, &index_uri);
        
        httpd_uri_t upload_uri = { "/upload", HTTP_POST, upload_post_handler, nullptr };
        httpd_register_uri_handler(server, &upload_uri);

        httpd_uri_t favicon_uri = { "/favicon.ico", HTTP_GET, favicon_get_handler, nullptr };
        httpd_register_uri_handler(server, &favicon_uri);
    }
    return server;
}