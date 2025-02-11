

## Atividade: Configurando e Usando o Display OLED com Botões, LED e Sons 

Este projeto demonstra como configurar e programar um Display OLED SSD1306 utilizando a Raspberry Pi Pico com a linguagem C. O código permite exibir diferentes mensagens no display e alternar entre elas utilizando os botões A e B. Além disso, há feedback sonoro e visual com um buzzer e um LED RGB.

## Objetivo

Desenvolver um programa que:
- Inicializa e configura o Display OLED via comunicação I2C.
- Exibe mensagens no display e alterna entre elas ao pressionar botões físicos.
- Reproduz sons ao alternar mensagens.
- Controla um LED RGB para indicar mudanças na interface.

## Desenvolvimento

### Passo 1: Configuração do Ambiente

Antes de iniciar a programação, certifique-se de que o ambiente de desenvolvimento está configurado corretamente:

- Conecte a Raspberry Pi Pico ao computador via USB.
- Utilize o VS Code ou outro editor compatível para compilar e carregar o código na Pico.
- Verifique se as bibliotecas necessárias estão instaladas:
  - [ssd1306.h](https://github.com/BitDogLab/BitDogLab-C/blob/main/display_oled/inc/ssd1306.h) - Biblioteca para manipular o display OLED.
  - [ssd1306_font.h](https://github.com/BitDogLab/BitDogLab-C/blob/main/display_oled/inc/ssd1306_font.h) - Biblioteca para renderizar caracteres no display.
  - [ssd1306_i2c.h](https://github.com/BitDogLab/BitDogLab-C/blob/main/display_oled/inc/ssd1306_i2c.h) - Biblioteca para controle do display via I2C.
  - [ssd1306_i2c.c](https://github.com/BitDogLab/BitDogLab-C/blob/main/display_oled/inc/ssd1306_i2c.c) - Implementação das funções do display.
  - Certifique-se de incluir os arquivos acima em uma pasta "inc" dentro do projeto.

### Passo 2: Configuração do Hardware

Os componentes estão conectados da seguinte forma:
- **Display OLED:**
  - SDA: GPIO14
  - SCL: GPIO15
  - Endereço I2C: 0x3C
- **Botões:**
  - Botão A: GPIO5
  - Botão B: GPIO6
- **Buzzer:**
  - GPIO21
- **LED RGB:**
  - GPIO11

### Passo 3: Explicação do Código

#### Inicialização do I2C e Display OLED
O código inicializa a comunicação I2C e configura o display SSD1306:
```c
const uint I2C_SDA = 14;
const uint I2C_SCL = 15;
i2c_init(i2c1, 400000);
gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
gpio_pull_up(I2C_SDA);
gpio_pull_up(I2C_SCL);
ssd1306_init();
```

#### Configuração dos Botões, Buzzer e LED
Os botões são configurados como entradas com pull-up, enquanto o buzzer e o LED são saídas:
```c
gpio_init(BUTTON_A);
gpio_init(BUTTON_B);
gpio_set_dir(BUTTON_A, GPIO_IN);
gpio_set_dir(BUTTON_B, GPIO_IN);
gpio_pull_up(BUTTON_A);
gpio_pull_up(BUTTON_B);
gpio_init(BUZZER_PIN);
gpio_set_dir(BUZZER_PIN, GPIO_OUT);
gpio_init(LED_PIN);
gpio_set_dir(LED_PIN, GPIO_OUT);
```

#### Exibição de Mensagens no Display
O código exibe mensagens predefinidas e alterna entre elas ao pressionar os botões:
```c
char *text[] = {"EMBARCA", "SHOW", "COMPENSA", "3MIL"};
int current_text = 0;
```

#### Alternância de Mensagens com Botões
Ao pressionar o botão A, o código avança para a próxima mensagem. O botão B retorna à mensagem anterior:
```c
if (!gpio_get(BUTTON_A)) {
    current_text = (current_text + 1) % count_of(text);
    play_click_sound();
}
if (!gpio_get(BUTTON_B)) {
    current_text = (current_text - 1 + count_of(text)) % count_of(text);
    play_click_sound();
}
```

#### Reprodução de Sons
Um som curto é emitido ao alternar mensagens:
```c
void play_click_sound() {
    gpio_put(BUZZER_PIN, 1);
    sleep_ms(10);
    gpio_put(BUZZER_PIN, 0);
}
```

## DEMONSTRAÇÃO
- [Video📽️](https://drive.google.com/file/d/1Wgj5784C5Xs-128YfSatKYl4GhBLDUYN/view?usp=drivesdk)



### Conclusão
Este projeto permite explorar a integração de um display OLED com botões, sons e LED na Raspberry Pi Pico. Você pode modificar as mensagens exibidas ou expandir o código para incluir mais interações. 🚀

