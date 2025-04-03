#include <xc.inc>

; When assembly code is placed in a psect, it can be manipulated as a
; whole by the linker and placed in memory.  
;
; In this example, barfunc is the program section (psect) name, 'local' means
; that the section will not be combined with other sections even if they have
; the same name.  class=CODE means the barfunc must go in the CODE container.
; PIC18 should have a delta (addressible unit size) of 1 (default) since they
; are byte addressible.  PIC10/12/16 have a delta of 2 since they are word
; addressible.  PIC18 should have a reloc (alignment) flag of 2 for any
; psect which contains executable code.  PIC10/12/16 can use the default
; reloc value of 1.  Use one of the psects below for the device you use:

; psect   barfunc,local,class=CODE,delta=2 ; PIC10/12/16
; psect   barfunc,local,class=CODE,reloc=2 ; PIC18

/*
     global _bar ; extern of bar function goes in the C source file
_bar:
    movf PORTA,w    ; here we use a symbol defined via xc.inc
    return
*/
    
; CONFIG
  CONFIG  FOSC = INTOSC        ; Oscillator Selection bits (INTOSC oscillator: CLKIN function disabled)
  CONFIG  BOREN = ON           ; Brown-out Reset Enable (Brown-out Reset enabled)
  CONFIG  WDTE = OFF          ; Watchdog Timer Enable (WDT enabled)
  CONFIG  PWRTE = ON         ; Power-up Timer Enable bit (PWRT enabled)
  CONFIG  MCLRE = ON          ; MCLR Pin Function Select bit (MCLR pin function is MCLR)
  CONFIG  CP = OFF              ; Code Protection bit (Program memory code protection is disabled)
  CONFIG  LVP = OFF          ; Low-Voltage Programming Enable (Low-voltage programming enabled)
  CONFIG  LPBOR = OFF          ; Brown-out Reset Selection bits (BOR enabled)
  CONFIG  BORV = HI           ; Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
  CONFIG  WRT = OFF             ; Flash Memory Self-Write Protection (Write protection off)
  
  
PSECT res_vect,class=CODE,delta=2

res_vect:
    goto START    
int_vect:
    org 0x04
    btfsc PIR1,6	;sprawdzenie flagi od ADC
    goto ADC_OPERAT
    goto TIMER
START:
    org 0x10
CLK_CONFIG: 
    movlw 0x60
    movwf OSCCON	;ustawia taktowania zegara na 8 MHz
PORTA_CONFIG:
    clrf PORTA
    movlw 0x01
    movwf TRISA		;ustawia pin RA0 portu A jako wejscie dla ADC
    movlw 0x01
    movwf ANSELA	;ustawiamy piny portu A - RA0 jako analog, 1 do 2 jako I/O cyfrowe
    movlw 0xD8		
    movwf OPTION_REG	;TMR0 liczy co cykl instrukcji - Fosc/4
ADC_CONFIG:
    movlw 0x21
    movwf ADCON		;ustawia konwersje zegara ADC na Fosc/8,  wlacza ADC
INT_CONFIG:
    clrf PIR1		;wyczyszczenie flagi przerwan ADC
    bsf PIE1,6		;wlaczenie przerwan od ADC
    movlw 0xC0
    movwf INTCON	;wlaczamy modul przerwan i przerwania od ukladow peryferyjnych
    
;---------------------------------------- START PROGRAMU ------------------------------------------------------
    
PROGRAM:
     bsf ADCON,1		;start konwersji ADC
; -------------czekanie na przerwanie od ADC i zalaczenie diody LED
   
LOOP:
    goto LOOP

LED_BLINK_2:
    clrf 0x76
    clrf 0x77
    movlw 0x15		;czas dla 1,963s pomiedzy mignieciem diody
    movwf 0x75
LED_BLINKING:    
    decfsz 0x77,1
    goto LED_BLINKING
    decfsz 0x76,1
    goto LED_BLINKING
    decfsz 0x75,1
    goto LED_BLINKING
LED_ON_OFF:    
    bsf PORTA,1		;wlaczenie LED na czas 98,176ms
    decfsz 0x77,1
    goto LED_ON_OFF
    decfsz 0x76,1
    goto LED_ON_OFF
    bcf PORTA,1
    goto LED_BLINK_2
;----------------------------------- PRZERWANIE OD ADC ------------------------------ 
ADC_OPERAT:
    movf ADRES,0	;przepisanie wartosci ADC do rejestru W
    bcf PIR1,6		;czyszczenie flagi od ADC
    movwf 0x50
    bsf PORTA,1
;zakres ADC dzielony na 5 czesci (0-50,51-101,102-152;153-203;204-255) dla napiecia  (0 - 4,2)V 

TIME_1:
    movf 0x50,0
    sublw 0x32		;odejmuje rejestr W od liczby 50
    btfss STATUS,0
    goto TIME_2		
    movwf 0x61
    movlw 0x01		;wpisanie wartosci 1 do W ( mnoznik petli dla 1s)
    movwf 0x73
    goto TMR0_SET
TIME_2:
    movf 0x50,0
    sublw 0x65		;odejmuje rejestr W od liczby 101
    btfss STATUS,0
    goto TIME_3
    movwf 0x61
    movlw 0x05		;wpisanie wartosci 5 do W ( mnoznik petli dla 5s)
    movwf 0x73
    goto TMR0_SET
TIME_3:
    movf 0x50,0
    sublw 0x98		;odejmuje rejestr W od liczby 152
    btfss STATUS,0
    goto TIME_4
    movwf 0x61
    movlw 0x0F		;wpisanie wartosci 15 do W ( mnoznik petli dla 15s)
    movwf 0x73
    goto TMR0_SET
TIME_4:
    movf 0x50,0
    sublw 0xCB		;odejmuje rejestr W od liczby 203
    btfss STATUS,0
    goto TIME_5
    movwf 0x61
    movlw 0x1E		;wpisanie wartosci 30 do W ( mnoznik petli dla 30s)
    movwf 0x73
    goto TMR0_SET    
TIME_5:
    movf 0x50,0
    sublw 0xFF		;odejmuje rejestr W od liczby 255
    btfss STATUS,0
    goto TIME_2
    movwf 0x61
    movlw 0x3C		;wpisanie wartosci 60 do W ( mnoznik petli dla 60s)
    movwf 0x73
TMR0_SET:
    movlw 0x1B		;wpisanie do rejestru 70h liczby 26+1d ( 1/(Fosc/4)*256=128us) --> 128*26=3328us -->3328*3=9984us 
    movwf 0x70
    movlw 0x96		;wpisanie do rejestru 71h liczby 3+1d
    movwf 0x71
    movlw 0x02		;wpisanie do rejestru 72h liczby 100+1d (100*10000us=1s)
    movwf 0x72
    movlw 0x0D		; wpisanie liczby 12+1 do 79h dla 12*128us
    movwf 0x79
    movlw 0x82		;wpisanie do TMR0 liczby 130 ,  ( brakujace 1600us uwzgledniajac 2 cykle na wykonanie instrukcji)
    movwf TMR0		;1600/128=12,5 --> 0,5*128us=64us , 1 cykl to 0,5us
    bcf INTCON,2	;czysczenie flagi TMR0
    bsf INTCON,5	;wlaczenie przerwania od TMR0
    retfie
TIMER:
    bcf INTCON,2
    btfsc 0x78,0
    goto T_COUNT
    decfsz 0x79
    retfie
    bsf 0x78,0
    movlw 0x0D
    movwf 0x79
  T_COUNT:
    decfsz 0x70,1	; >
    retfie
    movlw 0x1B
    movwf 0x70
    decfsz 0x71,1	;   >    czas wykonania tych instrukcji to 8us
    retfie	  
;-------miganie LED co 0,5s --------------
    btfsc PORTA,1	;>	    
    goto LED_OFF	;>	    	
LED_ON:				    
    bsf PORTA,1		;>
    goto GO		;>
LED_OFF:
    bcf PORTA,1		;>
    nop			;>
;--------------------    
GO:   
    movlw 0x96		;>		    ZNAK ;> OZNACZA INSTRUKCJE DO WYLICZENIE LICZBY 231 
    movwf 0x71		;>
    decfsz 0x72,1	;>
    retfie		
    movlw 0x02		;>
    movwf 0x72		;>
    bcf 0x78,0		;>
    movlw 0xA0		;>
    movwf TMR0		;wpisanie do TMR0 liczby 231 ( brakujace 13,5 us uwzgledniajac 2 cykle na wykonanie instrukcji)
    decfsz 0x73,1
    retfie
    bcf INTCON,2	;czysczenie flagi TMR0
    bcf INTCON,5	;wylaczenie przerwania od TMR0
    clrf 0x70
    clrf 0x71
    movlw 0x0B		;wpisanie 10+1d do rejestru w celu opoznienia dla 1 sekundy
    movwf 0x72
    bsf PORTA,1
    bsf PORTA,2
DELAY_1S:		; ((((3cykle*255)+2)*255)+3cykle*255+2cykle)*10+3cykle*10+2cykle+1cykl=1963553cykle 
    decfsz 0x70,1	;--> 0,5*1963553cykle = 981776,5us = 981,7765ms
    goto DELAY_1S
    decfsz 0x71,1
    goto DELAY_1S
    decfsz 0x72,1
    goto DELAY_1S
    bcf PORTA,2
    bcf PORTA,1
    goto LED_BLINK_2

    end res_vect