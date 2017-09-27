#define F_CPU 16000000L				//16MHz 16.000.000Hz Tinc una precisiò de 0.06º aprox
#define MIN_PULS_WITH 0.704
#define MAX_PULS_WITH 2.500
#define CLOCKS_PER_ANGLE ((MAX_PULS_WITH-MIN_PULS_WITH)/0.0005)/180.0
#define CLOCKS_PER_CICLE 40000
#define CLOCKS_PER25	  5000
#define ANGLE1	176 
#define ANGLE2 	95.0
#define N_CLOCKS1 (MIN_PULS_WITH/0.0005 +CLOCKS_PER_ANGLE*ANGLE1)
#define N_CLOCKS2 (MIN_PULS_WITH/0.0005 +CLOCKS_PER_ANGLE*ANGLE2)
#include <avr/io.h>					//Registres interns i periferics
#include <avr/interrupt.h>			//Per les interrupccio



typedef enum {FALSE,TRUE}bool;
volatile bool puja=TRUE;
volatile unsigned char n_cicles=0;

int main(void)
{
	DDRB = 0x18;					//0b010000 el bit 5 és el pin 4 i el posa en output (hi posa un 1), la resta 0 (INPUT)
	PORTB = 0x10;						//un 0 al bit del pin fa que sigui low, un 1 el fa high ,si és input un 0 res, un 1 activa el pull-up (a +5V)
	OCR1A =N_CLOCKS1;				//numero de cloks per arribar al pols del servo
	TCCR1B |= (1 << WGM12); 		// Mode 4, CTC al OCR1A
	TIMSK1 |= (1 << OCIE1A);		//definim interrupcciò al coincidir
	TCCR1B |= (1 << CS11); 			//prescaler 8
	sei();							// activa les interrupccions
	while (1);
}

ISR (TIMER1_COMPA_vect)//Cada vegada que el comptador arribi al que se li ha dit es crida aquesta funciò
{
	if(n_cicles==0)
	{
		if(puja)//En HIGH
		{
			OCR1A=CLOCKS_PER25-N_CLOCKS1;//interrupcciò al temps que falta
			PORTB=0x0;					//pin 4 que és el bit 5 el posa en high
			puja=FALSE;
		}
		else
		{
			OCR1A=N_CLOCKS2;//Torna a posar al cicle el que falta
			puja=TRUE;
			PORTB=0x08;
			n_cicles++;
		}
	}
	else if(n_cicles==1)
	{
		if(puja)//En HIGH
		{
			OCR1A=CLOCKS_PER25-N_CLOCKS2;//interrupcciò al temps que falta
			PORTB=0x00;
			puja=FALSE;
		}
		else
		{
			OCR1A=CLOCKS_PER25;//Torna a posar al cicle el que falta
			puja=TRUE;
			n_cicles++;
		}
	}
	else
	{
		OCR1A=CLOCKS_PER25;//Torna a posar al cicle el que falta
		n_cicles++;
	}
	if(n_cicles>7)
	{
		n_cicles=0;
		OCR1A=N_CLOCKS1;
		PORTB=0x10;
	}
}
