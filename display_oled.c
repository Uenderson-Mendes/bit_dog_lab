#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"

// Definição dos pinos SDA e SCL para comunicação I2C
const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

// Definição dos pinos dos botões A, B, buzzer e LED
#define BUTTON_A 5
#define BUTTON_B 6
#define BUZZER_PIN 21
#define LED_PIN 11

// Cores do LED
enum { RED, GREEN, BLUE };
int led_color = RED; // Cor inicial

// Frequências das notas (em Hz)
#define NOTE_C 261

// Função para tocar uma nota
void play_tone(uint frequency, uint duration) {
    uint period = 1e6 / frequency; // Período em microssegundos
    uint pulse_width = period / 2; // Duração do pulso alto

    for (uint i = 0; i < (duration * 1000) / period; i++) {
        gpio_put(BUZZER_PIN, 1); // Liga o buzzer
        sleep_us(pulse_width);    // Espera metade do período
        gpio_put(BUZZER_PIN, 0); // Desliga o buzzer
        sleep_us(pulse_width);    // Espera metade do período
    }
}

// Função para tocar uma melodia
void play_melody() {
    play_tone(NOTE_C, 100); // Adicionando uma nota alta no final
}

// Função para emitir som de click
void play_click_sound() {
    gpio_put(BUZZER_PIN, 1); // Liga o buzzer
    sleep_ms(10);            // Dura por 10 ms para o som de click
    gpio_put(BUZZER_PIN, 0); // Desliga o buzzer
}

// Função para configurar a cor do LED
void set_led_color(int color) {
    switch (color) {
        case RED:
            gpio_put(LED_PIN, 1); // Acende o LED (vermelho)
            break;
        case GREEN:
            gpio_put(LED_PIN, 1); // Acende o LED (verde)
            break;
        case BLUE:
            gpio_put(LED_PIN, 1); // Acende o LED (azul)
            break;
    }
    sleep_ms(50); // Mantém a cor por 50 ms
    gpio_put(LED_PIN, 0); // Desliga o LED
}

// Função para dividir o texto em várias linhas
void split_text_into_lines(char *text, char lines[][20], int max_line_length) {
    int line_index = 0;
    char *word = strtok(text, " ");
    int line_length = 0;

    while (word != NULL) {
        // Se o comprimento da linha mais a nova palavra for maior que o limite
        if (line_length + strlen(word) + 1 > max_line_length) {
            // Avança para a próxima linha
            line_index++;
            line_length = 0;
        }
        
        // Adiciona a palavra à linha atual
        if (line_length > 0) {
            strcat(lines[line_index], " ");
            line_length++;
        }
        strcat(lines[line_index], word);
        line_length += strlen(word);
        
        // Pega a próxima palavra
        word = strtok(NULL, " ");
    }
}

int main() {
    stdio_init_all();   // Inicializa os tipos stdio padrão presentes ligados ao binário

    // Inicialização do I2C
    i2c_init(i2c1, 400000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o OLED SSD1306
    ssd1306_init();

    // Inicialização dos botões e buzzer
    gpio_init(BUTTON_A);
    gpio_init(BUTTON_B);
    gpio_init(BUZZER_PIN); // Inicializa o pino do buzzer
    gpio_init(LED_PIN);     // Inicializa o pino do LED
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT); // Define o pino do buzzer como saída
    gpio_set_dir(LED_PIN, GPIO_OUT);     // Define o pino do LED como saída
    gpio_pull_up(BUTTON_A);  // Configura o botão A com resistor pull-up
    gpio_pull_up(BUTTON_B);  // Configura o botão B com resistor pull-up

    // Preparar área de renderização para o display
    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);

    // Zera o display inteiro
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

    // Mensagens a serem exibidas no display
	char *text[] = {
		"EMBARCA",
		"SHOW",
		"COMPENSA",
		"3MIL",
		"MUITO",
		"BOM",
		"HELLO",
		"WELCOME",
		"AVANCADA",
		"EFICIENTES",
		"SUPERIOR",
		"PARCERIA",
		"FUTURO",
		"INOVACAO",
		"PONTA"
	};

    int current_text = 0; // Índice do texto atual a ser exibido
    int y = 0;
    char lines[3][20]; // Suporte para até 3 linhas de texto por vez

    // Desenha a primeira mensagem no display
    for (int i = 0; i < count_of(text); i++) {
        memset(lines, 0, sizeof(lines)); // Zera as linhas
        split_text_into_lines(text[i], lines, 16); // Divide o texto em linhas
        for (int j = 0; j < 3; j++) {
            if (strlen(lines[j]) > 0) {
                ssd1306_draw_string(ssd, 5, y, lines[j]);
                y += 8; // Ajusta a posição vertical
            }
        }
        render_on_display(ssd, &frame_area);
    }

    // Loop para alternar entre os textos usando os botões A e B
    while (true) {
        if (!gpio_get(BUTTON_A)) {
            // Botão A foi pressionado: avança para o próximo texto
            current_text++;
            if (current_text >= count_of(text)) {
                current_text = 0; // Se ultrapassar, vai para o primeiro texto
            }
            // Zera o display e desenha o próximo texto
            memset(ssd, 0, ssd1306_buffer_length);
            memset(lines, 0, sizeof(lines)); // Zera as linhas
            split_text_into_lines(text[current_text], lines, 16); // Divide o texto
            y = 0; // Zera a posição vertical
            for (int j = 0; j < 3; j++) {
                if (strlen(lines[j]) > 0) {
                    ssd1306_draw_string(ssd, 5, y, lines[j]);
                    y += 8; // Ajusta a posição vertical
                }
            }
            render_on_display(ssd, &frame_area);
            play_click_sound(); // Emite som de click
            play_melody(); // Toca a melodia
            set_led_color(led_color); // Acende o LED na cor atual
            led_color = (led_color + 1) % 3; // Alterna a cor do LED
            sleep_ms(200); // Pausa para evitar múltiplas leituras do botão
        }

        if (!gpio_get(BUTTON_B)) {
            // Botão B foi pressionado: volta para o texto anterior
            current_text--;
            if (current_text < 0) {
                current_text = count_of(text) - 1; // Se for para o negativo, vai para o último texto
            }
            // Zera o display e desenha o texto anterior
            memset(ssd, 0, ssd1306_buffer_length);
            memset(lines, 0, sizeof(lines)); // Zera as linhas
            split_text_into_lines(text[current_text], lines, 16); // Divide o texto
            y = 0; // Zera a posição vertical
            for (int j = 0; j < 3; j++) {
                if (strlen(lines[j]) > 0) {
                    ssd1306_draw_string(ssd, 5, y, lines[j]);
                    y += 8; // Ajusta a posição vertical
                }
            }
            render_on_display(ssd, &frame_area);
            play_click_sound(); // Emite som de click
            play_melody(); // Toca a melodia
            set_led_color(led_color); // Acende o LED na cor atual
            led_color = (led_color + 1) % 3; // Alterna a cor do LED
            sleep_ms(200); // Pausa para evitar múltiplas leituras do botão
        }
    }

    return 0;
}
