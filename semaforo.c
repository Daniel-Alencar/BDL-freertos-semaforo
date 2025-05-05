#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pio_matrix.pio.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "lib/led_matrix.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include <stdio.h>
#include "pico/bootrom.h"


// Controle dos periféricos
#define botaoA 5
#define botaoB 6

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

#define BUZZER_FREQUENCY 2000

uint slice_num;
pwm_config config;

PIO pio;
uint sm;

bool cor = true;
ssd1306_t ssd;

#define INITIAL_LED 0
// Controla o estado do ledRGB
uint8_t activatedLed = INITIAL_LED;
// Controla o estado do buzzer
bool buzzerActive = true;
// Controla os modos do led RGB
bool normalModeOn = true;
// Armazena o tempo do último evento (em microssegundos)
static volatile uint32_t last_time = 0;

char *colors[] = {"Verde", "Amarelo", "Vermelho"};



void gpio_irq_handler(uint gpio, uint32_t events) {
    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
  
    // Verifica se passou tempo suficiente desde o último evento
    // 200 ms de debouncing
    if (current_time - last_time > 200000) {
      // Atualiza o tempo do último evento
      last_time = current_time;
  
      if(gpio == botaoA) {
        normalModeOn = !normalModeOn;
        last_time = current_time;
      } else if (gpio == botaoB) {
        reset_usb_boot(0, 0);
      }
    }   
  }


///////////
// TASKS //
///////////

void vBlinkLedTask() {
    while(true) {
        if(normalModeOn) {
            if(activatedLed == 0) {
                gpio_put(11, true);
                gpio_put(12, false);
                gpio_put(13, false);

                buzzerActive = true;
                vTaskDelay(pdMS_TO_TICKS(1000));
                buzzerActive = false;
                vTaskDelay(pdMS_TO_TICKS(1000));

                // Muda para a próxima cor do semáforo
                activatedLed++;
            } else if(activatedLed == 1) {
                gpio_put(11, true);
                gpio_put(12, false);
                gpio_put(13, true);

                buzzerActive = true;
                vTaskDelay(pdMS_TO_TICKS(250));
                buzzerActive = false;
                vTaskDelay(pdMS_TO_TICKS(250));
                buzzerActive = true;
                vTaskDelay(pdMS_TO_TICKS(250));
                buzzerActive = false;
                vTaskDelay(pdMS_TO_TICKS(250));
                buzzerActive = true;
                vTaskDelay(pdMS_TO_TICKS(250));
                buzzerActive = false;
                vTaskDelay(pdMS_TO_TICKS(250));

                // Muda para a próxima cor do semáforo
                activatedLed++;
            } else if(activatedLed == 2) {
                gpio_put(11, false);
                gpio_put(12, false);
                gpio_put(13, true);

                buzzerActive = true;
                vTaskDelay(pdMS_TO_TICKS(500));
                buzzerActive = false;
                vTaskDelay(pdMS_TO_TICKS(1500));

                // Muda para a próxima cor do semáforo
                activatedLed = INITIAL_LED;
            }
        } else {
            gpio_put(11, true);
            gpio_put(12, false);
            gpio_put(13, true);

            buzzerActive = true;
            vTaskDelay(pdMS_TO_TICKS(250));
            buzzerActive = false;
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }
}

void vBuzzerTask() {
    while(true) {
        if(buzzerActive) {
            pwm_set_enabled(slice_num, true);
        } else {
            pwm_set_enabled(slice_num, false);
        }
        vTaskDelay(1);
    }
}

void vDisplayTask() {
    while (true) {
        ssd1306_fill(&ssd, !cor);                        // Limpa o display
        // Desenha o o semáforo
        ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);    // Desenha um retângulo
        ssd1306_line(&ssd, 20, 7, 45, 7, cor);           // Desenha uma linha
        ssd1306_line(&ssd, 20, 7, 20, 57, cor);          // Desenha uma linha
        ssd1306_line(&ssd, 20, 57, 45, 57, cor);         // Desenha uma linha
        ssd1306_line(&ssd, 45, 7, 45, 57, cor);          // Desenha uma linha

        if(normalModeOn) {
            ssd1306_rect(&ssd, 12, 28, 11, 10, cor, activatedLed == 0 ? cor : !cor);
            ssd1306_rect(&ssd, 27, 28, 11, 10, cor, activatedLed == 1 ? cor : !cor);
            ssd1306_rect(&ssd, 42, 28, 11, 10, cor, activatedLed == 2 ? cor : !cor);

            // Deseha a cor correspondente do semáforo
            ssd1306_draw_string(&ssd, colors[activatedLed], 50, 29);
        } else {
            ssd1306_rect(&ssd, 12, 28, 11, 10, cor, !cor);
            ssd1306_rect(&ssd, 27, 28, 11, 10, cor, cor);
            ssd1306_rect(&ssd, 42, 28, 11, 10, cor, !cor);

            // Deseha o Amarelo do semáforo
            ssd1306_draw_string(&ssd, colors[1], 50, 29);
        }
        // Atualiza o display
        ssd1306_send_data(&ssd);
        vTaskDelay(1);
    }
}

void vLedMatrixTask() {
    while(true) {
        if(normalModeOn) {
            set_pixel_color(7, 0, activatedLed == 0 ? 0.1 : 0, 0);
            set_pixel_color(12, activatedLed == 1 ? 0.1 : 0, activatedLed == 1 ? 0.1 : 0, 0);
            set_pixel_color(17, activatedLed == 2 ? 0.1 : 0, 0, 0);
        } else {
            set_pixel_color(7, 0, 0, 0);
            set_pixel_color(12, 1, 1, 0);
            set_pixel_color(17, 0, 0, 0);
        }
        draw_matrix(pio, sm);
        vTaskDelay(1);
    }
}


int main() {
    stdio_init_all();

    // Configuração Led RGB
    gpio_init(11);
    gpio_set_dir(11, GPIO_OUT);
    gpio_init(12);
    gpio_set_dir(12, GPIO_OUT);
    gpio_init(13);
    gpio_set_dir(13, GPIO_OUT);

    // Configuração Botões
    gpio_set_dir(botaoA, GPIO_IN);
    gpio_pull_up(botaoA);
    gpio_init(botaoB);
    gpio_set_dir(botaoB, GPIO_IN);
    gpio_pull_up(botaoB);
    gpio_set_irq_enabled_with_callback(botaoA, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Configuração Buzzer
    gpio_set_function(21, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(21);
    config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.0f);
    pwm_init(slice_num, &config, false);
    
    uint32_t wrap = (clock_get_hz(clk_sys) / 4 / BUZZER_FREQUENCY) - 1;
    pwm_set_wrap(slice_num, wrap);
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(21), wrap / 2);

    // Configuração Display
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Configuração Matriz de leds
    pio = pio0;
    set_sys_clock_khz(128000, false);
    uint offset = pio_add_program(pio, &pio_matrix_program);
    sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, 7);

    xTaskCreate(
        vBlinkLedTask, "Tarefa Led RGB", configMINIMAL_STACK_SIZE, 
        NULL, tskIDLE_PRIORITY, NULL
    );
    xTaskCreate(
        vBuzzerTask, "Tarefa Buzzer", configMINIMAL_STACK_SIZE, 
        NULL, tskIDLE_PRIORITY, NULL
    );
    xTaskCreate(
        vDisplayTask, "Tarefa Display", configMINIMAL_STACK_SIZE, 
        NULL, tskIDLE_PRIORITY + 1, NULL
    );
    xTaskCreate(
        vLedMatrixTask, "Tarefa Matriz de leds", configMINIMAL_STACK_SIZE, 
        NULL, tskIDLE_PRIORITY + 1, NULL
    );

    vTaskStartScheduler();
    panic_unsupported();
}
