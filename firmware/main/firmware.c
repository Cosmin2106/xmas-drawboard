#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "freertos/FreeRTOS.h"
#include "hal/adc_types.h"
#include "hal_extensions.h"
#include "max17048.h"
#include "u8g2.h"


void app_main() {
    ESP_ERROR_CHECK(gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_level(LED_PIN, 0));

    adc_unit_t unit_ct1, unit_ct2;
    adc_channel_t channel_ct1, channel_ct2;

    ESP_ERROR_CHECK(adc_oneshot_io_to_channel(CT1_PIN, &unit_ct1, &channel_ct1));
    ESP_ERROR_CHECK(adc_oneshot_io_to_channel(CT2_PIN, &unit_ct2, &channel_ct2));
    // ESP32C6 has a single ADC unit, so the above unit variables should always be the same
    ESP_ERROR_CHECK(unit_ct1 == unit_ct2 ? ESP_OK : ESP_ERR_NOT_SUPPORTED);

    adc_oneshot_unit_handle_t adc;
    const adc_oneshot_unit_init_cfg_t adc_config = {
        .unit_id = unit_ct1,
        .ulp_mode = ADC_ULP_MODE_DISABLE
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_config, &adc));

    adc_oneshot_chan_cfg_t channel_config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc, channel_ct1, &channel_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc, channel_ct2, &channel_config));

    init_i2c();

    max17048_handle_t bat_gauge = max17048_create(i2c_bus, MAX17048_I2C_ADDR_DEFAULT);
    ESP_ERROR_CHECK(bat_gauge ? ESP_OK : ESP_FAIL);

    u8g2_t dd;
    u8g2_Setup_ssd1315_i2c_128x64_noname_f(&dd, U8G2_R0, hal_u8g2_cb, hal_u8g2_delay);
    u8g2_InitDisplay(&dd);
    u8g2_SetPowerSave(&dd, 0);
    u8g2_SetContrast(&dd, 255);

    while (true) {
        int raw_ct1, raw_ct2;
        float perc_ct1, perc_ct2;
        adc_oneshot_read(adc, channel_ct1, &raw_ct1);
        adc_oneshot_read(adc, channel_ct2, &raw_ct2);
        perc_ct1 = (float)raw_ct1 * 100 / 3300;
        perc_ct2 = (float)raw_ct2 * 100 / 3300;

        float perc_bat, v_bat;
        max17048_get_cell_percent(bat_gauge, &perc_bat);
        max17048_get_cell_voltage(bat_gauge, &v_bat);

        char buffer1[16], buffer2[16], buffer3[32];
        sprintf(buffer1, "CT1: %d%%", (int)perc_ct1);
        sprintf(buffer2, "CT2: %d%%", (int)perc_ct2);
        sprintf(buffer3, "BAT: %.1f%%, %.2fV", perc_bat, v_bat);

        u8g2_ClearBuffer(&dd);
        u8g2_SetFont(&dd, u8g2_font_ncenB08_tr);
        u8g2_DrawStr(&dd, 0, 20, buffer1);
        u8g2_DrawStr(&dd, 0, 40, buffer2);
        u8g2_DrawStr(&dd, 0, 60, buffer3);
        u8g2_SendBuffer(&dd);
    }
}
