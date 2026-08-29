#include "display.hpp"
#include "hardware.hpp"

#include <Arduino.h>
#include <WiFi.h>
#include <hardware/adc.h>
#include <hardware/powman.h>

const uint VBUS_PIN = 24;
const uint WAKE_PIN = 27;


void initPowerManagement() {
  powman_timer_set_1khz_tick_source_lposc();
  gpio_init(VBUS_PIN);
  gpio_set_dir(VBUS_PIN, GPIO_IN);
  powman_enable_gpio_wakeup(1, WAKE_PIN, false, false);

  adc_init();
  adc_set_temp_sensor_enabled(true);
}

void sleep() {
  // DORMANT_CLOCK_SOURCE_LPOSC
  return;
  displayPower(false);
  // Serial.flush();
  // Serial.end();
  // WiFi.end();

  // cyw43_arch_deinit();
  powman_power_state on_state = powman_get_power_state();
  powman_configure_wakeup_state(POWMAN_POWER_STATE_NONE, on_state); 
  powman_set_power_state(POWMAN_POWER_STATE_NONE);
  // cyw43_arch_init();

  // Serial.begin(115200);
  updateRTC();
  displayPower(true);
}

// Source: https://github.com/raspberrypi/pico-examples/blob/master/adc/read_vsys/power_status/power_status.c
#define CYW43_WL_GPIO_VBUS_PIN 2
#define PICO_VSYS_PIN 29
#define PICO_FIRST_ADC_PIN 26
#define PICO_POWER_SAMPLE_COUNT 3

bool charging() {
    return cyw43_arch_gpio_get(CYW43_WL_GPIO_VBUS_PIN);
}

float voltage() {
    cyw43_thread_enter();
    // Make sure cyw43 is awake
    cyw43_arch_gpio_get(CYW43_WL_GPIO_VBUS_PIN);

    // setup adc
    adc_gpio_init(PICO_VSYS_PIN);
    adc_select_input(PICO_VSYS_PIN - PICO_FIRST_ADC_PIN);
 
    adc_fifo_setup(true, false, 0, false, false);
    adc_run(true);

    // We seem to read low values initially - this seems to fix it
    int ignore_count = PICO_POWER_SAMPLE_COUNT;
    while (!adc_fifo_is_empty() || ignore_count-- > 0) {
        (void)adc_fifo_get_blocking();
    }

    // read vsys
    uint32_t vsys = 0;
    for(int i = 0; i < PICO_POWER_SAMPLE_COUNT; i++) {
        uint16_t val = adc_fifo_get_blocking();
        vsys += val;
    }

    adc_run(false);
    adc_fifo_drain();

    vsys /= PICO_POWER_SAMPLE_COUNT;
    cyw43_thread_exit();

    // Generate voltage
    const float conversion_factor = 3.3f / (1 << 12);
    return vsys * 3 * conversion_factor;
}
