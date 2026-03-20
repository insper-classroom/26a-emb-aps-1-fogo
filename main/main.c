/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

//Definindo pinos
const int PIN_SOM  = 28;
const int PIN_BUZZER = 2;
const int PIN_BTN_Y  = 3;
const int PIN_BTN_B= 4;
const int PIN_BTN_G  = 5;
const int PIN_BTN_R = 6;

volatile int btn_flag_Y;
volatile int btn_flag_B;
volatile int btn_flag_G;
volatile int btn_flag_R;
/*
btn_flag = 1 ==> Yellow
btn_flag = 2 ==> Blue
btn_flag = 3 ==> Green
btn_flag = 4 ==> Red

*/


//Funções callback
//Função de IRQ
void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { // fall edge
        if(gpio==PIN_BTN_Y){
            btn_flag_Y = 1;
        }
        else if(gpio == PIN_BTN_B){
            btn_flag_B = 2;
        }
        else if(gpio == PIN_BTN_G){
            btn_flag_G = 3;
        }
        else if(gpio == PIN_BTN_R){
            btn_flag_R = 4;
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


int main() {
    stdio_init_all();

    

    //Inicializando botões
    gpio_init(PIN_BTN_Y);
    gpio_init(PIN_BTN_B);
    gpio_init(PIN_BTN_G);
    gpio_init(PIN_BTN_R);
    

    //Definindo direções
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
    int freqG = 1000; //hz
    int freqR = 6000;//hz
    int time = 100000; //micro s
    */
    


    while (true) {
        if(btn_flag_Y==1){
            printf("Clickou o amarelo\n");
            //playtone(PIN_BUZZER, freqY, time);
            btn_flag_Y = 0;
            sleep_ms(200);
        }
        if(btn_flag_B==1){
            printf("Clickou o a\n");
            //playtone(PIN_BUZZER, freqB, time);
            btn_flag_B = 0;
            sleep_ms(200);
        }
        if(btn_flag_G==1){
            printf("Clickou o amarelo\n");
            //playtone(PIN_BUZZER, freqG, time);
            btn_flag_G = 0;
            sleep_ms(200);
        }
        if(btn_flag_R==1){
            printf("Clickou o amarelo\n");
            //playtone(PIN_BUZZER, freqR, time);
            btn_flag_R = 0;
            sleep_ms(200);
        }
    }
}


