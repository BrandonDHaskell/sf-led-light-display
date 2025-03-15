#include "wifi_manager.h"
#include "http_server.h"

extern "C" void app_main() {
    wifi_manager_init();
    http_server_init();
}
