#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "esp_err.h"

/**
 * @brief Initialize WiFi manager.
 */
void wifi_manager_init();

/**
 * @brief Connect to WiFi network.
 */
void wifi_manager_connect();

/**
 * @brief Disconnect from WiFi network.
 */
void wifi_manager_disconnect();

/**
 * @brief Get connection status.
 * @return true if connected, false otherwise.
 */
bool wifi_manager_is_connected();

#endif // WIFI_MANAGER_H
