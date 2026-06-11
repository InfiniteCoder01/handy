#include "display.hpp"
#include "hardware.hpp"

#include <Arduino.h>
#include <WiFi.h>
#include <hardware/adc.h>
#include <pico/cyw43_arch.h>

static void dormant(uint8_t pin);

void sleep() {
  displayPower(false);
  Serial.flush();
  Serial.end();
  WiFi.end();

  dormant(27);

  Serial.begin(115200);
  updateRTC();
  displayPower(true);
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

// ************************************************** SLEEP
#include <hardware/pll.h>
#include <hardware/xosc.h>
#include <pico/runtime_init.h>
#include <pico/stdlib.h>

inline static void rosc_write(io_rw_32 *addr, uint32_t value) {
  hw_clear_bits(&rosc_hw->status, ROSC_STATUS_BADWRITE_BITS);
  assert(!(rosc_hw->status & ROSC_STATUS_BADWRITE_BITS));
  *addr = value;
  assert(!(rosc_hw->status & ROSC_STATUS_BADWRITE_BITS));
}

static void dormant(uint8_t pin) {
  cyw43_arch_deinit();
  const auto freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC) * KHZ;
  // clock_configure(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_ROSC_CLKSRC_PH, 0,
  //                 freq, freq);
  // clock_configure(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF, 0, freq,
  //                 freq);
  // clock_stop(clk_adc);
  // clock_stop(clk_usb);
  clock_configure(clk_rtc, 0, CLOCKS_CLK_RTC_CTRL_AUXSRC_VALUE_ROSC_CLKSRC_PH,
                  freq, 46875);
  // clock_configure(clk_peri, 0, CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS, freq,
  //                 freq);
  // pll_deinit(pll_sys);
  // pll_deinit(pll_usb);
  gpio_set_dormant_irq_enabled(pin, IO_BANK0_DORMANT_WAKE_INTE0_GPIO0_EDGE_LOW_BITS, true);
  
  xosc_dormant();

  gpio_acknowledge_irq(pin, IO_BANK0_DORMANT_WAKE_INTE0_GPIO0_EDGE_LOW_BITS);
  // clocks_init();
  Serial.begin();
  cyw43_arch_init();
}