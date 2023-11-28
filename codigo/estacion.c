#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "dht.h"
#include "lcd.h"

static const dht_model_t DHT_MODEL = DHT11;
const uint LED_PIN = PICO_DEFAULT_LED_PIN;
static const uint DATA_PIN = 15;
const uint MAX_TIMINGS = 85;
const uint mq135 = 26;
const float high_ppm= 350.0;
const uint cooler_pin=19;
float convert_co2_ppm(float rs, float r0);

void control_cooler(float co2_concentracion,float high_ppm) {
if (co2_concentracion>high_ppm) {
        gpio_put(cooler_pin, 1);
        sleep_ms(2000);
      } else {
        gpio_put(cooler_pin, 0);
      }
}
float convert_co2_ppm(float rs, float r0) {
  return r0 * 1000 / (r0 - rs);
}
float co2_concentracion;
float mq135_reading;

static inline float fahrenheit_to_celsius(float fahrenheit)

 {
    // Retornar el valor de la temperatura en Fahrenheit
    (fahrenheit - 32) * 1,8;
}
int main() {
  
  stdio_init_all();

  gpio_init(LED_PIN);

  gpio_init(DATA_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  adc_init();
  adc_gpio_init(mq135);
  adc_select_input(0);

  gpio_set_function(4, GPIO_FUNC_I2C);
  gpio_set_function(5, GPIO_FUNC_I2C);
  i2c_init(i2c0, 100000);
  lcd_init();

  gpio_init(cooler_pin);
  gpio_set_dir(cooler_pin, GPIO_OUT);
  dht_t dht;
  dht_init(&dht, DHT_MODEL, pio0, DATA_PIN, true /* pull_up */);

  while (1) {
       
      dht_start_measurement(&dht);
      float humidity, temperature_c;
      dht_result_t result = dht_finish_measurement_blocking(&dht, &humidity, &temperature_c);
      if (result == DHT_RESULT_OK) {
       
        printf(" Humedad  de X.X%%" "y  Temperatura de XX.XX C");
      } else if (result == DHT_RESULT_TIMEOUT) {
            // Mensaje de error
            puts("El DHT no responde, probablemente este mal el conexionado");
        } else {
            // Error de checksum
            puts("Mal checksum");
            sleep_ms(500);
        }
     
      uint16_t mq135_reading=adc_read();
      float rs=1024.0/ mq135_reading-1.0;
      co2_concentracion= convert_co2_ppm(rs,10.0);
      char co2_str[15];
      sprintf(co2_str, "CO2 = %.1f ppm", co2_concentracion); 
      char ftemp[15];
      sprintf(ftemp,"T=%.1fC",temperature_c);
      char humid[15];
      sprintf(humid,"h=%.1f%%",humidity);
      
      //muestra temp y humedad en el display
      lcd_set_cursor(1,0);
      lcd_string(ftemp);
      lcd_set_cursor(1,8);
      lcd_string(humid);
      lcd_set_cursor(0,0);
      lcd_string(co2_str);
      sleep_ms(2000); 
      lcd_clear();
      
      
      
      
  }
