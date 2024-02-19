/*
 * ADC_Potenciometro.c
 *
 * Created: 21/01/2024 22:57:26
 * Author : paolo
 */ 

#define F_CPU 16000000L	
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>


//Creando macros


#define		Leer_ADC_DDR			DDRC		//Registro para configurar el Puerto C como entrada	
#define		BAUD					9600		//Configurado los baudios
#define		UBRR_VAL	((F_CPU/16UL/BAUD)-1)

//Configurar puertos

//Configurar el puerto C0 como entrada

void configurarPuerto(){
	
	Leer_ADC_DDR &= ~(1 << DDC0);
	
}


//Configurar ADC

void configurarADC(){
	
	
	//*******************************************************************************//
									/* CONFIGURANDO ADMUX */
	//*******************************************************************************//
	
	
	//Colocando que la referencia sea Vcc es decir 5 V
	
	ADMUX &= ~(1<<REFS1);
	ADMUX |= (1 << REFS0);

	//Colocando alineación del resultado obtenido, al colocar en 0 ADCH se alinea a la izquierda, los bits mas significativos van a ADCH
	
	ADMUX &= ~(1 << ADLAR ); 
	
	//Seleccionar el canal C0 como ADC - MUX  0000 ADC0, ESTA CONFIGURACIÓN SE DEBE COLOCAR EN EL LOOP
	
	 ADMUX &= ~(1<<MUX3)|~(1<<MUX2)|~(1<<MUX1)|~(1<<MUX0); 


	//*******************************************************************************//
				/* CONFIGURACION ADCSRA - ADC CONTROL AND STATIS REGISTER A */
	//*******************************************************************************//

	
		//Habilitando ADC
		
		ADCSRA |= (1<<ADEN);
		
		/*Para iniciar una nueva conversión del ADC   es recomendable hacerlo en el loop del programa
		ADCSRA = (1<<ADSC);  
		*/
		
		//ADATE se coloca en 0 porque la lectura de ADC será en cualquier instante de tiempo y no depende de interrupciones externas.
		ADCSRA = (1<<ADATE);
		
		
		/*ADIF en 1 indica que la conversión ADC terminó, se lo coloca manualmente en 0 al iniciar una nueva lectura, solo se utiliza cuando se manejan interrupciones 
		ADCSRA = (0<<ADIF);
		*/
		
		
		//Se desactiva ADIE porque no se van a generar interrupciones al momento de terminar la lectura del ADC
		ADCSRA &= ~(1<<ADIE);
		
		
		//factor de division entre la frecuencia de reloj del sistema y el reloj interno del ADC Pre escalador para este caso 16Mhz/8 = 2Mhz
		ADCSRA |= (1<<ADPS0);
		ADCSRA |= (1<<ADPS1);
		ADCSRA |= (1<<ADPS2);
		
	
		
	//*******************************************************************************//
							/* CONFIGURACION REGISTRO ADCSRB */
	//*******************************************************************************//
		
	//Configurando registros
	//ADTS
	ADCSRB =	0;
	


	//*******************************************************************************//
							/* CONFIGURACION REGISTRO DIDR0  */
	//*******************************************************************************//
	

	DIDR0 |= (1 << ADC0D);
	
}


//Configurando comunicación puerto serie

	/*
	Inicializa la comunicación serial del microcontrolador
	*/


void uart_init() {
	// Configurar la velocidad de transmisión, divide en 2 el valor UBRR_VAL
	/* UBRR0H y UBRR0L estos registros determinan la velocidad de transimision de UART, se deben configurar para 
	que coincida con la velocidad de transmisión deseada */
	
	UBRR0H = (unsigned char)(UBRR_VAL >> 8);
	UBRR0L = (unsigned char)UBRR_VAL;
	
	// Habilitar la transmisión y recepción
	
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);
	
	// Configurar el formato de trama: 8 bits de datos, 1 bit de parada
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}


	/*Función para enviar un caracter (byte) a través del UART*/
void uart_putchar(char c) {
	
	// Esperar a que el búfer de transmisión esté vacío.
	/*
	Espera hasta que el registro de datos de transmisión (UDR0) esté vacío (UDRE0 en 1), lo que significa que 
	está listo para recibir un nuevo byte.
	*/  
	 
	while (!(UCSR0A & (1 << UDRE0))); 
	
	// Enviar el carácter
	UDR0 = c;
}


		/* Funcion para enviar cadena de caracteres */

void uart_puts(const char *str) {
	// Enviar una cadena de caracteres
	while (*str) {										/*Recorre la cadena de caracteres y envia cada caracter usando la funcion uart_putchar, la transmisión se da hasta que llegue un nulo*/
		uart_putchar(*str++);
	}
}




int main(void)
{
    uart_init();
    configurarPuerto();
	configurarADC();
		
	/* Replace with your application code */
    while (1) 
    {
		
		 ADCSRA |= (1<<ADSC);
		
		
	
		
				while (ADCSRA & (1 << ADSC));  //  Se mantiene en espera mientras el bit ADSC (Start Conversion) en el registro ADCSRA esté en 1. Cuando la conversión finaliza, ADSC se establece automáticamente en 0, y el bucle termina.
				uint16_t resultado_ADC0 = ADC;
		
		
		         uart_puts("Resultado ADC0: ");
		         
		         // Convertir el resultado del ADC a una cadena y enviarlo
		         char buffer[10];  // Suficiente espacio para almacenar un número entero de 16 bits en formato decimal
		         sprintf(buffer, "%u\n", resultado_ADC0);	//Convierte el resultado del ADC a una cadena de caracteres	
		         uart_puts(buffer);
		
				_delay_ms(1000);
    }
	return 0;
}

