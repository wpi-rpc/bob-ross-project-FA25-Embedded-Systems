#include "esp_wifi.h"
#include "mdns.h"
#include "network.hpp"

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
    ESP_LOGI(TAG, "Connecting to WiFi %s...", WIFI_SSID);
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

static char* generate_hostname() {
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