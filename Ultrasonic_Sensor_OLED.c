#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

#define TRIG_PIN 16
#define ECHO_PIN 17
#define I2C_PORT i2c1
#define SDA_PIN 14
#define SCL_PIN 15

float get_distance_cm();

int main()
{
    stdio_init_all();

    gpio_init(TRIG_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);
    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);

    //Initialize I2C stuff
    i2c_init(I2C_PORT, 400000);

    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);

    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    ssd1306_t disp;

    disp.external_vcc = false;

    ssd1306_init(&disp, 128, 64, 0x3C, I2C_PORT);

    while (true) {
        float distance = get_distance_cm();

        printf("Distance: %.2f cm\n", distance);

        ssd1306_clear(&disp);
        char buffer[32];
        sprintf(buffer, "%.2f cm", distance);
        ssd1306_draw_string(&disp, 0, 0, 2, "Distance:");
        ssd1306_draw_string(&disp, 0, 20, 2, buffer);
        ssd1306_show(&disp);

        sleep_ms(500);
    }
}

float get_distance_cm(){
    gpio_put(TRIG_PIN, 0);
    sleep_us(2);

    gpio_put(TRIG_PIN, 1);
    sleep_us(10);
    gpio_put(TRIG_PIN, 0);

    while (!gpio_get(ECHO_PIN));

    absolute_time_t start_time = get_absolute_time();
    while (gpio_get(ECHO_PIN));

    absolute_time_t end_time = get_absolute_time();

    int pulse_width = absolute_time_diff_us(start_time, end_time);

    return pulse_width * .0343f / 2.0f;

}

