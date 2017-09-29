#include <avr/io.h>					//Registres interns i periferics
#include <avr/interrupt.h>			//Per les interrupccio

#define F_CPU 16000000L				//16MHz 16.000.000Hz Tinc una precisiò de 0.06º aprox
#define MIN_PULS_WITH 0.704
#define MAX_PULS_WITH 2.512
#define CLOCKS_PER_ANGLE ((MAX_PULS_WITH-MIN_PULS_WITH)/0.0005)/180.0
#define CLOCKS_PER_CICLE 40000
#define CLOCKS_PER25	  5000

#define N_SERVOS 2

typedef enum {FALSE,TRUE}bool;


float angles[N_SERVOS]={90.0,72.0};	//vector que guarda el valor dels angles a escriure als servos
volatile bool puja=TRUE;			//Per saber si estem al high o al low dins els primers 2.5 ms
volatile unsigned char n_cicles=0;	//Per saber quin dels 8 servos és
volatile int clocks_a_comptar=2500;	//Iniciem el servo 0 a 1.250 ms
unsigned long long time_now=0;

int angles2clocks(float _angle);	//Funció que transforma l'angle en l'amplada del pols

int main(void)
{
	DDRB = 0b00011;					//PB0 i PB1 com a sortides
	PORTB = 0x01;					//PB0 en high
	OCR1A =clocks_a_comptar;		//numero de clocks en high per donar l'angle correcte 
	TCCR1B |= (1 << WGM12); 		//mode 4, CTC al OCR1A (torna el comptador de temps a 0 quan arriba al registre esmentat)
	TIMSK1 |= (1 << OCIE1A);		//definim interrupcciò al coincidir
	TCCR1B |= (1 << CS11); 			//prescaler 8 que dona una precisió  de 5 microsegons
	//Timer 0 per comptar el temps
	TIMSK0=
	sei();							//activa les interrupccions
	while (1);						
}

int angles2clocks(float _angle)		//converteix el valor de l'angle que volem en el numero de clocks que ha de fer el rellotge per donar-lo
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
