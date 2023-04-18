/*
 * Robocik.c
 *
 * Created: 14.05.2022 17:06:39
 */ \
#define F_CPU 8000000UL 
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

uint8_t L = 0;
uint8_t R = 0;
uint8_t tmp = 0;

ISR(INT0_vect){  //Zdefiniowanie przerwania
	tmp = 1;
	PORTB=PORTB | 0b00000001;
	_delay_ms(2000);
	PORTB=PORTB & 0b11111110;
}

//Obrót serwa w lewo
void RotateLeft(){
	OCR1A = 1499;                                             //Zmiana wype³nienia PWM (Obrót serwa)
	sei();
	PORTD=PORTD | 0b00010000;                                 //Symulacja i sygnalizowanie badania odleg³oœci
	_delay_ms(2000);
	PORTD=PORTD & 0b11101111;
	cli();
	L = tmp;
	tmp = 0;
}

//Obrót serwa w prawo
void RotateRight(){
	OCR1A = 4499;                                             //Zmiana wype³nienia PWM (Obrót serwa)
	sei();
	PORTD=PORTD | 0b00001000;                                 //Symulacja i sygnalizowanie badania odleg³oœci.
	_delay_ms(2000);
	PORTD=PORTD & 0b11110111;
	cli();
	R = tmp;
	tmp = 0;
}

//Obrót serwa na wprost
void RotateFront(){
	OCR1A = 2999; //Zmiana wype³nienia PWM 
}

//Obrót w lewo
void TurnLeft(){
	PORTD=PORTD | 0b00010000; //Sygnalizacja obrotu
	PORTB = PORTB | 0b00101000; //Obrót
	_delay_ms(1500);
	PORTB = PORTB & 0b11000011; //Zatrzymanie silników
	PORTD=PORTD & 0b11101111;
}

//Obrót w prawo
void TurnRight(){
	PORTD=PORTD | 0b00001000; //Sygnalizacja obrotu
	PORTB = PORTB | 0b00010100; //Obrót
	_delay_ms(1500);
	PORTB = PORTB & 0b11000011; //Zatrzymanie silników
	PORTD=PORTD & 0b11110111;
}

//Zawracanie
void TurnBack(){
	PORTD=PORTD | 0b00011000; //Sygnalizacja obrotu
	PORTB = PORTB | 0b00010100; //Obrót
	_delay_ms(3000);
	PORTB = PORTB & 0b11000011; //Zatrzymanie silników
	PORTD=PORTD & 0b11100111;
}

int main(void) {
	uint8_t temp;
	temp = EICRA;
	EICRA = temp | (1<<ISC01|0<<ISC00);
	temp = EIMSK;
	EIMSK = temp | (1<<INT0);
	DDRD = 0b00011000;
    DDRB = 0b00111110;
	cli();
	
	temp = TCCR1A;
	TCCR1A = temp | (1<<COM1A1 | 0<<COM1A0 | 1<<WGM11 | 0<<WGM10); //Ustawienie nieodwracajacego wyjscia PWM i trybu PWM w liczniku Timer1
	temp = TCCR1B;
	TCCR1B = temp | (1<<WGM12 | 1<<WGM13 | 0<<CS12 | 1<<CS11 | 0<<CS10); //Ustawienie prescalera
	ICR1 = 39999;
	OCR1A = 2999;
	
	
	
	while (1) {
		PORTB = PORTB | 0b00100100;                              //Jazda na wprost
		if (PIND & 0b00000100) {                                  //Detekcja przeszkody z przodu. PIND2
			PORTB = PORTB & 0b11000011;                          //Zatrzymanie
			_delay_ms(500);
			temp=EIFR;
			EIFR=temp|(1<<INTF0);                                //kasowanie przerwania
			
			RotateLeft();				                         //Badanie terenu	
			RotateRight();
			RotateFront();
			if (L^R) {				                             //Podjêcie decyzji o kierunku jazdy (Domyœlnie w lewo)
				if (L) {
					TurnLeft();
				}
				else {
					TurnRight();
				}
			}
			else if (L&R) {
				TurnBack();
			}
			else {
				TurnLeft();
			}
		}
	}		
}