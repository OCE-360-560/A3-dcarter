/*
 * A4_SD_Logging.c
 * Environmental logger: reads light
 * and temperature once per sample
 * period, prints each reading with the
 * corresponding time and voltage, and
 * appends it to multi_source_cali_ref.csv.
 *
 * Author: David Carter
 * Date: October 18th, 2026
 * License: MIT (see LICENSE)
 */

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

#include "sensors.h"   // photocell and LM19 temperature sensor
#include "sd_logger.h" // SD card logging

#define SAMPLE_PERIOD_MS 1000

int main()
{
    stdio_init_all();

    sensors_init(); // initialize the ADC and both sensor pins

    sd_logger_init(); // initialize the SD card and SPI pins

    sleep_ms(SAMPLE_PERIOD_MS);

    if (!sd_logger_open("multi_source_cali_ref.csv", "time_s, lux, temp_C\r\n"))
    {
        printf("# SD open failed\r\n");
        halt();
    }
        
    sleep_ms(SAMPLE_PERIOD_MS);
    while (true)
    {
        double time_s = to_ms_since_boot(get_absolute_time()) / 1000.0; // convert millisec to sec

        float lux = sensors_read_lux();        // read the photocell once, returns illuminance in lux
        float temp_c = sensors_read_celsius(); // read the LM19 once, returns temperature in

         if (!sd_logger_write(time_s, lux, temp_c))
        printf("# SD write failed\r\n");
            sleep_ms(SAMPLE_PERIOD_MS);

    }

}
