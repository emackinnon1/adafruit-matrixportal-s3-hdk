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


// TODO: make a function to concatenate the url here
// https://stackoverflow.com/questions/2218290/concatenate-char-array-in-c
char * APPLETS[] = {
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=aurora&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=countdown&event=Anniversary&event_time=2025-11-06T07%3A00%3A09.000Z&eventColor=%23FFA500&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=date_progress&show_day=true&show_month=true&show_year=true&show_labels=true&show_values=true&color_year=%230ff&color_month=%230f0&color_day=%23f00&start_hour=0&start_minute=0&end_hour=0&end_minute=0&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=days_to_xmas&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=digital_rain&color=green&char_size=smaller&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=dvdlogo&show_day=true&show_month=true&show_year=true&show_labels=true&show_values=true&color_year=%230ff&color_month=%230f0&color_day=%23f00&start_hour=0&start_minute=0&end_hour=0&end_minute=0&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=eplscores&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=marta&station=Five+Points&arrivals=false&scroll=true&text_color=%23aaaaaa&orientation=false&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=mvv&stop=%7B%22display%22%3A%22Hauptbahnhof%22%2C%22text%22%3A%22Hauptbahnhof%22%2C%22value%22%3A%22de%3A09162%3A6%22%7D&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=nws_forecast&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=planetarium_clock&furthest_planet=8&mode=clock&show_date=true&format=webp&output=image",
  // "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=soccermens&leagueOptions=ger.1&team_sequence=home&displayType=colors&displayTimeColor=%23fff&displaySpeed=1500&is_us_date_format=true&day_range=true&days_back=1&days_forward=1&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=soccermens&leagueOptions=ger.1&team_sequence=home&displayType=colors&displayTimeColor=%23fff&displaySpeed=3000&is_24_hour_format=true&is_us_date_format=true&format=webp&output=image",
  // "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=soccermens&leagueOptions=esp.1&team_sequence=home&displayType=colors&displayTimeColor=%23fff&displaySpeed=1500&is_us_date_format=true&day_range=true&days_back=1&days_forward=1&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=soccermens&leagueOptions=esp.1&team_sequence=home&displayType=colors&displayTimeColor=%23fff&displaySpeed=3000&is_24_hour_format=true&is_us_date_format=true&format=webp&output=image",
  // "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=soccermens&leagueOptions=eng.fa&team_sequence=home&displayType=colors&displayTimeColor=%23fff&displaySpeed=1500&is_us_date_format=true&day_range=true&days_back=1&days_forward=1&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=soccermens&leagueOptions=eng.fa&team_sequence=home&displayType=colors&displayTimeColor=%23fff&displaySpeed=3000&is_24_hour_format=true&is_us_date_format=true&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=soccer_tables&isplayOptions=Rank&LeagueOptions=eng.1&ColorOptions=black&speed=3&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=soccer_tables&isplayOptions=Rank&LeagueOptions=esp.1&ColorOptions=black&speed=3&format=webp&output=image",
  "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=soccer_tables&isplayOptions=Rank&LeagueOptions=ger.1&ColorOptions=black&speed=3&format=webp&output=image",
  // "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=yearprogress&color=%2347a&format=webp&output=image"
  // DISABLE "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=divbyt&format=webp&output=image",
  // DISABLE "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=starfield&?background_color=%23000&star_color=%23FFF&star_count=10&star_speed=0.5&star_tail_length=1.5&use_custom_star_colors=false&format=webp&output=image",
  // "https://teal-sprinkles-cd1d0e.netlify.app/?fileName=sunrisesunset&items_to_display=both&location="%7B"lat"%3A"39.742"%2C"lng"%3A"-104.992"%2C"locality"%3A"Denver%2C+CO"%2C"timezone"%3A"America%2FDenver"%7D"&format=webp&output=image"
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

      // ESP_LOGI(TAG, "Count: %d", i);
      vTaskDelay(pdMS_TO_TICKS(30000));
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
