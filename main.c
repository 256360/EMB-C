/*
@author 256360
 */
#include <avr/io.h>
#define F_CPU 16000000ul
#include<util/delay.h>
#define BAUD 9600
#define BAUD_PRESCALE ((F_CPU/(16ul*BAUD))-1)
long int i;
void port() //Initialize and declare required ports
{
    DDRB=00000001;
    DDRD &= ~(0 << PIND0); //configuring pin 0 of port d as input pin
    DDRD &= ~(0 << PIND1); //configuring pin 1 of port d as input pin
    PORTD |= 1 << PIND1;//set pin 1 of port d i.e HIGH
    PORTD |= 1 << PIND0; //set pin 0 of port d i.e HIGH
    DDRD|=(1<<DDD6);
}
void InitADC() //Initialize ADC
{
    ADMUX=(1<<REFS0);
    ADCSRA=(1<<ADEN)|(7<<ADPS0);
}
uint16_t ReadADC(uint8_t ch) //ADC Quantization and Sampling
{
    //ADC channel Ch must be 0-7
    ADMUX&=0xf8;
    ch=ch&0b00000111;
    ADMUX|=ch;
    //single conversion
    ADCSRA|=(1<<ADSC);
    //wait for conversion to complete
    while(!(ADCSRA&(1<<ADIF)));
    //cleanADIF by writing one
    ADCSRA|=(1<<ADIF);
    return(ADC);
}
void compare(uint16_t temp)
{
    if(temp<=200)
       {
           OCR0A=50;             // PWM Duty cycle: 20%
       }
      else if(temp>200 && temp<=500)
       {
        OCR0A=101;              //PWM Duty cycle: 40%
       }
      else if(temp>500 && temp<=700)
        {
            OCR0A=177;           //PWM Duty cycle: 70%
        }
      else if(temp>700 && temp<=1024)
         {
             OCR0A=241;         //PWM Duty cycle: 95%
         }
}
void USART_init()
{
    //Enable transmitter
    UCSR0B=(1<<TXEN0);
    // Set 8 bit data for transmission
    UCSR0C =(1<<UCSZ01)|(1<<UCSZ00);
    //Set BAUD RATE
    UBRR0L=(BAUD_PRESCALE);
    UBRR0H=(BAUD_PRESCALE>>8);
}
void display(uint16_t temp)
{
    if(temp<=200)
       {   unsigned char data[] = "Temperature: 20 deg Celsius ";
           i=0;
           while(data[i]!=0)
           {
               while(!(UCSR0A &(1<<UDRE0)));//wait for empty transmitter buffer
                UDR0 =data[i];               //put data into buffer and  transmit to serial monitor
                i++;
           }
       }
      else if(temp>200 && temp<=500)
       {
        unsigned char data[] = "Temperature: 25 deg Celsius ";
        i=0;
        while(data[i]!=0)
           {
               while(!(UCSR0A &(1<<UDRE0)));//wait for empty transmitter buffer
                UDR0 =data[i];             //put data into buffer and  transmit to serial monitor
                i++;
           }
       }
      else if(temp>500 && temp<=700)
        {
            unsigned char data[] = "Temperature: 29 deg Celsius ";
            i=0;
            while(data[i]!=0)
           {
               while(!(UCSR0A &(1<<UDRE0)));//wait for empty transmitter buffer
                UDR0 =data[i];               //put data into buffer and  transmit to serial monitor
                i++;
           }
        }
      else if(temp>700 && temp<=1024)
         {
             unsigned char data[] = "Temperature: 33 deg Celsius ";
             i=0;
             while(data[i]!=0)
           {
               while(!(UCSR0A &(1<<UDRE0)));//wait for empty transmitter buffer
                UDR0 =data[i];           //put data into buffer and  transmit to serial monitor
                i++;
           }
         }
}
uint16_t temp;  //initialize temp
int main(void)
{
     /** @brief initialize and declare required ports
      *
      */
    port();
    while(1)
    {

       if(bit_is_clear(PIND, 0)) // check if passenger is present or not by checking pin 0 of Port D
        {

            if(bit_is_clear(PIND, 1)) // check if button pressed or not by checking pin 1 of port D
            {
                PORTB=0b00000001; //LED ON if passenger present and heater button pressed
                _delay_ms(200);
                /** @brief ADC Configuration
                *
                */
                InitADC();
                TCCR0A|=(1<<COM0A1)|(1<<WGM00)|(1<<WGM01); //Select fast OWM with MAX TOP and SET PWM in non inverting mode
                TCCR0B|=(1<<CS00);  //select No pre-scale
                /** @brief Read i/p to the channel, sampling & quantization process occurs
                *   @param ADC channel number in use
                *   @return digital equivalent of analog input signal
                */
                temp = ReadADC(0);
                USART_init();
                /** @brief compares temperature value and gives PWM output
                *   @param ADC value store in temp variable
                */
                compare(temp);
                /** @brief display messages in Serial Monitor using USART
                 *  @param ADC value store in temp variable
                 */
                display(temp);

            }
            else
            {
            PORTB=0b00000000; // LED OFF
             _delay_ms(150);

            }
        }
        else
        {
            PORTB=0b00000000; //LED OFF
            _delay_ms(150);

        }
    }
    return 0;
}
