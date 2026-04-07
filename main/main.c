/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"  // interrupts
#include "hardware/pwm.h"  // pwm 
#include "hardware/sync.h" // wait for interrupt 
#include "hardware/clocks.h" // redefined set_sys_clock_khz() 
#include "game_over.h"
#include "game_start.h"
#define MAX_SEQ 100
#define DEBOUNCE_MS 200000
//Definindo pinos
const int PIN_SOM  = 28;
const int PIN_BUZZER = 2;
const int PIN_BTN_Y  = 3;
const int PIN_BTN_B= 4;
const int PIN_BTN_G  = 5;
const int PIN_BTN_R = 6;
const int Led_R = 7;
const int Led_B = 8;
const int Led_G = 9;
const int Led_Y = 10;

volatile bool btn_flag_Y = false;
volatile bool btn_flag_B = false;
volatile bool btn_flag_G = false;
volatile bool btn_flag_R = false;
volatile int wav_position_game_over = 0;
volatile bool game_over = false;
volatile int wav_position_game_start = 0;
volatile bool game_start = false;
volatile uint32_t last_time_Y = 0;
volatile uint32_t last_time_B = 0;
volatile uint32_t last_time_G = 0;
volatile uint32_t last_time_R = 0;

volatile bool perdeu = false;
/*
btn_flag = 1 ==> Yellow
btn_flag = 2 ==> Blue
btn_flag = 3 ==> Green
btn_flag = 4 ==> Red

*/


//Funções callback
//Função de IRQ
void pwm_interrupt_handler() {
    pwm_clear_irq(pwm_gpio_to_slice_num(PIN_SOM));    
    if (game_start){
        if (wav_position_game_start < (WAV_DATA_GAME_START_LENGTH<<3) - 1) { 
            // set pwm level 
            // allow the pwm value to repeat for 8 cycles this is >>3 
            pwm_set_gpio_level(PIN_SOM, WAV_DATA_GAME_START[wav_position_game_start>>3]);  
            wav_position_game_start++;
        }
        else {
            game_start = false;
            wav_position_game_start = 0;
        }
    } else if (game_over){
        if (wav_position_game_over < (WAV_DATA_GAME_OVER_LENGTH<<3) - 1) { 
            // set pwm level 
            // allow the pwm value to repeat for 8 cycles this is >>3 
            pwm_set_gpio_level(PIN_SOM, WAV_DATA_GAME_OVER[wav_position_game_over>>3]);  
            wav_position_game_over++;
        }
        else {
            game_over = false;
            wav_position_game_over = 0;
        }
    }
}

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { // fall edge
        uint32_t now = time_us_32();

        if (gpio == PIN_BTN_Y && (now - last_time_Y) > DEBOUNCE_MS) {
            last_time_Y = now;
            btn_flag_Y = true;
        }
        else if (gpio == PIN_BTN_B && (now - last_time_B) > DEBOUNCE_MS) {
            last_time_B = now;
            btn_flag_B = true;
        }
        else if (gpio == PIN_BTN_G && (now - last_time_G) > DEBOUNCE_MS) {
            last_time_G = now;
            btn_flag_G = true;
        }
        else if (gpio == PIN_BTN_R && (now - last_time_R) > DEBOUNCE_MS) {
            last_time_R = now;
            btn_flag_R = true;
        }
    }
}



void playtone(int pino, int frequencia, int tempo){

    int T = (1000000/frequencia);
    int ciclos = (tempo) / T;

    for(int i = 0; i < ciclos ; i++){
                gpio_put(pino, 1);
                sleep_us(T/2);
                gpio_put(pino, 0);
                sleep_us(T/2);
            }   
}

int64_t time_callback(alarm_id_t id, void *user_data) {
    perdeu = true;
    return 0;
}


int main() {
    stdio_init_all();

    

    //Inicializando botões
    gpio_init(PIN_BTN_Y);
    gpio_init(PIN_BTN_B);
    gpio_init(PIN_BTN_G);
    gpio_init(PIN_BTN_R);
    gpio_init(Led_R);
    gpio_init(Led_B);
    gpio_init(Led_G);
    gpio_init(Led_Y);
    gpio_init(PIN_BUZZER);
    

    //Definindo direções
    gpio_set_dir(PIN_BUZZER, GPIO_OUT);

    gpio_set_dir(Led_R, GPIO_OUT);

    gpio_set_dir(Led_B, GPIO_OUT);

    gpio_set_dir(Led_G, GPIO_OUT);

    gpio_set_dir(Led_Y, GPIO_OUT);

    gpio_set_dir(PIN_BTN_Y, GPIO_IN);
    gpio_pull_up(PIN_BTN_Y);

    gpio_set_dir(PIN_BTN_B, GPIO_IN);
    gpio_pull_up(PIN_BTN_B);

    gpio_set_dir(PIN_BTN_G, GPIO_IN);
    gpio_pull_up(PIN_BTN_G);

    gpio_set_dir(PIN_BTN_R, GPIO_IN);
    gpio_pull_up(PIN_BTN_R);

    ////Definindo IRQ a partir do botão Y
    gpio_set_irq_enabled_with_callback(
      PIN_BTN_Y,GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    //Definindo IRQ a partir do botão B
    // callback BOTÃO G(nao usar _with_callback na segunda chamada de Interrupção, o callback e o nome dessa função callback já foi registrado anteriormente)
    gpio_set_irq_enabled(PIN_BTN_B,GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

    //Definindo IRQ a partir do botão G
    gpio_set_irq_enabled(PIN_BTN_G,GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

    //Definindo IRQ a partir do botão R
    gpio_set_irq_enabled(PIN_BTN_R,GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

        

    //Inicializando buzzer ----------------------------------------------------------------------------------------
    gpio_init(PIN_BUZZER);
    gpio_set_dir(PIN_BUZZER, GPIO_OUT);

    //Frequência Buzzer
    /*
    int freqY = 1000; //hz
    int freqB = 6000;//hz
    int freqG = 500; //hz
    int freqR = 3000;//hz
    int time = 100000; //micro s
    */

    // iniciando audio pwm
    set_sys_clock_khz(176000, true); 
    gpio_set_function(PIN_SOM, GPIO_FUNC_PWM);
    int audio_pin_slice = pwm_gpio_to_slice_num(PIN_SOM);
    pwm_clear_irq(audio_pin_slice);
    pwm_set_irq_enabled(audio_pin_slice, true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_interrupt_handler); 
    irq_set_enabled(PWM_IRQ_WRAP, true);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 8.0f); 
    pwm_config_set_wrap(&config, 255); 
    pwm_init(audio_pin_slice, &config, true);

    pwm_set_gpio_level(PIN_SOM, 0);


    // variaveis
    srand(time_us_32());
    int nova_seq[MAX_SEQ];
    int m = 0;
    bool flag_show = false;
    bool flag_innit = true;
    bool flag_play = false;
    bool flag_seq =false;
    int p = 0;
    alarm_id_t alarm_button;
    


    while (true) {
        if (flag_innit){
            printf("Iniciou jogo\n");
            game_start = true;
            flag_innit = false;
            flag_seq = true;
            m = 0;
            sleep_ms(2000);
        }
        if (flag_seq) {
            int num = rand() % 4 + 1;
            nova_seq[m] = num;   // Simplesmente adiciona no final
            m += 1;
            flag_seq = false;
            flag_show = true;
        }
        if (flag_show) {
            for (int i = 0; i < m; i ++) {
                int num = nova_seq[i];
                switch (num) {
                    case 1:
                        printf("Amarelo\n");
                        playtone(PIN_BUZZER, 1000, 100000);
                        gpio_put(Led_Y, 1);
                        sleep_ms(500);
                        gpio_put(Led_Y, 0);
                        break;
                    case 2:
                        printf("Azul\n");
                        playtone(PIN_BUZZER, 6000, 100000);
                        gpio_put(Led_B, 1);
                        sleep_ms(500);
                        gpio_put(Led_B, 0);
                        break;
                    case 3:
                        printf("Verde\n");
                        playtone(PIN_BUZZER, 500, 100000);
                        gpio_put(Led_G, 1);
                        sleep_ms(500);
                        gpio_put(Led_G, 0);
                        break;
                    case 4:
                        printf("Vermelho\n");
                        playtone(PIN_BUZZER, 3000, 100000);
                        gpio_put(Led_R, 1);
                        sleep_ms(500);
                        gpio_put(Led_R, 0);
                        break;
                }
            }
            flag_show = false;
            alarm_button = add_alarm_in_ms(5000, time_callback, NULL, false);
            p = 0;
            flag_play = true;
        }
        if (perdeu) {
            printf("Perdeu\n");
            if (alarm_button > 0){
                cancel_alarm(alarm_button);
            }
            game_over = true;
            sleep_ms(2000);
            m = 0;
            perdeu = false;
            flag_innit = true;
        }
        if (flag_play) {
            if(btn_flag_Y){
                printf("Clickou o amarelo\n");
                cancel_alarm(alarm_button);
                if (nova_seq[p] != 1) {
                    perdeu = true;
                    flag_play = false;
                    flag_show = false;
                    flag_innit = false;
                    flag_seq = false;
                }
                playtone(PIN_BUZZER, 1000, 100000);
                btn_flag_Y = false;
                gpio_put(Led_Y, 1);
                sleep_ms(500);
                gpio_put(Led_Y, 0);
                p +=1;
                if (p >= m && perdeu == false) {
                    flag_play = false;
                    flag_seq = true;
                }
                if (p < m && perdeu == false) {
                    alarm_button = add_alarm_in_ms(5000, time_callback, NULL, false);
                }
            }
            if(btn_flag_B){
                printf("Clickou o azul\n");
                cancel_alarm(alarm_button);
                if (nova_seq[p] != 2) {
                    perdeu = true;
                    flag_play = false;
                    flag_show = false;
                    flag_innit = false;
                    flag_seq = false;
                }
                playtone(PIN_BUZZER, 6000, 100000);
                btn_flag_B = false;
                gpio_put(Led_B, 1);
                sleep_ms(500);
                gpio_put(Led_B, 0);
                p +=1;
                if (p >= m && perdeu == false) {
                    flag_play = false;
                    flag_seq = true;
                }
                if (p < m && perdeu == false) {
                    alarm_button = add_alarm_in_ms(5000, time_callback, NULL, false);
                }
            }
            if(btn_flag_G){
                printf("Clickou o verde\n");
                cancel_alarm(alarm_button);
                if (nova_seq[p] != 3) {
                    perdeu = true;
                    flag_play = false;
                    flag_show = false;
                    flag_innit = false;
                    flag_seq = false;
                }
                playtone(PIN_BUZZER, 500, 100000);
                btn_flag_G = false;
                gpio_put(Led_G, 1);
                sleep_ms(500);
                gpio_put(Led_G, 0);
                p +=1;
                if (p >= m && perdeu == false) {
                    flag_play = false;
                    flag_seq = true;
                }
                if (p < m && perdeu == false) {
                    alarm_button = add_alarm_in_ms(5000, time_callback, NULL, false);
                }
            }
            if(btn_flag_R){
                printf("Clickou o vermelho\n");
                cancel_alarm(alarm_button);
                if (nova_seq[p] != 4) {
                    perdeu = true;
                    flag_play = false;
                    flag_show = false;
                    flag_innit = false;
                    flag_seq = false;
                }
                playtone(PIN_BUZZER, 3000, 100000);
                btn_flag_R = false;
                gpio_put(Led_R, 1);
                sleep_ms(500);
                gpio_put(Led_R, 0);
                p +=1;
                if (p >= m && perdeu == false) {
                    flag_play = false;
                    flag_seq = true;
                }
                if (p < m && perdeu == false) {
                    alarm_button = add_alarm_in_ms(5000, time_callback, NULL, false);
                }
            }
        }
        
    }
}


