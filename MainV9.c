//#include "system.h"
//#include "uart2.h"

#include <p24fxxxx.h>
#include <stdio.h>
#include <string.h>

// Configuration Bits
#ifdef _PIC24FJ64GA004_ //Defined by MPLAB when using 24FJ64GA004 device
_CONFIG1(JTAGEN_OFF& GCP_OFF& GWRP_OFF& COE_OFF& FWDTEN_OFF& ICS_PGx1& IOL1WAY_ON)
_CONFIG2(FCKSM_CSDCMD& OSCIOFNC_OFF& POSCMOD_HS& FNOSC_PRI& I2C1SEL_SEC)
#else
_CONFIG1(JTAGEN_OFF& GCP_OFF& GWRP_OFF& COE_OFF& FWDTEN_OFF& ICS_PGx2)
_CONFIG2(FCKSM_CSDCMD& OSCIOFNC_OFF& POSCMOD_HS& FNOSC_PRI)
#endif

//Passsword
#define KEY "pass"
#define MAX_TEMP 512

void Uart_PrintString(char *s); //aqui para poder fazer debug com prints



//-------------------------------------------------UART------------------------------------
void Uart_Inicio(){
   //U2BRG=25; //Set Baudrate  9600
   U2BRG=12;  //funciona no proteus
   U2STA = 0;
   U2MODE = 0x8000; //Enable Uart for 8-bit data
   //no parity, 1 STOP bit
   U2STAbits.UTXEN = 1; //Enable Transmit
   // reset RX flag
   IFS1bits.U2RXIF = 0;
}

int Uart_Ready(){
    return U2STAbits.URXDA;
}

void Uart_PrintChar(char cha){//Print uma caracter no terminal
    while(U2STAbits.UTXBF == 1);//so se o registo estiver vazio
      U2TXREG = cha;//coloca o caracter no registo
}


char Uart_ReadChar() {
    char buff;
    while(IFS1bits.U2RXIF == 0);
    buff = U2RXREG;
    IFS1bits.U2RXIF = 0;
    return buff;
}


void Uart_ReadString(char *s) {
    unsigned int i = 0;
    s[i] = Uart_ReadChar();
    while(s[i] != '\0' && s[i] != '\n' && s[i] != '\r'){ //enquanto n for fim de string ou enter continuar a ler
        i++;
        s[i] = Uart_ReadChar();
    }
    s[i] = '\0'; 
}


void Uart_PrintString(char *s){//Print uma string no terminal
  int i;
  for(i=0; s[i] != '\0';i++)
  Uart_PrintChar(s[i]);
}
   
   
void  Uart_PrintDecimal(int Dec){
  char string[10];
  sprintf(string,"%d", Dec);
  Uart_PrintString(string);
}



//--------------------------------------------------------ADC-----------------------------------------
void ADC_Inicio(){ //RB2/SS1/AN2
AD1PCFG = 0xFFD2; // tenho q ver ainda mas basicamente por os pins que queremos a digital
AD1CON1 = 0x0000; // SAMP bit = 0 ends sampling and starts converting
AD1CSSL = 0;
AD1CON3 = 0x0002; // Manual Sample, Tad = 2 Tcy || Exemplo 17.1 tem isto a 0 para ter clock ser tcy
AD1CON2 = 0;
AD1CON1bits.ADON = 1; // turn ADC ON
}


void Delay(int milisegundo){ //Atrasa o programa 
    unsigned int i;
    unsigned long j;
    for (i = 0; i < milisegundo; i++)
        for (j = 0; j <= 120; j++)
            ;
}


int ADC_Read(int channel){
  int ADCValue;
  AD1CHS = channel; 
  AD1CON1bits.SAMP = 1; // start sampling...
  Delay(300); // Ensure the correct sampling time has elapsed before starting conversion.|| Not sure quantos ms por aqui
  AD1CON1bits.SAMP = 0; // start Converting
  while (!AD1CON1bits.DONE); // conversion done?
  ADCValue = ADC1BUF0; // yes then get ADC value
  return ADCValue;
}



//------------------------------------Motor-----------------------------
void Motor_Roda(int diff, int modo) {
    if (modo == 0){//serve para por o modo bronzear ou normal
    	if (diff >= 0) {
    		PORTAbits.RA0 = 1;
        	PORTAbits.RA1 = 1;
        	PORTAbits.RA6 = 0;
        	PORTAbits.RA7 = 0;
        	Uart_PrintString("Chapeu de sol -> Esquerda\r");
    	}else{
    		PORTAbits.RA0 = 0;
        	PORTAbits.RA1 = 0;
    		PORTAbits.RA6 = 1;
        	PORTAbits.RA7 = 1;
        	Uart_PrintString("Chapeu de sol -> Direita\r");
    	}
	}else if(modo == 1){
		if (diff < 0) {
    		PORTAbits.RA0 = 1;
        	PORTAbits.RA1 = 1;
        	PORTAbits.RA6 = 0;
        	PORTAbits.RA7 = 0;
        	Uart_PrintString("Chapeu de sol -> Esquerda\r");
    	}else{
    		PORTAbits.RA0 = 0;
        	PORTAbits.RA1 = 0;
    		PORTAbits.RA6 = 1;
        	PORTAbits.RA7 = 1;
        	Uart_PrintString("Chapeu de sol -> Direita\r");
    	}
	}
}

void Motor_Stop() {
    PORTAbits.RA0 = 0;
    PORTAbits.RA1 = 0;
	PORTAbits.RA6 = 0;
	PORTAbits.RA7 = 0;
}



//-------------------------------------------------------------------------------------
void menu(){
	Uart_PrintString("\r\rMenu:\r");
	Uart_PrintString("T-Temperatura\r");
	Uart_PrintString("P-Potenciometro/Ventuinha\r");
  	Uart_PrintString("L-LDR/Sensor de Luz\r");
	Uart_PrintString("D-Debug\r");  
}

void menu_2(){
	Uart_PrintString("\r\rDEBUG MENU\r");
    Uart_PrintString("K-Leds\r");
    Uart_PrintString("S-Sensores\r");
    Uart_PrintString("E-EXIT\r");
}

void led_show(){
	PORTAbits.RA0 = 1;
	Delay(300);
	PORTAbits.RA1 = 1;
	Delay(300);
	PORTAbits.RA2 = 1;
	Delay(300);
	PORTAbits.RA3 = 1;
	Delay(300);
	PORTAbits.RA4 = 1;
	Delay(300);
	PORTAbits.RA5 = 1;
	Delay(300);
	PORTAbits.RA6 = 1;
	Delay(300);
	PORTAbits.RA7 = 1;
	Delay(300);
	PORTAbits.RA7 = 0;
	Delay(300);
	PORTAbits.RA6 = 0;
	Delay(300);
	PORTAbits.RA5 = 0;
	Delay(300);
	PORTAbits.RA4 = 0;
	Delay(300);
	PORTAbits.RA3 = 0;
	Delay(300);	
	PORTAbits.RA2 = 0;
	Delay(300);
	PORTAbits.RA1 = 0;
	Delay(300);
	PORTAbits.RA0 = 0;
	Delay(1000);
	TRISAbits.TRISA7 = 0;
}


int Password_Check(){
  char pass[] = KEY;
  char input[50];
  Uart_PrintString("Insira a password: \r");
  Uart_ReadString(input);
  if (!strcmp(pass, input)){
    Uart_PrintString("Password correcta! \rBem Vindo! \r ");
    return 1;
  }else {
    Uart_PrintString("Password errada, tente novamente. \r");
    return 0;
  }
return 0;
}


int User_Input(int temp, int fan, int ldr_1, int ldr_2){
  char input;
  int menu_is_on=0;
  while(Uart_Ready()){
    input = Uart_ReadChar();
    switch(input){
      case 't':
        Uart_PrintString("\rTEMP-AN2->");
		Uart_PrintDecimal(temp);
		if (temp>MAX_TEMP)
			Uart_PrintString("\rVentoinha Ligada");
      	else
      		Uart_PrintString("\rVentoinha Desligada");
      menu();
      break;
      case 'p':
      	Uart_PrintString("\rFAN-AN3->");
    	Uart_PrintDecimal(fan);
      	menu();
      break;
      case 'l':
        Uart_PrintString("\rLDR_1-AN0->");
    	Uart_PrintDecimal(ldr_1);
    	Uart_PrintString("\rLDR_2-AN1->");
  		Uart_PrintDecimal(ldr_2);
      menu();
      break;
      case 'd':
      menu_2();
      input = Uart_ReadChar();
      	switch(input){
      		case 'k':
      		led_show();
      		menu();
      		break;
      		case 's':
      		menu_is_on=1;
      		while(menu_is_on){
      			Uart_PrintString("\rTEMP-AN0->");
    			Uart_PrintDecimal(ADC_Read(0));
    			Uart_PrintString("\rLDR_1-AN2->");
   				Uart_PrintDecimal(ADC_Read(2));
    			Uart_PrintString("\rLDR_1-AN3->");
    			Uart_PrintDecimal(ADC_Read(3));
    			Uart_PrintString("\rFAN-AN3->");
    			Uart_PrintDecimal(ADC_Read(5));
    			Uart_PrintString("\r");
    			if(Uart_Ready())
    				menu_is_on=0;
    		}
    		Uart_ReadChar();
    		menu();
      		break;
      		case 'e':
      		menu();
      		break;
      	}
      
      break;
      default:
      Uart_PrintString("Erro, caracter invalido\r");


    }
  }

}


int main(void){
   
	Uart_Inicio();
	ADC_Inicio();

	//Inicialisacao de leds e butoes
	TRISDbits.TRISD6 = 1; //botao 3
	TRISDbits.TRISD7 = 1;  //botao 6
	TRISDbits.TRISD13 = 1; //botao 4
	TRISAbits.TRISA0 = 0;
	TRISAbits.TRISA1 = 0;
	TRISAbits.TRISA2 = 0;
	TRISAbits.TRISA3 = 0;
	TRISAbits.TRISA4 = 0;
	TRISAbits.TRISA5 = 0;
	TRISAbits.TRISA6 = 0;
	TRISAbits.TRISA7 = 0;
   
  	int i = 0;
  	int diff = 0;
  	int modo = 0;
  	int destino = 0;
  	int fan_on = 0;
  	int LDR_1 = 0;
  	int LDR_2 = 0;
  	int TEMP = 0;
  	int FAN = 0;

	Uart_PrintString("Trabalho 2 - SAD\r");
	while(PORTDbits.RD13);//se o botao 4 estiver primido entrar
  	while (!Password_Check());//fica aqui ate ter a pass correta  

  	menu();

	while(1){
    	LDR_1=ADC_Read(2);
    	LDR_2=ADC_Read(3);
    	TEMP=ADC_Read(0);
    	FAN=ADC_Read(5);

    	diff = abs(LDR_1-LDR_2);

		User_Input(TEMP, FAN, LDR_1, LDR_2);

		if(!PORTDbits.RD6){ // 0 modo normal, 1 modo bronze
			if(modo == 1){
				modo = 0;
				Uart_PrintString("Modo Normal Ativo\r");
			}
			else if (modo == 0){
				modo = 1;
				Uart_PrintString("Modo Bronzear Ativo\r");
			}
		}

		if (TEMP>MAX_TEMP && fan_on==0){
			//ligar o motor da ventuinha
			Uart_PrintString("Ventoinha Ligada\r");
			Uart_PrintString("Velocidade a ");
			Uart_PrintDecimal(FAN);
			Uart_PrintString("\r");
			fan_on=1;
		}else if(TEMP<MAX_TEMP && fan_on==1){
			Uart_PrintString("Ventoinha Desligada\r");
			fan_on=0;
		}


   		if(diff>=50){
   			destino = 0;
     		Motor_Roda(LDR_1-LDR_2, modo);
      		Delay(2000);
      		Motor_Stop();
    	}else if (destino == 0){
     		Motor_Stop();
      		Uart_PrintString("Chapeu de sol chegou ao destino\r");
      		destino = 1;
    	}

    	//Uart_PrintString("1");
  
    
  }
}

