#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <LCD.h>

const char system_password[6] = "6985";
char entered_password[100] = "";
char entered_password_hide[100] = "";
int entered_password_index = 0;
int hide_or_show = 0; // 0: hide, 1: show
char ADC_value[20] = "";
int ADC_value_index = 0;
int mode = 0; // 0: wrong password, 1: correct password
int cooler_duty_cycle = 0;
int heater_duty_cycle = 0;
int is_blinking = 0;
int LDR_duty_cycle = 0;

void SPI_registers(){
    SPCR = (1<<SPIE) | (1<<SPE) | (0<<DORD) | (0<<MSTR) | (0<<CPOL) | (0<<CPHA) | (1<<SPR1) | (1<<SPR0);
    SPSR = (0<<SPI2X);
}

void TIMER_COUNTER_registeres(){
    // set timer counter 1 to be in fast PWM mode, non-inverting mode
    TCCR1A = (1<<COM1A1) | (1<<COM1B1) | (1<<WGM10);
    TCCR1B = (1<<WGM12) | (1<<CS11);

    TCCR2 = (1<<WGM20) | (1<<WGM21) | (1<<COM21) | (1<<CS21); // prescaler = 8

    // set PWM pin as non-inverting
    TCCR1A &= ~(1<<COM1A0);
    TCCR1A &= ~(1<<COM1B0);
    TCCR2 &= ~(1<<COM20);
}


void checking_password(int entered_password_length){
    char correct_password[20] = "Access is granted";
    char wrong_password[20] = "Wrong password";

    LCD_cmd(0x01);         // clear the screen 

    // if password wrong
    if(entered_password_length != strlen(system_password)){
        for(int j = 0; wrong_password[j]; j++){
            LCD_write(wrong_password[j]);
            
        }
        LCD_cmd(0x01);         // clear the screen
        return;
    }else if(entered_password_length == strlen(system_password)){
        for(int i = 0; i < entered_password_length; i++){
            if(entered_password[i] != system_password[i]){
                for(int j = 0; wrong_password[j]; j++){
                    LCD_write(wrong_password[j]);
                }
                LCD_cmd(0x01);         // clear the screen
                return;
            }
        }    
    }

    // if password correct
    for(int i = 0; correct_password[i]; i++){
        LCD_write(correct_password[i]);   
    }
    mode = 1;
     // write ADC_value to LCD
    _delay_ms(100);
    LCD_cmd(0x01);         // clear the screen
    for(int i = 0; i < strlen(ADC_value); i++){
        LCD_write(ADC_value[i]);
    }
    return;
}


void checking_push_button(unsigned char received_data){
        if(received_data == 12){
        hide_or_show ^= 1;
        if(hide_or_show == 1){
            LCD_cmd(0x01);             //clear the screen
            _delay_ms(100);
            // write entered password to LCD
            for(int i = 0; i < strlen(entered_password); i++){
                LCD_write(entered_password[i]);
            }
        }else if(hide_or_show == 0){
            LCD_cmd(0x01);             //clear the screen
            _delay_ms(100);
            // write entered password to LCD
            for(int i = 0; i < strlen(entered_password_hide); i++){
                LCD_write(entered_password_hide[i]);
            }
        }
    }
}


void add_value_of_SPDR_to_entered_password(char received_data){
        // add value of SPDR to entered_password
    if(received_data != 10 && received_data != 11 && received_data != 12){

        entered_password[entered_password_index] = received_data + '0';
        entered_password_hide[entered_password_index] = '*';

        if(hide_or_show == 0){
            LCD_write('*');
        }else if(hide_or_show == 1){
            LCD_write(received_data + '0');
        }
        entered_password_index++;
    }
}

void submit_and_remove_operations(char received_data){
        // check password
    if(received_data == 10){

        LCD_cmd(0x01);         // clear the screen
        _delay_ms(100);
        // write entered password to LCD
        for(int i = 0; i < strlen(entered_password); i++){
            LCD_write(entered_password[i]);
        }
        _delay_ms(100);

        int entered_password_length = strlen(entered_password);
        checking_password(entered_password_length);
        _delay_ms(100);

        for (int i = 0; i < entered_password_length; i++) {
            entered_password[i] = '\0';
            entered_password_hide[i] = '\0';
        }
        entered_password_index = 0;
    }else if(received_data == 11){
        // remove last character from entered_password
        if(entered_password_index > 0){
            entered_password_index--;
            entered_password[entered_password_index] = '\0';
            entered_password_hide[entered_password_index] = '\0';
            LCD_cmd(0x10);         // move cursor left
            LCD_write(' ');        // remove last character from LCD
            LCD_cmd(0x10);         // move cursor left
        }

    }
}

void add_ADC_value(char received_data){
    // convert received_data to string and add it to ADC_value
    sprintf(ADC_value, "%d", received_data);
}

void cooler_and_heater_motors(char temperature){
    if(temperature >= 25 && temperature <= 55){
        cooler_duty_cycle = 50.0 + ((temperature - 25) / 5) * 10;
        heater_duty_cycle = 0.0;
    }else if(temperature >= 3 && temperature <= 20){
        cooler_duty_cycle = 0.0;
        heater_duty_cycle = 100.0 - ((temperature - 0) / 5) * 25;
    }else if (temperature > 55){
        cooler_duty_cycle = 0.0;
        heater_duty_cycle = 0.0;
    }else if(temperature <3){
        cooler_duty_cycle = 0.0;
        heater_duty_cycle = 0.0;
    }    
}

void led_blinking(){
    if (is_blinking==1){
        PORTB |= (1<<PB0);
        _delay_ms(200);
        PORTB &= ~(1<<PB0);
        _delay_ms(200);
    }else if(is_blinking==2){
        PORTB |= (1<<PB1);
        _delay_ms(200);
        PORTB &= ~(1<<PB1);
        _delay_ms(200);
    }else if(is_blinking==0){
        PORTB &= ~(1<<PB0);
        PORTB &= ~(1<<PB1);
    }
}

void LDR_motor(char LDR_value){
    if(LDR_value <= 25){
        LDR_duty_cycle = 100.0;
    }else if(LDR_value <= 50){
        LDR_duty_cycle = 75.0;
    }else if(LDR_value <= 75){
        LDR_duty_cycle = 50.0;
    }else if(LDR_value <= 100){
        LDR_duty_cycle = 25.0;
    }
}




int main() {
    DDRC = (1<<DDC0) | (1<<DDC1) | (1<<DDC2) | (1<<DDC3) | (1<<DDC4) | (1<<DDC5) | (1<<DDC6) | (1<<DDC7);
    DDRB = (0<<DDB7) | (1<<DDB6) | (0<<DDB5) | (0<<DDB4) | (1<<DDB1) | (1<<DDB0);
    DDRD = (1<<DDD0) | (1<<DDD1) | (1<<DDD2) | (1<<DDD4) | (1<<DDD5) | (1<<DDD7);
    DDRA = (0 << DDA0);


    SPI_registers();
    TIMER_COUNTER_registeres();

    init_LCD();
    LCD_cmd(0x0f); // make blinking cursor

    sei();  
    while(1){
        
        led_blinking();
        _delay_ms(1);
    }
}

ISR(SPI_STC_vect) {
    int received_data = SPDR;
    if(received_data >= 100 && received_data < 200){
            if(mode == 1){
                LDR_motor(received_data - 100);
                OCR2 = (LDR_duty_cycle / 100.0) * 255.0;
            }
    }
    // if password is correct, mode = 1 and if password is wrong, mode = 0
    if(received_data >= 200 && mode == 0){
        checking_push_button(received_data - 200);
        add_value_of_SPDR_to_entered_password(received_data - 200);
        submit_and_remove_operations(received_data - 200);
    }else if(received_data < 100){
        add_ADC_value(received_data);
        if(mode == 1){
            cooler_and_heater_motors(received_data);
            OCR1A = (cooler_duty_cycle / 100.0) * 255.0;
            OCR1B = (heater_duty_cycle / 100.0) * 255.0;

            // if tempreture greeter than 50 blink LED connect to PB0 until tempreture change
            // if tempreture less than 3 blink LED connect to PB1 until tempreture change
            if(received_data > 55){
                is_blinking = 1;
            }else if(received_data < 3){
                is_blinking = 2;
            }else{
                is_blinking = 0;
            }

            // write ADC_value to LCD
            _delay_ms(100);
            LCD_cmd(0x01);         // clear the screen
             for(int i = 0; i < strlen(ADC_value); i++){
                LCD_write(ADC_value[i]);
            }
        }
    }
}