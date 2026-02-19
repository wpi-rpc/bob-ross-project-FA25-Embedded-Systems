#ifndef NETWORK_HPP
#define NETWORK_HPP

#include "esp_log.h"

#define WIFI_SSID "" // Replace with hostname of hotspot/router/service
#define WIFI_PASS "" // Replace with password of hotspot/router/service

static const char* TAG = "webserver";

// Declarations
void wifi_init(); // WiFi connection
void start_mdns_service(); // Resolves IP to mDNS hostname and starts mDNS service
static char* generate_hostname(); // https://components.espressif.com/components/espressif/mdns/versions/1.3.2/examples/query_advertise

#endif