set(SOURCES
        firmware.c
        hal.c
)

idf_component_register(
        SRCS ${SOURCES}
        INCLUDE_DIRS "include"
        REQUIRES esp_adc esp_driver_gpio esp_driver_i2c u8g2 freertos
)
