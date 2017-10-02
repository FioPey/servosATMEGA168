#include <avr/io.h>					//Registres interns i periferics
#include <avr/interrupt.h>			//Per les interrupccio
#include <math.h>					//pel cosinos

#define F_CPU 				16000000L				//16MHz 16.000.000Hz Tinc una precisiò de 0.06º aprox
#define MIN_PULS_WITH 		0.704
#define MAX_PULS_WITH 		2.512
#define CLOCKS_PER_ANGLE ((MAX_PULS_WITH-MIN_PULS_WITH)/0.0005)/180.0
#define CLOCKS_PER_CICLE 	40000
#define CLOCKS_PER25	  	 5000
#define PI 3.14159265358979323846


#define N_SERVOS 			    2
#define T_REFRESH 			   20.0 			//ms

#define T			1.0
#define AMPLITUT 20
#define W    2.0*PI/T


typedef enum {FALSE,TRUE}bool;


const float a0[N_SERVOS]={90.0,72.0};	//vector que guarda el valor dels angles a escriure als servos
float angles[N_SERVOS]={90.0,72.0};	//vector que guarda el valor dels angles a escriure als servos
volatile bool puja=TRUE;			//Per saber si estem al high o al low dins els primers 2.5 ms
volatile unsigned char n_cicles=0;	//Per saber quin dels 8 servos és
volatile int clocks_a_comptar=2500;	//Iniciem el servo 0 a 1.250 ms
unsigned long long time_now=0;
unsigned long t_ant=0;
float t=0;

unsigned long millis();
int angles2clocks(float _angle);	//Funció que transforma l'angle en l'amplada del pols

int main(void)
{
	int i=0;
	DDRB = 0b00011;					//PB0 i PB1 com a sortides
	PORTB = 0x01;					//PB0 en high
	OCR1A =clocks_a_comptar;		//numero de clocks en high per donar l'angle correcte 
	TCCR1B |= (1 << WGM12); 		//mode 4, CTC al OCR1A (torna el comptador de temps a 0 quan arriba al registre esmentat)
	TIMSK1 |= (1 << OCIE1A);		//definim interrupcciò al coincidir
	TCCR1B |= (1 << CS11); 			//prescaler 8 que dona una precisió  de 5 microsegons
	
	// Timer0control de temps
	TCCR0B = _BV(CS00);	//Timer normal 
	TIMSK0 = _BV(TOIE0); 			//habilitat el desbordament
	    
	sei();							//activa les interrupccions
	t_ant=millis();
	while (1)
	{
		if(millis()-t_ant>T_REFRESH)
		{
			t+=((float)(millis()-t_ant))/1000.00;
			t_ant=millis();
			for(i=0;i<N_SERVOS;i++)
			{
				angles[i]=a0[i]-AMPLITUT*sin(W*t);
			}
		}
	}						
}

int angles2clocks(float _angle)		//converteix el valor de l'angle que volem en el numero de clocks que ha de fer el rellotge per donar-lo
{
	if(_angle<1) _angle=1;
	else if (_angle>179) _angle=179;
	return (int)(MIN_PULS_WITH/0.0005 +CLOCKS_PER_ANGLE*_angle);
}

unsigned long millis()
{
	unsigned long _millis=((time_now)<<8)+TCNT0;
	_millis=_millis*0.0000625;
	return _millis;
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

ISR(TIMER0_OVF_vect) 		//Overflow timer 0
{
	time_now++;
}
