/*
 * File:   Principal.c
 * Author: schwe
 *
 * Created on 3 de agosto de 2023, 12:34 AM
 */

// CONFIG1
#pragma config FOSC = EXTRC_NOCLKOUT// Oscillator Selection bits (RCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

//Librerías
#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#include "I2C.h"
#include "LCD.h"

//Definición de variables
#define _XTAL_FREQ 8000000
int POT1;                                                   //Variables para recibir
char cPOT1[4];                                              //Variables para mandar a LCD
int SEG;
char cSEG[3];
int MIN;
char cMIN[3];
int HOR;
char cHOR[3];
int DAY;
char cDAY[3];
int MON;
char cMON[3];
int YEAR;
char cYEAR[3];

//Prototipos
void setup(void);
void main(void);
uint8_t Bcd_to_Dec(uint8_t bcd);
uint8_t Dec_to_Bcd(uint8_t dec_number);
void Set_Time_RTC(uint8_t hour, uint8_t min, uint8_t sec);
void Set_Date_RTC(uint8_t day, uint8_t mon, uint8_t year);

//Setup General
void setup(void){
    //Oscilador
    OSCCON = 0B01110001;                                    //Oscilador a 8Mhz
    
    //I2C
    I2C_Master_Init(100000);
    
    //Entradas y salidas
    ANSEL = 0;                                              //Ningún puerto analógico
    ANSELH = 0;
    TRISA = 0;                                              //Puertos como salidas
    TRISB = 0;
    TRISD = 0;
    TRISE = 0;
    
    //Valores iniciales de variables y puertos
    PORTA = 0;
    PORTB = 0;
    PORTD = 0;
    PORTE = 0;
    
    return;
}

//Loop
void main(void) {
    setup();
    
    Set_Time_RTC(1, 15, 0);                                 //Funcion de valores iniciales de la hora
    Set_Date_RTC(9, 8, 23);                                 //Funcion de valores iniciales de la fecha
    
    Lcd_Init();                                             //Iniciar LCD
    Lcd_Clear();                                            //Limpiar LCD
    Lcd_Set_Cursor(1, 1);
    Lcd_Write_String("POT1");
    
    while(1){
        I2C_Master_Start();                                 //Iniciar comunicación I2C
        I2C_Master_Write(0x11);                             //Recibir datos de 0x10
        POT1 = I2C_Master_Read(0);
        I2C_Master_Stop();
        __delay_ms(200);
               
        I2C_Master_Start();                                 //Iniciar comunicacion I2C
        I2C_Master_Write(0xD0);                             //Escribit datos a 0xD0
        I2C_Master_Write(0x00);                             //Elegir el registro 0x00
        I2C_Master_RepeatedStart();
        I2C_Master_Write(0xD1);                             //Recibir datos de 0xD0
        SEG = Bcd_to_Dec(I2C_Master_Read(0));
        I2C_Master_Stop();
        __delay_ms(200);
        
        I2C_Master_Start();                                 //Repetir
        I2C_Master_Write(0xD0);
        I2C_Master_Write(0x01);                             //Registro de minutos
        I2C_Master_RepeatedStart();
        I2C_Master_Write(0xD1);
        MIN = Bcd_to_Dec(I2C_Master_Read(0));
        I2C_Master_Stop();
        __delay_ms(200);
        
        I2C_Master_Start();
        I2C_Master_Write(0xD0);
        I2C_Master_Write(0x02);                             //Registro de horas
        I2C_Master_RepeatedStart();
        I2C_Master_Write(0xD1);
        HOR = Bcd_to_Dec(I2C_Master_Read(0));
        I2C_Master_Stop();
        __delay_ms(200);
        
        I2C_Master_Start();
        I2C_Master_Write(0xD0);
        I2C_Master_Write(0x04);                             //Registro de día
        I2C_Master_RepeatedStart();
        I2C_Master_Write(0xD1);
        DAY = Bcd_to_Dec(I2C_Master_Read(0));
        I2C_Master_Stop();
        __delay_ms(200);
        
        I2C_Master_Start();
        I2C_Master_Write(0xD0);
        I2C_Master_Write(0x05);                             //Registro de mes
        I2C_Master_RepeatedStart();
        I2C_Master_Write(0xD1);
        MON = Bcd_to_Dec(I2C_Master_Read(0));
        I2C_Master_Stop();
        __delay_ms(200);
        
        I2C_Master_Start();
        I2C_Master_Write(0xD0);
        I2C_Master_Write(0x06);                             //Registro de año
        I2C_Master_RepeatedStart();
        I2C_Master_Write(0xD1);
        YEAR = Bcd_to_Dec(I2C_Master_Read(0));
        I2C_Master_Stop();
        __delay_ms(200);

        Lcd_Set_Cursor(1, 6);
        sprintf(cDAY, "%d", DAY);                            //Convertir ADC en tipo char
        if(DAY < 10){                                        //Si es unidad, ubicar 0 al frente
            cDAY[1] = cDAY[0];
            cDAY[0] = '0';
        }
        Lcd_Write_String(cDAY);                              //Escribir en LCD
        Lcd_Set_Cursor(1, 8);
        Lcd_Write_Char('/');

        Lcd_Set_Cursor(1, 9);
        sprintf(cMON, "%d", MON);                            //Convertir valor en tipo char
        if(MON < 10){
            cMON[1] = cMON[0];
            cMON[0] = '0';
        }
        Lcd_Write_String(cMON);
        Lcd_Set_Cursor(1, 11);
        Lcd_Write_Char('/');

        Lcd_Set_Cursor(1, 12);
        sprintf(cYEAR, "%d", YEAR);                          //Convertir valor en tipo char
        Lcd_Write_String(cYEAR);

        Lcd_Set_Cursor(2, 1);
        sprintf(cPOT1, "%d", POT1);                          //Convertir ADC en tipo char
        if(POT1 < 10){
            cPOT1[2] = cPOT1[0];
            cPOT1[1] = '0';
            cPOT1[0] = '0';
        }
        else if(POT1 < 100){
            cPOT1[2] = cPOT1[1];
            cPOT1[1] = cPOT1[0];
            cPOT1[0] = '0';
        }
        Lcd_Write_String(cPOT1);
        
        
        Lcd_Set_Cursor(2, 6);
        sprintf(cHOR, "%d", HOR);                          //Convertir valor en tipo char
        if(HOR < 10){
            cHOR[1] = cHOR[0];
            cHOR[0] = '0';
        }
        Lcd_Write_String(cHOR);
        Lcd_Set_Cursor(2, 8);
        Lcd_Write_Char(':');

        Lcd_Set_Cursor(2, 9);
        sprintf(cMIN, "%d", MIN);                          //Convertir valor en tipo char
        if(MIN < 10){
            cMIN[1] = cMIN[0];
            cMIN[0] = '0';
        }
        Lcd_Write_String(cMIN);
        Lcd_Set_Cursor(2, 11);
        Lcd_Write_Char(':');

        Lcd_Set_Cursor(2, 12);
        sprintf(cSEG, "%d", SEG);                          //Convertir valor en tipo char
        if(SEG < 10){
            cSEG[1] = cSEG[0];
            cSEG[0] = '0';
        }
        Lcd_Write_String(cSEG);
    }
}

//Funciones
uint8_t Bcd_to_Dec(uint8_t bcd){
    uint8_t dec;                                            //Variable para guardar valor
    dec = ((bcd>>4)*10)+(bcd & 0b00001111);                 //Hacer un corrimiento de bits y sumar con la unidad
    return dec;                                             //Retornar valor
}

uint8_t Dec_to_Bcd(uint8_t dec_number){
    uint8_t bcd_number;                                     //Variable para almacenar dato bcd
    bcd_number = 0;                                         //Limpiar numero
    while(1){                                               //Loop
        if (dec_number >= 10){                              //Convertir numero y repetir ciclo hasta que el numero sea menor que 10
            dec_number = dec_number - 10;                   //Restar 10
            bcd_number = bcd_number + 0b00010000;           //Ir sumando diez en bcd
        }
        else {                                              //Suma de números
            bcd_number = bcd_number + dec_number;           //Suma
            break;                                          //Salirse del loop
        }
    }
    return bcd_number;                                      //Retornar valor BCD
}

void Set_Time_RTC(uint8_t hour, uint8_t min, uint8_t sec){
    
    I2C_Master_Start();
    I2C_Master_Write(0xD0);                                 //Seleccionar RTC
    I2C_Master_Write(0x00);                                 //Registro de segundos
    I2C_Master_Write(Dec_to_Bcd(sec));                      //Valor inicial de segundos
    I2C_Master_Stop();
    
    I2C_Master_Start();
    I2C_Master_Write(0xD0);                                 //Seleccionar RTC
    I2C_Master_Write(0x01);                                 //Registro de minutos
    I2C_Master_Write(Dec_to_Bcd(min));                      //Valor inicial de minutos
    I2C_Master_Stop();
    
    I2C_Master_Start();
    I2C_Master_Write(0xD0);                                 //Seleccionar RTC
    I2C_Master_Write(0x02);                                 //Registro de horas
    I2C_Master_Write(Dec_to_Bcd(hour));                     //Valor inicial de horas
    I2C_Master_Stop();
}

void Set_Date_RTC(uint8_t day, uint8_t mon, uint8_t year){
    
    I2C_Master_Start();
    I2C_Master_Write(0xD0);                                 //Seleccionar RTC
    I2C_Master_Write(0x04);                                 //Registro de días
    I2C_Master_Write(Dec_to_Bcd(day));                      //Valor inicial de días
    I2C_Master_Stop();
    
    I2C_Master_Start();
    I2C_Master_Write(0xD0);                                 //Seleccionar RTC
    I2C_Master_Write(0x05);                                 //Registro de meses
    I2C_Master_Write(Dec_to_Bcd(mon));                      //Valor inicial de meses
    I2C_Master_Stop();
    
    I2C_Master_Start();
    I2C_Master_Write(0xD0);                                 //Seleccionar RTC
    I2C_Master_Write(0x06);                                 //Registro de años
    I2C_Master_Write(Dec_to_Bcd(year));                     //Valor inicial de años
    I2C_Master_Stop();
}