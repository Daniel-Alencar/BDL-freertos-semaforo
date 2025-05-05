#include "hardware/pio.h"

typedef struct {
  double r;
  double g;
  double b;
} Pixel;

// Desenho do contorno do semáforo
Pixel semaforo_model[25] = { 
  {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
  {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
  {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
  {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
  {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}
};
  
// Rotina para definição da intensidade de cores do led
uint32_t matrix_rgb(double r, double g, double b) {
  unsigned char R, G, B;
  R = r * 255;
  G = g * 255;
  B = b * 255;
  return (G << 24) | (R << 16) | (B << 8);
}
  
// Rotina para acionar a matrix de leds
void draw_matrix(PIO pio, uint sm) {
  uint32_t valor_led;

  for (int16_t i = 0; i < 25; i++) {
    valor_led = matrix_rgb(
      semaforo_model[24-i].r,  // vermelho
      semaforo_model[24-i].g,  // verde
      semaforo_model[24-i].b   // azul
    );
    pio_sm_put_blocking(pio, sm, valor_led);
  }
}
  
// Modifica a cor de pixels individuais
void set_pixel_color(int led_index, double r, double g, double b) {
  if (led_index >= 0 && led_index < 25) {
    semaforo_model[led_index].r = r;
    semaforo_model[led_index].g = g;
    semaforo_model[led_index].b = b;
  }
}