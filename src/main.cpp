// GENERATED WITH HELP FROM GPT-5 MINI; WILL REFACTOR WHEN CONVENIENT
#include <vector>
#include <string>
#include <cstring>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_http_server.h"
#include "esp_sleep.h"

#define WIFI_SSID "" // Replace with hostname of hotspot/router/service
#define WIFI_PASS "" // Replace with password of hotspot/router/service

static const char* TAG = "webserver";
static httpd_handle_t server = nullptr;

// Store uploaded image in RAM
static std::vector<uint8_t> uploaded_image;

// Simple HTML page
const char* index_html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Image Upload</title>
</head>
<body>
  <h1>Upload an Image</h1>
  <form method="POST" action="/upload" enctype="multipart/form-data">
    <input type="file" name="image">
    <input type="submit" value="Upload">
  </form>
  <img id="uploaded" src="" style="max-width:300px;">
  <script>
    const form = document.querySelector('form');
    form.addEventListener('submit', e => {
      e.preventDefault();
      const data = new FormData(form);
      fetch('/upload', { method:'POST', body:data }).then(() => {
        document.getElementById('uploaded').src = '/image?' + new Date().getTime();
      });
    });
  </script>
</body>
</html>
)rawliteral";

// Root page
esp_err_t index_get_handler(httpd_req_t* req) {
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, index_html, strlen(index_html));
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

extern "C" void app_main() {
    if (!strlen(WIFI_SSID)) {
      ESP_LOGI(TAG, "ERROR: WiFi credentials not set! Halting...");
      esp_deep_sleep_start();
    } else {
      nvs_flash_init();
      wifi_init();
      server = start_webserver();
    }
}