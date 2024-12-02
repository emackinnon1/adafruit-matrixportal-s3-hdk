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

char * APPLETS[] = {
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=aurora&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=date_progress&show_day=true&show_month=true&show_year=true&show_labels=true&show_values=true&color_year=%230ff&color_month=%230f0&color_day=%23f00&start_hour=0&start_minute=0&end_hour=0&end_minute=0&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=days_to_xmas&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=digital_rain&color=green&char_size=smaller&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=dvdlogo&show_day=true&show_month=true&show_year=true&show_labels=true&show_values=true&color_year=%230ff&color_month=%230f0&color_day=%23f00&start_hour=0&start_minute=0&end_hour=0&end_minute=0&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=divbyt&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=eplscores&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=marta&station=Five+Points&arrivals=false&scroll=true&text_color=%23aaaaaa&orientation=false&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=mvv&stop=%7B%22display%22%3A%22Hauptbahnhof%22%2C%22text%22%3A%22Hauptbahnhof%22%2C%22value%22%3A%22de%3A09162%3A6%22%7D&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=nws_forecast&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=planetarium_clock&furthest_planet=8&mode=clock&show_date=true&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=starfield&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=yearprogress&&color=%2347a&format=webp&output=image"
};

char* format_output = "&format=webp&output=image";

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

  for (int i=0; i<sizeof(APPLETS)/sizeof(APPLETS[0]); i++){
      printf("%s\n",APPLETS[i]);
  }

  while (true) {
    for (int i=0; i<sizeof(APPLETS)/sizeof(APPLETS[0]); i++) {
      uint8_t* webp;
      size_t len;
      if (remote_get(APPLETS[i], &webp, &len)) {
        ESP_LOGE(TAG, "Failed to get webp");
      } else {
        ESP_LOGI(TAG, "Updated webp (%d bytes)", len);
        gfx_update(webp, len);
        free(webp);
      }

      vTaskDelay(pdMS_TO_TICKS(10000));
      ESP_LOGI(TAG, "Count: %d", i);
    }
  }
  // for (;;) {
  //   uint8_t* webp;
  //   size_t len;
  //   if (remote_get(TIDBYT_REMOTE_URL, &webp, &len)) {
  //     ESP_LOGE(TAG, "Failed to get webp");
  //   } else {
  //     ESP_LOGI(TAG, "Updated webp (%d bytes)", len);
  //     gfx_update(webp, len);
  //     free(webp);
  //   }

  //   vTaskDelay(pdMS_TO_TICKS(10000));
  // }
}
