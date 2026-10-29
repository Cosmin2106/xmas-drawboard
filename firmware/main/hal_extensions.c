#include "hal_extensions.h"

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


static i2c_config_t i2c_config = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = SDA_PIN,
    .scl_io_num = SCL_PIN,
    .sda_pullup_en = false,
    .scl_pullup_en = false,
    .master.clk_speed = I2C_FREQ_HZ
};
i2c_bus_handle_t i2c_bus = nullptr;

void init_i2c() {
    i2c_bus = i2c_bus_create(I2C_NUM_0, &i2c_config);
    ESP_ERROR_CHECK(i2c_bus ? ESP_OK : ESP_FAIL);
}

uint8_t hal_u8g2_cb(u8x8_t* u8x8, uint8_t msg, uint8_t count, void* data) {
    (void)u8x8;
    static i2c_bus_device_handle_t i2c_dev;
    static uint8_t buffer[1040];
    static size_t length;

    switch (msg) {
    case U8X8_MSG_BYTE_INIT:
        i2c_dev = i2c_bus_device_create(i2c_bus, DD_ADDR, I2C_FREQ_HZ);
        ESP_ERROR_CHECK(i2c_dev ? ESP_OK : ESP_FAIL);
        break;
    case U8X8_MSG_BYTE_SET_DC:
        break;
    case U8X8_MSG_BYTE_SEND:
        if (length + count > sizeof(buffer)) {
            return 0;
        }
        if (count > 0) {
            memcpy(buffer + length, data, count);
            length += count;
        }
        break;
    case U8X8_MSG_BYTE_START_TRANSFER:
        length = 0;
        break;
    case U8X8_MSG_BYTE_END_TRANSFER:
        if (length == 0) {
            return 0;
        }
        return i2c_bus_write_bytes(i2c_dev, NULL_I2C_MEM_ADDR, length, buffer) == ESP_OK;
    default:
        return 0;
    }
    return 1;
}

uint8_t hal_u8g2_delay(u8x8_t* u8x8, uint8_t msg, uint8_t value, void* data) {
    (void)u8x8;
    (void)data;

    switch (msg) {
    case U8X8_MSG_DELAY_MILLI:
        vTaskDelay(pdMS_TO_TICKS(value));
        break;
    case U8X8_MSG_DELAY_10MICRO:
        esp_rom_delay_us((uint32_t)value * 10);
        break;
    case U8X8_MSG_DELAY_100NANO:
        esp_rom_delay_us(((uint32_t)value + 9) / 10);
        break;
    case U8X8_MSG_DELAY_NANO:
        break;
    default:
        return 0;
    }
    return 1;
}
