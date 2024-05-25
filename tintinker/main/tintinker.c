#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "driver/ledc.h"
#include "hal/gpio_types.h"
#include "portmacro.h"

#define BLINK_LED 8

void app_main(void) {
  char *ourTaskName = pcTaskGetName(NULL);
  ESP_LOGI(ourTaskName, "hello, starting up");

  gpio_reset_pin(BLINK_LED);
  int test = gpio_set_direction(BLINK_LED, GPIO_MODE_OUTPUT);

  ESP_LOGI(ourTaskName, "gpio_set_direction result : %d", test);
  gpio_dump_io_configuration(stdout, BLINK_LED);

  while (1) {
    gpio_set_level(BLINK_LED, 1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    gpio_set_level(BLINK_LED, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// I (293) gpio: GPIO[8]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
// I (293) gpio: GPIO[8]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
