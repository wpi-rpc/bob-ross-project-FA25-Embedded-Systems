// GENERATED WITH HELP FROM GPT-5 MINI; WILL REFACTOR WHEN CONVENIENT
#include <vector>
#include <string>
#include <cstring>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_http_server.h"
#include "esp_sleep.h"
#include "mdns.h"
#include "edge_path_planning.hpp"

#define WIFI_SSID "" // Replace with hostname of hotspot/router/service
#define WIFI_PASS "" // Replace with password of hotspot/router/service

static const char* TAG = "webserver";
static httpd_handle_t server = nullptr;

// Store uploaded image in RAM
static std::vector<uint8_t> uploaded_image;
static uint8_t buffer[255*255];
cv::Mat viewN(255,255,CV_8UC1,buffer); // NxN-dimensional view of buffer

// Declarations
static char *generate_hostname(void); // https://components.espressif.com/components/espressif/mdns/versions/1.3.2/examples/query_advertise

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
    uploaded_image.clear();

    char buf[1024];
    int remaining = req->content_len;

    // Read all bytes from POST
    std::vector<uint8_t> raw_data;
    while (remaining > 0) {
        int to_read = std::min(remaining, (int)sizeof(buf));
        int ret = httpd_req_recv(req, buf, to_read);
        if (ret <= 0) break;
        raw_data.insert(raw_data.end(), buf, buf + ret);
        remaining -= ret;
    }

    // Find the start of the file data
    const std::string data(raw_data.begin(), raw_data.end());
    size_t pos = data.find("\r\n\r\n"); // end of headers
    if (pos != std::string::npos) {
        pos += 4; // skip the "\r\n\r\n"
        size_t end = data.rfind("--"); // rough end of multipart
        if (end != std::string::npos && end > pos) {
            uploaded_image.assign(raw_data.begin() + pos, raw_data.begin() + end - 2); // strip final \r\n
        } else {
            uploaded_image.assign(raw_data.begin() + pos, raw_data.end());
        }
    }

    ESP_LOGI(TAG, "LOG: Uploaded %d bytes", uploaded_image.size());
    return httpd_resp_send(req, "OK", 2);
}

// Serve image in chunks
esp_err_t image_get_handler(httpd_req_t* req) {
    if (uploaded_image.empty()) {
        return httpd_resp_send_404(req);
    }

    httpd_resp_set_type(req, "image/jpeg");
    size_t chunk_size = 1024;
    size_t sent = 0;
    while (sent < uploaded_image.size()) {
        size_t to_send = std::min(chunk_size, uploaded_image.size() - sent);
        esp_err_t ret = httpd_resp_send_chunk(req,
            reinterpret_cast<const char*>(uploaded_image.data() + sent), to_send);
        if (ret != ESP_OK) break;
        sent += to_send;
    }
    return httpd_resp_send_chunk(req, nullptr, 0); // end of chunks
}

// Start webserver
httpd_handle_t start_webserver() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 8192;
    config.recv_wait_timeout = 10;

    httpd_handle_t server = nullptr;
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t index_uri = { "/", HTTP_GET, index_get_handler, nullptr };
        httpd_register_uri_handler(server, &index_uri);

        httpd_uri_t upload_uri = { "/upload", HTTP_POST, upload_post_handler, nullptr };
        httpd_register_uri_handler(server, &upload_uri);

        httpd_uri_t image_uri = { "/image", HTTP_GET, image_get_handler, nullptr };
        httpd_register_uri_handler(server, &image_uri);

        httpd_uri_t favicon_uri = { "/favicon.ico", HTTP_GET, favicon_get_handler, nullptr };
        httpd_register_uri_handler(server, &favicon_uri);
    }
    return server;
}

// WiFi connection
void wifi_init() {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);

    wifi_config_t wifi_config = {};
    strcpy((char*)wifi_config.sta.ssid, WIFI_SSID);
    strcpy((char*)wifi_config.sta.password, WIFI_PASS);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);

    esp_wifi_start();
    esp_wifi_connect();
    ESP_LOGI(TAG, "LOG: Connecting to WiFi %s...", WIFI_SSID);
}

void start_mdns_service() {
    // Initialize mDNS
    mdns_init();
    // Set hostname
    char* hostname = generate_hostname();
    mdns_hostname_set(hostname); // Sets the hostname to the generated hostname
    // Set default instance name
    mdns_instance_name_set("ESP32 Web Server");
    mdns_service_add(nullptr, "_http", "_tcp", 80, nullptr, 0);
}

extern "C" void app_main() {
    if (!strlen(WIFI_SSID)) {
        ESP_LOGI(TAG, "ERROR: WiFi credentials not set! Halting...");
        esp_deep_sleep_start();
    } else {
        nvs_flash_init();
        wifi_init();
        // vTaskDelay(pdMS_TO_TICKS(1500));
        start_mdns_service();
        server = start_webserver();
    }
}

static char* generate_hostname(void) {
    #ifndef CONFIG_MDNS_ADD_MAC_TO_HOSTNAME
        return strdup(CONFIG_MDNS_HOSTNAME);
    #else
        uint8_t mac[6];
        char* hostname;
        esp_read_mac(mac, ESP_MAC_WIFI_STA);
        if (-1 == asprintf(&hostname, "%s-%02X%02X%02X", CONFIG_MDNS_HOSTNAME, mac[3], mac[4], mac[5])) {
            abort();
        }
        return hostname;
    #endif
}