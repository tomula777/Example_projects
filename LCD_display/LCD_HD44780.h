/* 
 * File:   LCD_HD44780.h
 * Author: Dell
 *
 * Created on 25 kwietnia 2024, 12:25
 */

#ifndef LCD_HD44780_H
#define	LCD_HD44780_H

#ifdef	__cplusplus
extern "C" {
#endif
    
//------------------------------------------------------------------------------
//          THE DIRECTIVES                          ( Customize to Your display )
//------------------------------------------------------------------------------

#define _XTAL_FREQ                16000000          // Frequency oscilator in Hz;
#define delay_us(x) __delay_us(x)                   // Macro for user's compiler macro e.g. __delay_us(x)
#define delay_ms(x) __delay_ms(x)                   // Macro for user's compiler macro e.g. __delay_ms(x)
#define LCD_4_BIT_MODE                              // Options: LCD_4_BIT_MODE , LCD_8_BIT_MODE;
#define CHECK_BUSY_FLAG                             // Option for check busy flag LCD;
#define WRITE_READ_LCD_BY_LOOP
#ifndef WRITE_READ_LCD_BY_LOOP
    #define WRITE_LCD_BY_INTERRUPT
    #define TMR_ON_OFF                  T0CONbits.TMR0ON    //*
    #define TMR_ON                      1                   //*
    #define TMR_OFF                     0                   //*
    #define TIMER_L_REG                 TMR0L               //*
    #define TIMER_H_REG                 TMR0H               //*
    #define INT_TIME_TMR                200                 //* The TMR interrupt is generated after this value (microseconds);
    #define T_TIME_TMR                  (unsigned int)(INT_TIME_TMR * (_XTAL_FREQ / 4000000.0))
    #define INIT_TMR_INT                (0-T_TIME_TMR)
    #define T_TIME_TMR_2MS              (unsigned int)(2000 * (_XTAL_FREQ / 4000000.0))
    #define INIT_TMR_INT_2MS            (0-T_TIME_TMR_2MS)
#endif //  WRITE_READ_LCD_BY_LOOP  
#define LCD_LINES                       2           // LCD_4X32, LCD_4X40, ... (1 ,2, 4, 8} X {8, 16, 20, 40, 32, 80}
#define LCD_CHARS                      16           // -genarally (lines)X(chars);
#define OUTPUT_SET                      0           // used with the data direction register to set MCU port register;
#define INPUT_SET                       1           // used with the data direction register to set MCU port register;

#define INT_ENA_GIE                 INTCONbits.GIE      //*
#define INT_ENA_BIT                 INTCONbits.TMR0IE   //*
#define INT_FLAG                    INTCONbits.TMR0IF   //*
#define RS_LINE_DIR                 TRISBbits.TRISB0    //*
#define RW_LINE_DIR                 TRISBbits.TRISB1    //*  
#define E1_LINE_DIR                 TRISBbits.TRISB2    //*
#define RS_LINE_LCD                 LATBbits.LB0        //*
#define RW_LINE_LCD                 LATBbits.LB1        //*
#define E1_LINE_LCD                 LATBbits.LB2        //*
#if (LCD_CHARS==40 && LCD_LINES==4)
#define E2_LINE_LCD                 PORTAbits.RA3       //*
#define E2_LINE_DIR                 TRISAbits.TRISA3    //*
#endif  // (LCD_CHARS==40 && LCD_LINES==4)
#define E_LINE_LCD

#ifdef LCD_8_BIT_MODE
#define DB0_LINE_DIR                TRISBbits.TRISB0    //*
#define DB1_LINE_DIR                TRISBbits.TRISB1    //*
#define DB2_LINE_DIR                TRISBbits.TRISB2    //*
#define DB3_LINE_DIR                TRISBbits.TRISB3    //*
#define DB0_OUT_LCD                 LATBbits.LB0        //*
#define DB1_OUT_LCD                 LATBbits.LB1        //*
#define DB2_OUT_LCD                 LATBbits.LB2        //*
#define DB3_OUT_LCD                 LATBbits.LB3        //*
#define DB0_IN_LCD                  PORTBbits.RB0       //*
#define DB1_IN_LCD                  PORTBbits.RB1       //*
#define DB2_IN_LCD                  PORTBbits.RB2       //*
#define DB3_IN_LCD                  PORTBbits.RB3       //*
#endif  // LCD_8_BIT_MODE
#if defined(LCD_4_BIT_MODE)|| defined(LCD_8_BIT_MODE)
#define DB4_LINE_DIR                TRISAbits.TRISA2    //*
#define DB5_LINE_DIR                TRISBbits.TRISB5    //*
#define DB6_LINE_DIR                TRISBbits.TRISB6    //*
#define DB7_LINE_DIR                TRISBbits.TRISB7    //*
#define DB4_OUT_LCD                 LATAbits.LA2        //*
#define DB5_OUT_LCD                 LATBbits.LB5        //*
#define DB6_OUT_LCD                 LATBbits.LATB6      //*
#define DB7_OUT_LCD                 LATBbits.LB7        //*
#define DB4_IN_LCD                  PORTAbits.RA2       //*
#define DB5_IN_LCD                  PORTBbits.RB5       //*
#define DB6_IN_LCD                  PORTBbits.RB6       //*
#define DB7_IN_LCD                  PORTBbits.RB7       //*
#endif  //  defined(LCD_4_BIT_MODE)|| defined(LCD_8_BIT_MODE)

//------------------------------------------------------------------------------
//          Code instructions HD44780               ( See the datasheet to understand the description )
//------------------------------------------------------------------------------

//_____________Clear display____________________
extern const unsigned char LCD_CLR;

//_____________Return home______________________
extern const unsigned char LCD_HOME;

//_____________Entry mode set___________________
extern const unsigned char LCD_WRITE_R;             // Moves the cursor to the right, the display is frozen
extern const unsigned char LCD_WRITE_L;             // Moves the cursor to the left, the display is frozen
extern const unsigned char LCD_WRITE_MOV_R;         // Moves the display to the right, the cursor is frozen
extern const unsigned char LCD_WRITE_MOV_L;         // Moves the display to the left, the cursor is frozen

//_____________Display on/off control___________
extern const unsigned char LCD_ON;                  // Display on
extern const unsigned char LCD_OFF;                 // Display off
extern const unsigned char CURSOR_ON;               // Cursor on
extern const unsigned char CURSOR_OFF;              // Cursor off
extern const unsigned char CUR_BLINK_ON;            // The character indicated by the cursor blinks
extern const unsigned char CUR_BLINK_OFF;           // The character indicated by the cursor does not blink

//_____________Cursor or display shift__________
extern const unsigned char LCD_SHIFT_R;             // Shifts the display to the right
extern const unsigned char LCD_SHIFT_L;             // Shifts the display to the left
extern const unsigned char CUR_SHIFT_R;             // Moves the cursor to the right
extern const unsigned char CUR_SHIFT_L;             // Moves the cursor to the left

//_____________Function set_____________________
extern const unsigned char INTERFACE_8_BIT;         // Sets the interface to a length of 8 bits
extern const unsigned char INTERFACE_4_BIT;         // Sets the interface to a length of 4 bits
extern const unsigned char LCD_ONE_LINE;            // Sets one line ( top ) to be displayed
extern const unsigned char LCD_TWO_LINE;            // Sets two lines to be displayed
extern const unsigned char LCD_FONT_5X7;            // Sets the character font to 5x7 dots
extern const unsigned char LCD_FONT_5X10;           // Sets the character font to 5x10 dots

//_____________Set CGRAM adress_________________
extern const unsigned char CGRAM_SET_ADDRESS;       // Sets CGRAM adress with other 6 bits

//_____________Set DDRAM adress_________________
extern const unsigned char DDRAM_SET_ADDRESS;       // Sets DDRAM adress with other 6 bits


//------------------------------------------------------------------------------
//          Variables and functions declarations
//------------------------------------------------------------------------------

//***********************************************************************************

#if !(LCD_CHARS==40 && LCD_LINES==4)
    void Write_Command(unsigned char);
    //                (command code )                                               etc. Write_Command(LCD_ON | CURSOR_OFF | CUR_BLINK_OFF);
#endif // !(LCD_CHARS==40 && LCD_LINES==4)
#if (LCD_CHARS==40 && LCD_LINES==4)
    void Write_Command(unsigned char, unsigned char);
    //                (command code, controller  )                                  etc. Write_Command(LCD_ON | CURSOR_OFF | CUR_BLINK_OFF, E1);
#endif // !(LCD_CHARS==40 && LCD_LINES==4)
    void Write_Data_DDRAM(unsigned char*,unsigned char,unsigned char);
    //                   (text ,    row      ,   collumn   )                        etc. Write_Data_DDRAM("Smile :)",0,7);
    void Write_Data_CGRAM(unsigned char*,unsigned char, unsigned char);
    //                   (tab pointer, address, number of characters)               etc. Write_Data_CGRAM("$&#",0,3); // It means it starts from 0, max 8 characters
    void Read_Data_DDRAM(unsigned char*, unsigned char, unsigned char, unsigned char);
    //                   ( tab pointer, row , collumn, number of characters)        etc. Read_Data_DDRAM(tab pointer,0,5);
    //void Read_Data_CGRAM(unsigned char*, unsigned char,  unsigned char);
    //                   (tab pointer, character position, number of characters)    etc. Read_Data_CGRAM(tab pointer,0,8);  // max 8 characters
    void Init_By_Instruct(void); 
    void Init_Dir_To_LCD(void);
    void Init_Dir_From_LCD(void);
    void Write_To_LCD(unsigned char*);
    unsigned char Read_From_LCD(void);
    void Write_Read_LCD_Loop(void);
    void Int_Ena_Dis(void);
    void Check_Busy_Flag(void);
    inline void Ena_Line_LCD_ON();
    inline void Ena_Line_LCD_OFF();
    
    
    extern unsigned char CGRAM_ADDRESS;
    extern unsigned char DDRAM_ADDRESS;
    extern unsigned char* p_data_write;
    extern unsigned char* p_data_read;
    extern unsigned char number_of_chars;
    extern unsigned char lcd_busy;
    extern const unsigned char LCD_lines;                  
    extern const unsigned char LCD_chars;
    extern unsigned char dir_to_LCD;
    extern unsigned char dir_from_LCD;
    extern unsigned char int_flag;
    #define INT_FLAG_LCD int_flag
    extern unsigned char r_row;
    extern unsigned char c_col;
    
#ifdef	__cplusplus
}
#endif

#endif	/* LCD_HD44780_H */

