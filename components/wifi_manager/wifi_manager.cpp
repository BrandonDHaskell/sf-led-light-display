#include "wifi_manager.h"
#include "wifi_credentials.h"

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"

#include <cstring>

#ifndef WIFI_SSID
#endif

#ifndef WIFI_PASSWORD
#endif

// #define WIFI_SSID      "YourSSID"
// #define WIFI_PASSWORD  "YourPassword"

static const char *TAG = "wifi_manager";
static bool is_connected = false;

/**
 * @brief WiFi event handler.
 */
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "WiFi disconnected. Reconnecting...");
        is_connected = false;
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        is_connected = true;
    }
}

void wifi_manager_init() {
    ESP_LOGI(TAG, "Initializing WiFi...");

    // Initialize NVS (stores SSID & Password on device)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    // Initialize the TCP/IP stack
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Create WiFi station
    esp_netif_create_default_wifi_sta();

    // WiFi configuration
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Register event handler
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

    // Configure WiFi
    wifi_config_t wifi_config = {};
    strcpy((char *)wifi_config.sta.ssid, WIFI_SSID);
    strcpy((char *)wifi_config.sta.password, WIFI_PASSWORD);
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    // Set WiFi mode and apply configuration
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    // Start WiFi
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi initialized.");
}

void wifi_manager_connect() {
    ESP_LOGI(TAG, "Connecting to WiFi...");
    esp_wifi_connect();
}

void wifi_manager_disconnect() {
    ESP_LOGI(TAG, "Disconnecting from WiFi...");
    esp_wifi_disconnect();
    is_connected = false;
}

bool wifi_manager_is_connected() {
    return is_connected;
}