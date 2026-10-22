#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/spi.h"
#include "ff.h"      // FatFs: f_mount, f_open, f_write, f_sync
#include "tf_card.h" // pico_fatfs: SD card pins and clock

#define PHOTO_CELL_PIN 26
#define PHOTO_CELL_ADC 0
#define RESISTOR_OHMS 7500.0f
#define LUX_CONSTANT 7.2723f
#define SUPPLY_VOLTS 3.3f
#define TEMP_PIN 27
#define TEMP_ADC 1
// #define PIN_SCK 18
// #define PIN_TX 19 // PICO: to the card's DI
// #define PIN_RX 20 // POCI: from the card's DO
// #define PIN_CS 21

const float ADC_FULL_SCALE_COUNTS = (1 << 12); // 12-bit ADC
const float CONVERSION_FACTOR_V = SUPPLY_VOLTS / ADC_FULL_SCALE_COUNTS;

static float read_volts(unsigned int adc_input)
{
    adc_select_input(adc_input);
    return adc_read() *
           SUPPLY_VOLTS / ADC_FULL_SCALE_COUNTS;
}

static float photocell_ohms(float photo_cell_volts)
{
    float lux_resistance = SUPPLY_VOLTS * RESISTOR_OHMS / photo_cell_volts - RESISTOR_OHMS;
    return lux_resistance;
}

static float photocell_lux(float lux_resistance)
{
    float lux = powf(10.0f, -1.67f * log10f(lux_resistance) + LUX_CONSTANT);
    return lux;
}

static float lm19_celsius(float temp_volts)
{
    float temp_lin = (1.8663f - temp_volts) / 0.01169f;
    // LM19 datasheet eq.(3): full range (uses sqrtf from math.h)
    float temp_full = -1481.96f + sqrtf(2.1962e6f + (1.8639f - temp_volts) / 3.88e-6f);
    float temp_c = temp_lin;
    return temp_c;
}

void sensors_init(void)
{
    adc_init();
    adc_gpio_init(PHOTO_CELL_PIN);
    // LM19 setup
    adc_gpio_init(TEMP_PIN);

}
float sensors_read_lux(void)
{
    float photo_cell_volts = read_volts(PHOTO_CELL_ADC);
    float lux_resistance = photocell_ohms(photo_cell_volts);
    float lux = photocell_lux(lux_resistance);
    return lux;
}
float sensors_read_celsius(void)
{
    float temp_volts = read_volts(TEMP_ADC);
    float temp_c = lm19_celsius(temp_volts);
    return temp_c;
}
