#define F_CPU 16000000L				//16MHz 16.000.000Hz Tinc una precisiò de 0.06º aprox
#define MIN_PULS_WITH 0.704
#define MAX_PULS_WITH 2.512
#define CLOCKS_PER_ANGLE ((MAX_PULS_WITH-MIN_PULS_WITH)/0.0005)/180.0
#define CLOCKS_PER_CICLE 40000
#define ANGLE 115.0// recta a 25º
#define N_CLOCKS (MIN_PULS_WITH/0.0005 +CLOCKS_PER_ANGLE*ANGLE)
#include <avr/io.h>					//Registres interns i periferics
#include <avr/interrupt.h>			//Per les interrupccio



typedef enum {FALSE,TRUE}bool;
volatile bool puja=TRUE;


int main(void)
{
	DDRB = 0x10;					//0b010000 el bit 5 és el pin 4 i el posa en output (hi posa un 1), la resta 0 (INPUT)
	PORTB = 0;						//un 0 al bit del pin fa que sigui low, un 1 el fa high ,si és input un 0 res, un 1 activa el pull-up (a +5V)
	OCR1A =N_CLOCKS;				//numero de cloks per arribar al pols del servo
	TCCR1B |= (1 << WGM12); 		// Mode 4, CTC al OCR1A
	TIMSK1 |= (1 << OCIE1A);		//definim interrupcciò al coincidir
	TCCR1B |= (1 << CS11); 			//prescaler 8
	sei();							// activa les interrupccions
	while (1);
}

ISR (TIMER1_COMPA_vect)//Cada vegada que el comptador arribi al que se li ha dit es crida aquesta funciò
{
	if(puja)//En HIGH
	{
		OCR1A=CLOCKS_PER_CICLE-N_CLOCKS;//interrupcciò al temps que falta
		PORTB=0x10;					//pin 4 que és el bit 5 el posa en high
		puja=FALSE;
	}
	else
	{
		OCR1A=N_CLOCKS;//Torna a posar al cicle el que falta
		PORTB=0;		//Apaga el pols
		puja=TRUE;
	}
}


/*
>>> 1/16000000.0
6.25e-08
>>> 1/16000000.0 * 1000
6.25e-05
>>> 1/16000000.0 * 1000 * 8
0.0005
>>> 1/(1/16000000.0 * 1000 * 8)
2000.0
>>> 180.0/2000.0
0.09
>>> 2000.0/180.0
11.11111111111111
>>> 3000.0/180
16.666666666666668
>>> 180.0/3000
0.06
*/
