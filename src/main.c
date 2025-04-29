#include <assets.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <webp/demux.h>

#include "display.h"
#include "flash.h"
#include "gfx.h"
#include "remote.h"
#include "sdkconfig.h"
#include "wifi.h"
#include "secrets.h"

static const char* TAG = "main";

const char* FORMAT_SUFFIX = "&format=webp&output=image";

// URL concatenation function
char* concat_url(const char* base, const char* path) {
    size_t len = strlen(base) + strlen(path) + 1; // +1 for null terminator
    char* result = malloc(len);
    if (result) {
        strcpy(result, base);
        strcat(result, path);
    }
    return result;
}

char * APPLETS[] = {};

// Function to build a full URL with base, path and format suffix
char* build_url(const char* path) {
  size_t len = strlen(BASE_URL) + strlen(path) + strlen(FORMAT_SUFFIX) + 1;
  char* result = malloc(len);
  if (result) {
      strcpy(result, BASE_URL);
      strcat(result, path);
      strcat(result, FORMAT_SUFFIX);
  }
  return result;
}

void app_main(void) {
  ESP_LOGI(TAG, "Hello world!");

  // Setup the device flash storage.
  if (flash_initialize()) {
    ESP_LOGE(TAG, "failed to initialize flash");
    return;
  }
  esp_register_shutdown_handler(&flash_shutdown);

  // Setup the display.
  if (gfx_initialize(ASSET_NOAPPS_WEBP, ASSET_NOAPPS_WEBP_LEN)) {
    ESP_LOGE(TAG, "failed to initialize gfx");
    return;
  }
  esp_register_shutdown_handler(&display_shutdown);

  // Setup WiFi.
  if (wifi_initialize(TIDBYT_WIFI_SSID, TIDBYT_WIFI_PASSWORD)) {
    ESP_LOGE(TAG, "failed to initialize WiFi");
    return;
  }
  esp_register_shutdown_handler(&wifi_shutdown);

  uint8_t mac[6];
  if (!wifi_get_mac(mac)) {
    ESP_LOGI(TAG, "WiFi MAC: %02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1],
             mac[2], mac[3], mac[4], mac[5]);
  }

  while (true) {
    for (int i = 0; i < sizeof(APPLET_PARAMS)/sizeof(APPLET_PARAMS[0]); i++) {
        char* full_url = build_url(APPLET_PARAMS[i]);
        if (full_url) {
            ESP_LOGI(TAG, "Fetching: %s", full_url);
            
            uint8_t* webp;
            size_t len;
            if (remote_get(full_url, &webp, &len)) {
                ESP_LOGE(TAG, "Failed to get webp");
            } else {
                ESP_LOGI(TAG, "Updated webp (%d bytes)", len);
                gfx_update(webp, len);
                free(webp);
            }
            
            free(full_url);
            vTaskDelay(pdMS_TO_TICKS(12000));
        } else {
            ESP_LOGE(TAG, "Failed to allocate memory for URL");
        }
      }
    }
}
