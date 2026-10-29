#pragma once

#include "i2c_bus.h"
#include "u8g2.h"

#define I2C_FREQ_HZ 400000
#define DD_ADDR 0x3C    // 7-bit address

#define CT1_PIN GPIO_NUM_0
#define CT2_PIN GPIO_NUM_1
#define LED_PIN GPIO_NUM_15
#define SCL_PIN GPIO_NUM_18
#define SDA_PIN GPIO_NUM_19

extern i2c_bus_handle_t i2c_bus;

void init_i2c();
uint8_t hal_u8g2_cb(u8x8_t* u8x8, uint8_t msg, uint8_t count, void* data);
uint8_t hal_u8g2_delay(u8x8_t* u8x8, uint8_t msg, uint8_t value, void* data);
