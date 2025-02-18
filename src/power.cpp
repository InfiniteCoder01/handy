#include <Arduino.h>

#include "input.h"
#include "power.h"
#include "ui/ui.h"

#include <WiFi.h>
#include <hardware/adc.h>
#include <pico/cyw43_arch.h>

static void dormant(uint8_t pin);

namespace power {
void init() {}

void sleep() {
  ui::displayPower(false);
  Serial.flush();

  dormant(3);

  ui::displayPower(true);
  Serial.end();
  Serial.begin(9600);

  updateRTC();
  input.lastActive = millis();
}

// Source:
// https://github.com/raspberrypi/pico-examples/blob/master/adc/read_vsys/power_status.c
bool charging() { return cyw43_arch_gpio_get(CYW43_WL_GPIO_VBUS_PIN); }

// Source:
// https://github.com/raspberrypi/pico-examples/blob/master/adc/read_vsys/power_status.c
float voltage() {
  cyw43_thread_enter();
  // Make sure cyw43 is awake
  cyw43_arch_gpio_get(CYW43_WL_GPIO_VBUS_PIN);

  // setup adc
  adc_gpio_init(PICO_VSYS_PIN);
  adc_select_input(PICO_VSYS_PIN - 26);

  adc_fifo_setup(true, false, 0, false, false);
  adc_run(true);

  // We seem to read low values initially - this seems to fix it
  int ignore = 3;
  while (!adc_fifo_is_empty() || ignore-- > 0) {
    (void)adc_fifo_get_blocking();
  }

  uint32_t vsys = 0;
  const uint8_t SAMPLES = 3;
  for (uint8_t i = 0; i < SAMPLES; i++)
    vsys += adc_fifo_get_blocking();
  vsys /= SAMPLES;

  adc_run(false);
  adc_fifo_drain();

  cyw43_thread_exit();

  // Compute voltage
  const float conversion_factor = 3.3f / (1 << 12);
  float voltage = vsys * 3 * conversion_factor;
  static float filtered = NAN;
  if (isnan(filtered))
    filtered = voltage;
  else
    filtered += (voltage - filtered) * 0.5;
  return filtered;
}
} // namespace power

// ************************************************** SLEEP
#include <WiFi.h>
#include <hardware/pll.h>
#include <hardware/xosc.h>
#include <pico/runtime_init.h>

static void dormant(uint8_t pin) {
  /*
  16.8mA - regular dormant
  2.3mA - cyw43_deinit(&cyw43_state) or cyw43_arch_deinit()
  */

  cyw43_arch_deinit();

  gpio_set_dormant_irq_enabled(
      pin, IO_BANK0_DORMANT_WAKE_INTE0_GPIO0_EDGE_LOW_BITS, true);
  xosc_dormant();
  gpio_acknowledge_irq(pin, IO_BANK0_DORMANT_WAKE_INTE0_GPIO0_EDGE_LOW_BITS);

  cyw43_arch_init();
}
