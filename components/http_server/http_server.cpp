#include "http_server.h"
#include "esp_http_server.h"
#include "esp_log.h"

#include <cJSON.h>

static const char *TAG = "http_server";
static httpd_handle_t server = NULL;

/**
 * @brief Handler for updating LED colors.
 */
static esp_err_t set_color_handler(httpd_req_t *req) {
    char content[100];
    int content_len = req->content_len;
    
    if (content_len >= sizeof(content)) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    int ret = httpd_req_recv(req, content, content_len);
    if (ret <= 0) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    content[ret] = '\0';  // Ensure null termination
    ESP_LOGI(TAG, "Received /set_color: %s", content);

    cJSON *json = cJSON_Parse(content);
    if (!json) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    int r = cJSON_GetObjectItem(json, "r")->valueint;
    int g = cJSON_GetObjectItem(json, "g")->valueint;
    int b = cJSON_GetObjectItem(json, "b")->valueint;
    cJSON_Delete(json);

    ESP_LOGI(TAG, "Parsed Color - R: %d, G: %d, B: %d", r, g, b);

    // Production code
    // led_controller_set_color(r, g, b);
    // httpd_resp_sendstr(req, "Color updated");
    // return ESP_OK;

    // Testing code, delete once ready
    // Send back received values in a JSON response
    cJSON *response = cJSON_CreateObject();
    cJSON_AddNumberToObject(response, "r", r);
    cJSON_AddNumberToObject(response, "g", g);
    cJSON_AddNumberToObject(response, "b", b);
    char *response_str = cJSON_Print(response);
    
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, response_str);

    free(response_str);
    cJSON_Delete(response);

    return ESP_OK;
}

/**
 * @brief Handler for updating LED animation.
 */
static esp_err_t set_animation_handler(httpd_req_t *req) {
    char content[100];
    int content_len = req->content_len;

    if (content_len >= sizeof(content)) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    int ret = httpd_req_recv(req, content, content_len);
    if (ret <= 0) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    content[ret] = '\0';  // Ensure null termination
    ESP_LOGI(TAG, "Received /set_animation: %s", content);

    cJSON *json = cJSON_Parse(content);
    if (!json) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    const char *mode_temp = cJSON_GetObjectItem(json, "mode")->valuestring;
    char mode[32]; // Assume mode chars are not longer than 31 chars
    strncpy(mode, mode_temp, sizeof(mode) -1);
    mode[sizeof(mode) - 1] =  '\0';     // Ensure null termnation
    
    int speed = cJSON_GetObjectItem(json, "speed")->valueint;
    cJSON_Delete(json);

    // Prodcution code
    // led_controller_set_animation(mode, speed);
    // httpd_resp_sendstr(req, "Animation updated");
    // return ESP_OK;

    // testing code, delete when ready
    // Send back the received values in a JSON response
    cJSON *response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "mode", mode);
    cJSON_AddNumberToObject(response, "speed", speed);
    char *response_str = cJSON_Print(response);
    
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, response_str);

    free(response_str);
    cJSON_Delete(response);

    return ESP_OK;
}

/**
 * @brief Registers URI handlers.
 */
static void register_endpoints(httpd_handle_t server) {
    httpd_uri_t color_uri = {
        .uri       = "/set_color",
        .method    = HTTP_POST,
        .handler   = set_color_handler,
        .user_ctx  = NULL
    };
    httpd_register_uri_handler(server, &color_uri);

    httpd_uri_t animation_uri = {
        .uri       = "/set_animation",
        .method    = HTTP_POST,
        .handler   = set_animation_handler,
        .user_ctx  = NULL
    };
    httpd_register_uri_handler(server, &animation_uri);
}

/**
 * @brief Starts the HTTP server.
 */
void http_server_init() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI(TAG, "HTTP Server started.");
        register_endpoints(server);
    } else {
        ESP_LOGE(TAG, "Failed to start HTTP Server.");
    }
}