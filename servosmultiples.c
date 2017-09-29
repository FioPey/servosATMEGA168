#include <avr/io.h>					//Registres interns i periferics
#include <avr/interrupt.h>			//Per les interrupccio

#define F_CPU 16000000L				//16MHz 16.000.000Hz Tinc una precisiò de 0.06º aprox
#define MIN_PULS_WITH 0.704
#define MAX_PULS_WITH 2.512
#define CLOCKS_PER_ANGLE ((MAX_PULS_WITH-MIN_PULS_WITH)/0.0005)/180.0
#define CLOCKS_PER_CICLE 40000
#define CLOCKS_PER25	  5000

#define N_SERVOS 2

float angles[N_SERVOS]={90.0,72.0};

int angles2clocks(float _angle);
typedef enum {FALSE,TRUE}bool;
volatile bool puja=TRUE;
volatile unsigned char n_cicles=0;
volatile int clocks_a_comptar=2500;

int main(void)
{
	DDRB = 0b00111;					//0b010000 el bit 5 és el pin 4 i el posa en output (hi posa un 1), la resta 0 (INPUT)
	PORTB = 0x01;						//un 0 al bit del pin fa que sigui low, un 1 el fa high ,si és input un 0 res, un 1 activa el pull-up (a +5V)
	OCR1A =clocks_a_comptar;			//numero de cloks per arribar al pols del servo
	TCCR1B |= (1 << WGM12); 		// Mode 4, CTC al OCR1A
	TIMSK1 |= (1 << OCIE1A);		//definim interrupcciò al coincidir
	TCCR1B |= (1 << CS11); 			//prescaler 8
	sei();							// activa les interrupccions
	while (1);
}

int angles2clocks(float _angle)
{
	if(_angle<1) _angle=1;
	else if (_angle>179) _angle=179;
	return (int)(MIN_PULS_WITH/0.0005 +CLOCKS_PER_ANGLE*_angle);
}

ISR (TIMER1_COMPA_vect)//Cada vegada que el comptador arribi al que se li ha dit es crida aquesta funciò
{
	if(n_cicles<N_SERVOS)
	{
		if(puja)//En HIGH
		{
			OCR1A=CLOCKS_PER25-clocks_a_comptar;//interrupcciò al temps que falta
			PORTB=0x0;
			puja=FALSE;
		}
		else
		{
			if(n_cicles+1<N_SERVOS)
			{
				clocks_a_comptar=angles2clocks(angles[n_cicles+1]);
				OCR1A=clocks_a_comptar;//Torna a posar al cicle el que falta
				PORTB=(1<<(n_cicles+1));
			}
			else OCR1A=CLOCKS_PER25;
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
		clocks_a_comptar=angles2clocks(angles[n_cicles]);
		OCR1A=clocks_a_comptar;
		PORTB=(1<<(n_cicles));
	}
}
