#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// #include "max17048.h"
#include "u8g2.h"
#include "hal.h"


void app_main() {
    ESP_ERROR_CHECK(gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_level(LED_PIN, 0));

    init_i2c();

    u8g2_t dd;
    u8g2_Setup_ssd1315_i2c_128x64_noname_f(&dd, U8G2_R0, hal_u8g2_cb, hal_u8g2_delay);
    u8g2_InitDisplay(&dd);
    u8g2_SetPowerSave(&dd, 0);

    while (true) {
        u8g2_ClearBuffer(&dd);
        u8g2_SetFont(&dd, u8g2_font_ncenB08_tr);
        u8g2_DrawStr(&dd, 0, 30, "Hello ESP!");
        u8g2_SendBuffer(&dd);

        gpio_set_level(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_set_level(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
