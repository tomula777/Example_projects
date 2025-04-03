/* 
 * File:   LCD_HD44780.c
 * Author: Dell
 *
 * Created on 30 marca 2024, 15:00
 */
#include<xc.h>

#include"LCD_HD44780.h"
#ifdef	__cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
//          Code instructions HD44780               ( See the datasheet to understand the description )
//------------------------------------------------------------------------------

//_____________Clear display____________________
const unsigned char LCD_CLR               =0x01;

//_____________Return home______________________
const unsigned char LCD_HOME              =0x02;

//_____________Entry mode set___________________
const unsigned char LCD_WRITE_R           =0x06;    // Moves the cursor to the right, the display is frozen
const unsigned char LCD_WRITE_L           =0x04;    // Moves the cursor to the left, the display is frozen
const unsigned char LCD_WRITE_MOV_R       =0x05;    // Moves the display to the right, the cursor is frozen
const unsigned char LCD_WRITE_MOV_L       =0x07;    // Moves the display to the left, the cursor is frozen

//_____________Display on/off control___________
const unsigned char LCD_ON                =0x0C;    // Display on
const unsigned char LCD_OFF               =0x08;    // Display off
const unsigned char CURSOR_ON             =0x0A;    // Cursor on
const unsigned char CURSOR_OFF            =0x08;    // Cursor off
const unsigned char CUR_BLINK_ON          =0x09;    // The character indicated by the cursor blinks
const unsigned char CUR_BLINK_OFF         =0x08;    // The character indicated by the cursor does not blink

//_____________Cursor or display shift__________
const unsigned char LCD_SHIFT_R           =0x1C;    // Shifts the display to the right
const unsigned char LCD_SHIFT_L           =0x18;    // Shifts the display to the left
const unsigned char CUR_SHIFT_R           =0x14;    // Moves the cursor to the right
const unsigned char CUR_SHIFT_L           =0x10;    // Moves the cursor to the left

//_____________Function set_____________________
const unsigned char INTERFACE_8_BIT       =0x30;    // Sets the interface to a length of 8 bits
const unsigned char INTERFACE_4_BIT       =0x20;    // Sets the interface to a length of 4 bits
const unsigned char LCD_ONE_LINE          =0x20;    // Sets one line ( top ) to be displayed
const unsigned char LCD_TWO_LINE          =0x28;    // Sets two lines to be displayed
const unsigned char LCD_FONT_5X7          =0x20;    // Sets the character font to 5x7 dots
const unsigned char LCD_FONT_5X10         =0x24;    // Sets the character font to 5x10 dots

//_____________Set CGRAM adress_________________
const unsigned char CGRAM_SET_ADDRESS      =0x40;    // Sets CGRAM adress with other 6 bits

//_____________Set DDRAM adress_________________
const unsigned char DDRAM_SET_ADDRESS      =0x80;    // Sets DDRAM adress with other 6 bits


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
    //                   (tab pointer, address, number of characters)               etc. Write_Data_CGRAM("$&#",0,3); // It means it starts from 0 max 8 characters
    void Read_Data_DDRAM(unsigned char*, unsigned char, unsigned char,unsigned char);
    //                   ( tab pointer, row, collumn ,number of characters)         etc. Read_Data_DDRAM(pointer,0,5);
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
    
    
#if (LCD_CHARS==40 && LCD_LINES==4)
    unsigned char Ena_Line;
    const unsigned char E1 = 0, E2 = 1;
#endif // (LCD_CHARS==40 && LCD_LINES==4)
    unsigned char CGRAM_ADDRESS;
    unsigned char DDRAM_ADDRESS;
    unsigned char* p_data_write;
    unsigned char* p_data_read;
    unsigned char number_of_chars;
    unsigned char lcd_busy;
    const unsigned char LCD_lines = LCD_LINES;                  
    const unsigned char LCD_chars = LCD_CHARS;
    unsigned char dir_to_LCD;
    unsigned char dir_from_LCD;
    unsigned char int_flag = 1;
    unsigned char c_col;
    unsigned char r_row;
    
volatile union LCD
// <editor-fold defaultstate="collapsed" desc="{...}">
{
    unsigned word_LCD : 8;
    struct 
    {
        unsigned DB0 : 1,
                 DB1 : 1,
                 DB2 : 1,
                 DB3 : 1,
                 DB4 : 1,
                 DB5 : 1,
                 DB6 : 1,
                 DB7 : 1;
    };
} LCD_pin; // </editor-fold>
//------------------------------------------------------------------------------
void Init_By_Instruct(void)
{
    delay_ms(15);
    Init_Dir_To_LCD();
    RS_LINE_LCD = 0;
    RW_LINE_LCD = 0;
    unsigned char word = 0x30;
    unsigned char i;
#if !(LCD_CHARS==40 && LCD_LINES==4 )    
    for(i=0;i<3;i++)
    {
        LCD_pin.word_LCD=word;
        Ena_Line_LCD_ON();
        DB4_OUT_LCD=LCD_pin.DB4;
        DB5_OUT_LCD=LCD_pin.DB5;
        DB6_OUT_LCD=LCD_pin.DB6;
        DB7_OUT_LCD=LCD_pin.DB7;
        Ena_Line_LCD_OFF();
        if(!i)  delay_ms(5);
        else    delay_us(200);
    }
#ifdef LCD_8_BIT_MODE
    word = INTERFACE_8_BIT | LCD_TWO_LINE | LCD_FONT_5X7;   // default settings
#endif // LCD_8_BIT_MODE
#ifdef  LCD_4_BIT_MODE
    word = INTERFACE_4_BIT | LCD_TWO_LINE | LCD_FONT_5X7;   // default settings
#endif // LCD_4_BIT_MODE
    Write_To_LCD(&word);
    delay_us(50);
    word = LCD_OFF | CURSOR_OFF | CUR_BLINK_OFF;            // default settings
    Write_To_LCD(&word);
    delay_us(50);
    word = LCD_CLR ;                                        // default settings
    Write_To_LCD(&word);
    delay_ms(2);
    word = LCD_WRITE_R;                                     // default settings
    Write_To_LCD(&word);
    delay_us(50);
    word = LCD_ON | CURSOR_OFF | CUR_BLINK_OFF;             // default settings
    Write_To_LCD(&word);
    delay_us(50);
#endif // !(LCD_CHARS==40 && LCD_LINES==4 )
#if (LCD_CHARS==40 && LCD_LINES==4 )
    for(i=0;i<6;i++)
    {
        LCD_pin.word_LCD=word;
        if(i%2) Ena_Line=E2;
        else    Ena_Line=E1;
        Ena_Line_LCD_ON();
        DB4_OUT_LCD=LCD_pin.DB4;
        DB5_OUT_LCD=LCD_pin.DB5;
        DB6_OUT_LCD=LCD_pin.DB6;
        DB7_OUT_LCD=LCD_pin.DB7;
        Ena_Line_LCD_OFF();
        if(i<2)  delay_ms(5);
        else    delay_us(200);
    }
#ifdef LCD_8_BIT_MODE
    word = INTERFACE_8_BIT | LCD_TWO_LINE | LCD_FONT_5X7;   // default settings
#endif // LCD_8_BIT_MODE
#ifdef  LCD_4_BIT_MODE
    word = INTERFACE_4_BIT | LCD_TWO_LINE | LCD_FONT_5X7;   // default settings
#endif // LCD_4_BIT_MODE
    Ena_Line = E1;
    Write_To_LCD(&word);
    delay_us(50);
    Ena_Line = E2;
    Write_To_LCD(&word);
    delay_us(50);
    word = LCD_OFF | CURSOR_OFF | CUR_BLINK_OFF;            // default settings
    Ena_Line = E1;
    Write_To_LCD(&word);
    delay_us(50);
    Ena_Line = E2;
    Write_To_LCD(&word);
    delay_us(50);
    word = LCD_CLR ;                                        // default settings
    Ena_Line = E1;
    Write_To_LCD(&word);
    delay_ms(2);
    Ena_Line = E2;
    Write_To_LCD(&word);
    delay_ms(2);
    word = LCD_WRITE_R;                                     // default settings
    Ena_Line = E1;
    Write_To_LCD(&word);
    delay_us(50);
    Ena_Line = E2;
    Write_To_LCD(&word);
    delay_us(50);
    word = LCD_ON | CURSOR_OFF | CUR_BLINK_OFF;             // default settings
    Ena_Line = E1;
    Write_To_LCD(&word);
    delay_us(50);
    Ena_Line = E2;
    Write_To_LCD(&word);
    delay_us(50);
#endif // (LCD_LINES=4 && LCD_CHARS=40)    
}
//------------------------------------------------------------------------------   
void Init_Dir_To_LCD(void)
{
    dir_from_LCD = 0;
    dir_to_LCD = 1;
    RS_LINE_DIR = OUTPUT_SET;         // Set the data direction register pin as the output for the port register pin
    RW_LINE_DIR = OUTPUT_SET;
    E1_LINE_DIR = OUTPUT_SET;
#if (LCD_CHARS==40 && LCD_LINES==4)
    E2_LINE_DIR = OUTPUT_SET;
#endif  // (LCD_CHARS==40 && LCD_LINES==4)
#ifdef LCD_8_BIT_MODE
    DB0_LINE_DIR = OUTPUT_SET;
    DB1_LINE_DIR = OUTPUT_SET;
    DB2_LINE_DIR = OUTPUT_SET;
    DB3_LINE_DIR = OUTPUT_SET;
#endif  //  LCD_8_BIT_MODE
#if  defined(LCD_4_BIT_MODE) || defined(LCD_8_BIT_MODE)
    DB4_LINE_DIR = OUTPUT_SET;
    DB5_LINE_DIR = OUTPUT_SET;
    DB6_LINE_DIR = OUTPUT_SET;
    DB7_LINE_DIR = OUTPUT_SET;
#endif  //  defined(LCD_4_BIT_MODE) || defined(LCD_8_BIT_MODE)
}
//------------------------------------------------------------------------------
void Init_Dir_From_LCD(void)
{
    dir_to_LCD = 0;
    dir_from_LCD = 1;
    RS_LINE_DIR = OUTPUT_SET;         // Set the data direction register pin as the output for the port register pin
    RW_LINE_DIR = OUTPUT_SET;
    E1_LINE_DIR = OUTPUT_SET;
#if (LCD_CHARS==40 && LCD_LINES==4)
    E2_LINE_DIR = OUTPUT_SET;
#endif  // (LCD_CHARS==40 && LCD_LINES==4)
#ifdef LCD_8_BIT_MODE
    DB0_LINE_DIR = INPUT_SET;
    DB1_LINE_DIR = INPUT_SET;
    DB2_LINE_DIR = INPUT_SET;
    DB3_LINE_DIR = INPUT_SET;
#endif  //  LCD_8_BIT_MODE
#if  defined(LCD_4_BIT_MODE)|| defined(LCD_8_BIT_MODE)
    DB4_LINE_DIR = INPUT_SET;
    DB5_LINE_DIR = INPUT_SET;
    DB6_LINE_DIR = INPUT_SET;
    DB7_LINE_DIR = INPUT_SET;
#endif  //  defined(LCD_4_BIT_MODE)|| defined(LCD_8_BIT_MODE)
}
//------------------------------------------------------------------------------
void Write_To_LCD(unsigned char* p_word)
// <editor-fold defaultstate="collapsed" desc="{...}">
{
#ifdef LCD_8_BIT_MODE
    LCD_pin.word_LCD = *p_word;
    Ena_Line_LCD_ON();
    DB0_OUT_LCD = LCD_pin.DB0;
    DB1_OUT_LCD = LCD_pin.DB1;
    DB2_OUT_LCD = LCD_pin.DB2;
    DB3_OUT_LCD = LCD_pin.DB3;
    DB4_OUT_LCD = LCD_pin.DB4;
    DB5_OUT_LCD = LCD_pin.DB5;
    DB6_OUT_LCD = LCD_pin.DB6;
    DB7_OUT_LCD = LCD_pin.DB7;
    Ena_Line_LCD_OFF();
#endif // LCD_8_BIT_MODE
#ifdef LCD_4_BIT_MODE
    LCD_pin.word_LCD = *p_word;
    Ena_Line_LCD_ON();
    DB4_OUT_LCD = LCD_pin.DB4;
    DB5_OUT_LCD = LCD_pin.DB5;
    DB6_OUT_LCD = LCD_pin.DB6;
    DB7_OUT_LCD = LCD_pin.DB7;
    Ena_Line_LCD_OFF();
    Ena_Line_LCD_ON();
    DB4_OUT_LCD = LCD_pin.DB0;
    DB5_OUT_LCD = LCD_pin.DB1;
    DB6_OUT_LCD = LCD_pin.DB2;
    DB7_OUT_LCD = LCD_pin.DB3;
    Ena_Line_LCD_OFF();
#endif // LCD_4_BIT_MODE    
}// </editor-fold>
//------------------------------------------------------------------------------ 
unsigned char Read_From_LCD(void)
// <editor-fold defaultstate="collapsed" desc="{...}">
{
#ifdef LCD_8_BIT_MODE
    Ena_Line_LCD_ON();
    LCD_pin.DB0 = DB0_IN_LCD;
    LCD_pin.DB1 = DB1_IN_LCD;
    LCD_pin.DB2 = DB2_IN_LCD;
    LCD_pin.DB3 = DB3_IN_LCD;
    LCD_pin.DB4 = DB4_IN_LCD;
    LCD_pin.DB5 = DB5_IN_LCD;
    LCD_pin.DB6 = DB6_IN_LCD;
    LCD_pin.DB7 = DB7_IN_LCD;
    Ena_Line_LCD_OFF();
    return LCD_pin.word_LCD;
#endif // LCD_8_BIT_MODE
#ifdef LCD_4_BIT_MODE
    Ena_Line_LCD_ON();
    LCD_pin.DB4 = DB4_IN_LCD;
    LCD_pin.DB5 = DB5_IN_LCD;
    LCD_pin.DB6 = DB6_IN_LCD;
    LCD_pin.DB7 = DB7_IN_LCD;
    Ena_Line_LCD_OFF();
    Ena_Line_LCD_ON();
    LCD_pin.DB0 = DB4_IN_LCD;
    LCD_pin.DB1 = DB5_IN_LCD;
    LCD_pin.DB2 = DB6_IN_LCD;
    LCD_pin.DB3 = DB7_IN_LCD;
    Ena_Line_LCD_OFF();
    return LCD_pin.word_LCD;
#endif // LCD_4_BIT_MODE
}// </editor-fold>
//------------------------------------------------------------------------------ 
void Write_Data_DDRAM(unsigned char* p_tab,unsigned char row,unsigned char collumn)
// <editor-fold defaultstate="collapsed" desc="{...}">
{
    while (lcd_busy);
    if (*p_tab) 
    {
        p_data_write = p_tab;
        r_row = row;
        c_col= collumn;
#if (LCD_CHARS==40 && LCD_LINES==4)
        DDRAM_ADDRESS = DDRAM_SET_ADDRESS | (collumn + row%2 * 64);
        if(row==0 || row==1)        Write_Command(DDRAM_ADDRESS,E1);
        else if(row==2 || row==3)   Write_Command(DDRAM_ADDRESS,E2);
#endif // (LCD_CHARS==40 && LCD_LINES==4)
#if (LCD_CHARS==20 && LCD_LINES==4)
        if(row==0 || row==1)        DDRAM_ADDRESS = DDRAM_SET_ADDRESS | (collumn + row%2 * 64);
        else if(row==2 || row==3)   DDRAM_ADDRESS = DDRAM_SET_ADDRESS | (collumn + (row-1) * 20 + row%2 *44);
        Write_Command(DDRAM_ADDRESS);
#endif // (LCD_CHARS==20 && LCD_LINES==4)
#if (LCD_CHARS==16 && LCD_LINES==2)
        DDRAM_ADDRESS = DDRAM_SET_ADDRESS | (collumn + row%2 * 64);
        Write_Command(DDRAM_ADDRESS);
#endif // (LCD_CHARS==16 && LCD_LINES==2)
#ifdef WRITE_READ_LCD_BY_LOOP        
        Write_Read_LCD_Loop();   
#endif // WRITE_READ_LCD_BY_LOOP        
    }
}// </editor-fold>
//------------------------------------------------------------------------------
void Read_Data_DDRAM(unsigned char* p_tab, unsigned char row,unsigned char collumn,unsigned char num_of_chars)
// <editor-fold defaultstate="collapsed" desc="{...}">
{
    while (lcd_busy);
    if (num_of_chars)
    {
        p_data_read = p_tab;
        number_of_chars = num_of_chars;
        r_row = row;
        c_col= collumn;
#if (LCD_CHARS==40 && LCD_LINES==4)
        DDRAM_ADDRESS = DDRAM_SET_ADDRESS | (collumn + row%2 * 64);
        if(row==0 || row==1)        Write_Command(DDRAM_ADDRESS,E1);
        else if(row==2 || row==3)   Write_Command(DDRAM_ADDRESS,E2);
#endif // (LCD_CHARS==40 && LCD_LINES==4)
#if (LCD_CHARS==20 && LCD_LINES==4)
        if(row==0 || row==1)    DDRAM_ADDRESS = DDRAM_SET_ADDRESS | (collumn + row%2 * 64);
        else if(row==2 || row==3)   DDRAM_ADDRESS = DDRAM_SET_ADDRESS | (collumn + (row-1) * 20 + row%2 *44);
        Write_Command(DDRAM_ADDRESS);
#endif // (LCD_CHARS==20 && LCD_LINES==4)
#if (LCD_CHARS==16 && LCD_LINES==2)
        DDRAM_ADDRESS = DDRAM_SET_ADDRESS | (collumn + row%2 * 64);
        Write_Command(DDRAM_ADDRESS);
#endif // (LCD_CHARS==16 && LCD_LINES==2) 
#ifdef WRITE_READ_LCD_BY_LOOP        
        Write_Read_LCD_Loop();   
#endif // WRITE_READ_LCD_BY_LOOP
    }
}// </editor-fold>
//------------------------------------------------------------------------------
void Write_Data_CGRAM(unsigned char *char_pattern, unsigned char address, unsigned char num_of_chars)
// <editor-fold defaultstate="collapsed" desc="{...}">
{
    while (lcd_busy);
    if(dir_from_LCD)
      Init_Dir_To_LCD();
    if(num_of_chars)
    {
        unsigned char i;
        CGRAM_ADDRESS = CGRAM_SET_ADDRESS | address *8;
        RS_LINE_LCD = 0;
        RW_LINE_LCD = 0;
#if (LCD_CHARS==40 && LCD_LINES==4 )
        Ena_Line = E1;
        Write_To_LCD(&CGRAM_ADDRESS);
#endif // (LCD_CHARS==40 && LCD_LINES==4 )        
#if !(LCD_CHARS==40 && LCD_LINES==4 )
        Write_To_LCD(&CGRAM_ADDRESS);
#endif // !(LCD_CHARS==40 && LCD_LINES==4 )        
#ifdef  CHECK_BUSY_FLAG
        Check_Busy_Flag();
#else 
        delay_us(50);
#endif // CHECK_BUSY_FLAG
        for (i = 0; i < (8 * num_of_chars); i++) 
        {
            RS_LINE_LCD = 1;
            RW_LINE_LCD = 0;
            Write_To_LCD(char_pattern);
#ifdef  CHECK_BUSY_FLAG
        Check_Busy_Flag();
#else 
        delay_us(50);
#endif // CHECK_BUSY_FLAG  
            char_pattern++;
        }
    }
}// </editor-fold>
//------------------------------------------------------------------------------
/*void Read_Data_CGRAM(unsigned char *char_pattern, unsigned char address, unsigned char num_of_chars)
{
   while (lcd_busy);
    if (num_of_chars)
    {
        p_data_read = char_pattern;
        number_of_chars = num_of_chars;
#if (LCD_CHARS==40 && LCD_LINES==4)
        DDRAM_ADDRESS = DDRAM_SET_ADDRESS | (collumn + row%2 * 64);
        if(row==0 || row==1)        Write_Command(DDRAM_ADDRESS,E1);
        else if(row==2 || row==3)   Write_Command(DDRAM_ADDRESS,E2);
#endif // (LCD_CHARS==40 && LCD_LINES==4)
#if (LCD_CHARS==20 && LCD_LINES==4)
        DDRAM_ADDRESS = CGRAM_SET_ADDRESS | address*8;
        Write_Command(DDRAM_ADDRESS);
#endif // (LCD_CHARS==20 && LCD_LINES==4)
#if (LCD_CHARS==16 && LCD_LINES==2)
        DDRAM_ADDRESS = CGRAM_SET_ADDRESS | address*8;
        Write_Command(DDRAM_ADDRESS);
#endif // (LCD_CHARS==16 && LCD_LINES==2) 
#ifdef WRITE_LCD_BY_INTERRUPT       
        INT_FLAG_LCD = 1;
        Int_Ena_Dis();
#endif // WRITE_LCD_BY_INTERRUPT
#ifdef WRITE_READ_LCD_BY_LOOP        
        Write_Read_LCD_Loop();   
#endif // WRITE_READ_LCD_BY_LOOP
    }
    
}*/
//------------------------------------------------------------------------------
#if !(LCD_CHARS==40 && LCD_LINES==4)
void Write_Command(unsigned char word)
// <editor-fold defaultstate="collapsed" desc="{...}">
{
    while(lcd_busy); 
    if (dir_from_LCD)
       Init_Dir_To_LCD();
#ifdef  CHECK_BUSY_FLAG
            Check_Busy_Flag();
#endif  // CHECK_BUSY_FLAG 
    RS_LINE_LCD = 0;
    RW_LINE_LCD = 0;
    Write_To_LCD(&word);
#ifdef WRITE_LCD_BY_INTERRUPT    
    if(word<4)
    {
        TIMER_H_REG = INIT_TMR_INT_2MS >> 8;
        TIMER_L_REG = (unsigned char) INIT_TMR_INT_2MS;
    }
    else
    {
        TIMER_H_REG = INIT_TMR_INT >> 8;
        TIMER_L_REG = (unsigned char) INIT_TMR_INT;
    }
    INT_FLAG_LCD = 1;
    Int_Ena_Dis();
#endif // WRITE_LCD_BY_INTERRUPT   
}// </editor-fold>
//------------------------------------------------------------------------------
void Ena_Line_LCD_ON(void)
// <editor-fold defaultstate="collapsed" desc="{...}">
{
    delay_us(1);
    E1_LINE_LCD = 1;
    delay_us(1);
}// </editor-fold>
//------------------------------------------------------------------------------ 
void Ena_Line_LCD_OFF(void)
// <editor-fold defaultstate="collapsed" desc="{...}">
{
    delay_us(1);
    E1_LINE_LCD = 0;
    delay_us(1);
}// </editor-fold>
#endif // !(LCD_CHARS==40 && LCD_LINES==4)
//------------------------------------------------------------------------------
#if (LCD_CHARS==40 && LCD_LINES==4)
void Write_Command(unsigned char word, unsigned char Enable)
// <editor-fold defaultstate="collapsed" desc="{...}">
{
    while(lcd_busy);
    if (dir_from_LCD)
       Init_Dir_To_LCD();
    Ena_Line = Enable;
#ifdef CHECK_BUSY_FLAG        
    Check_Busy_Flag();
#endif // CHECK_BUSY_FLAG
    RS_LINE_LCD = 0;
    RW_LINE_LCD = 0;
    Write_To_LCD(&word);
#ifdef WRITE_LCD_BY_INTERRUPT    
    if(word<4)
    {
        TIMER_H_REG = INIT_TMR_INT_2MS >> 8;
        TIMER_L_REG = (unsigned char) INIT_TMR_INT_2MS;
    }
    else
    {
        TIMER_H_REG = INIT_TMR_INT >> 8;
        TIMER_L_REG = (unsigned char) INIT_TMR_INT;
    }
    INT_FLAG_LCD = 1;
    Int_Ena_Dis();
#endif // WRITE_LCD_BY_INTERRUPT 
}// </editor-fold>
//------------------------------------------------------------------------------ 
void Ena_Line_LCD_ON(void)
// <editor-fold defaultstate="collapsed" desc="{...}">
{
    delay_us(1);
    switch (Ena_Line)
    {
        case E1:
            E1_LINE_LCD = 1;
            break;
        case E2:
            E2_LINE_LCD = 1;
    }
    delay_us(1);
}// </editor-fold>
//------------------------------------------------------------------------------ 
void Ena_Line_LCD_OFF(void)
// <editor-fold defaultstate="collapsed" desc="{...}">
{
    delay_us(1);
    switch (Ena_Line)
    {
        case E1:
            E1_LINE_LCD = 0;
            break;
        case E2:
            E2_LINE_LCD = 0;
    }
    delay_us(1);
}// </editor-fold>
#endif // (LCD_CHARS==40 && LCD_LINES==4)
//------------------------------------------------------------------------------
#ifdef CHECK_BUSY_FLAG
void Check_Busy_Flag()     
{   DB7_LINE_DIR = INPUT_SET;
        RS_LINE_LCD = 0;
        RW_LINE_LCD = 1;
        Ena_Line_LCD_ON();
        while (DB7_IN_LCD) 
        {
            Ena_Line_LCD_OFF();
            Ena_Line_LCD_ON();
#ifdef LCD_8_BIT_MODE       
        }
#endif  //  LCD_8_BIT_MODE      
#ifdef LCD_4_BIT_MODE          
            Ena_Line_LCD_OFF();
            Ena_Line_LCD_ON();
        }
    Ena_Line_LCD_OFF();
    Ena_Line_LCD_ON();
#endif  //  LCD_4_BIT_MODE        
    Ena_Line_LCD_OFF();
    if(dir_to_LCD)  DB7_LINE_DIR = OUTPUT_SET;
    else            DB7_LINE_DIR = INPUT_SET;
}
#endif  //  defined(CHECK_BUSY_FLAG)   
//------------------------------------------------------------------------------
#ifdef WRITE_READ_LCD_BY_LOOP
void Write_Read_LCD_Loop()
{
    if(*p_data_write)
    {
        while(*p_data_write)
        {
        Check_Busy_Flag();
#if (LCD_CHARS==40 && LCD_LINES==4)
    if(c_col==40)
        {
            switch(r_row)
            {
                case 0:
                    r_row++;
                    c_col=0;
                    //Write_Command(DDRAM_SET_ADDRESS | 64);
                    Ena_Line = E1;
                    //Check_Busy_Flag();
                    break;
                case 1:
                    r_row++;
                    c_col=0;
                    //Write_Command(DDRAM_SET_ADDRESS | 0);
                    Ena_Line = E2;
                    //Check_Busy_Flag();
                    break;
                case 2:
                    r_row++;
                    c_col=0;
                    //Write_Command(DDRAM_SET_ADDRESS | 64);
                    Ena_Line = E2;
                    //Check_Busy_Flag();
                    break;
                case 3:
                    r_row=0;
                    c_col=0;
                    Ena_Line = E1;
            }
        }
#endif // (LCD_CHARS==40 && LCD_LINES==4)
#if (LCD_CHARS==20 && LCD_LINES==4)
    if(c_col==20)
        {
            switch(r_row)
            {
                case 0:
                    r_row++;
                    c_col=0;
                    Write_Command(DDRAM_SET_ADDRESS | 64);
                    Check_Busy_Flag();
                    break;
                case 1:
                    r_row++;
                    c_col=0;
                    Write_Command(DDRAM_SET_ADDRESS | 20);
                    Check_Busy_Flag();
                    break;
                case 2:
                    r_row++;
                    c_col=0;
                    Write_Command(DDRAM_SET_ADDRESS | 84);
                    Check_Busy_Flag();
                    break;
                case 3:
                    r_row=0;
                    c_col=0;
            }
        }
#endif // (LCD_CHARS==20 && LCD_LINES==4)
        RS_LINE_LCD = 1;
        RW_LINE_LCD = 0;
        Write_To_LCD(p_data_write);
        p_data_write++;
        c_col++;
        }
    }    
    else if(number_of_chars)
    {   
        while(number_of_chars)
        {    
        if(dir_to_LCD)
        Init_Dir_From_LCD();
        Check_Busy_Flag();
        RS_LINE_LCD = 1;
        RW_LINE_LCD = 1;
#if (LCD_CHARS==40 && LCD_LINES==4)
    if(c_col==40)
        {
            switch(r_row)
            {
                case 0:
                    r_row++;
                    c_col=0;
                    //Write_Command(DDRAM_SET_ADDRESS | 64);
                    Ena_Line = E1;
                    //Check_Busy_Flag();
                    break;
                case 1:
                    r_row++;
                    c_col=0;
                    //Write_Command(DDRAM_SET_ADDRESS | 0);
                    Ena_Line = E2;
                    //Check_Busy_Flag();
                    break;
                case 2:
                    r_row++;
                    c_col=0;
                    //Write_Command(DDRAM_SET_ADDRESS | 64);
                    Ena_Line = E2;
                    //Check_Busy_Flag();
                    break;
                case 3:
                    r_row=0;
                    c_col=0;
                    Ena_Line = E1;
            }
        }
#endif // (LCD_CHARS==40 && LCD_LINES==4)        
#if (LCD_CHARS==20 && LCD_LINES==4)
        if(c_col==20)
        {
            switch(r_row)
            {
                case 0:
                    r_row++;
                    c_col=0;
                    Write_Command(DDRAM_SET_ADDRESS | 64);
                    goto End_Loop;
                case 1:
                    r_row++;
                    c_col=0;
                    Write_Command(DDRAM_SET_ADDRESS | 20);
                    goto End_Loop;
                case 2:
                    r_row++;
                    c_col=0;
                    Write_Command(DDRAM_SET_ADDRESS | 84);
                    goto End_Loop;
                case 3:
                    r_row=0;
                    c_col=0;
            }
        }
#endif // (LCD_CHARS==20 && LCD_LINES==4)
        *p_data_read = Read_From_LCD();
        p_data_read++;
        number_of_chars--;
    End_Loop:  
        c_col++;
        }
    }        
}
#endif // WRITE_READ_LCD_BY_LOOP
//------------------------------------------------------------------------------
#ifdef WRITE_LCD_BY_INTERRUPT
void __interrupt(high_priority) MCU(void)
// <editor-fold defaultstate="collapsed" desc="{...}">
{   
    if (INT_ENA_BIT && INT_FLAG && INT_FLAG_LCD) 
    {
       TMR_ON_OFF = TMR_OFF;
#ifdef CHECK_BUSY_FLAG        
       Check_Busy_Flag();
#endif  // CHECK_BUSY_FLAG  
    if(*p_data_write) 
    {
       RS_LINE_LCD = 1;
       RW_LINE_LCD = 0;
#if (LCD_CHARS==40 && LCD_LINES==4)
    if(c_col==40)
        {
            switch(r_row)
            {
                case 0:
                    r_row++;
                    c_col=0;
                    Ena_Line = E1;
                    break;
                case 1:
                    r_row++;
                    c_col=0;
                    Ena_Line = E2;
                    break;
                case 2:
                    r_row++;
                    c_col=0;
                    Ena_Line = E2;
                    break;
                case 3:
                    r_row=0;
                    c_col=0;
                    Ena_Line = E1;
            }
        }
#endif // (LCD_CHARS==40 && LCD_LINES==4)        
#if (LCD_CHARS==20 && LCD_LINES==4)
        if(c_col==20)
        {
            switch(r_row)
            {
                case 0:
                    r_row++;
                    c_col=0;
                    lcd_busy=0;
                    Write_Command(DDRAM_SET_ADDRESS | 64);
                    lcd_busy=1;
                    goto Set_Timer;
                case 1:
                    r_row++;
                    c_col=0;
                    lcd_busy=0;
                    Write_Command(DDRAM_SET_ADDRESS | 20);
                    lcd_busy=1;
                    goto Set_Timer;
                case 2:
                    r_row++;
                    c_col=0;
                    lcd_busy=0;
                    Write_Command(DDRAM_SET_ADDRESS | 84);
                    lcd_busy=1;
                    goto Set_Timer;
                case 3:
                    r_row=0;
                    c_col=0;
            }
        }
#endif // (LCD_CHARS==20 && LCD_LINES==4)
        Write_To_LCD(p_data_write);
        p_data_write++;
    } 
    else if(number_of_chars)
    {
        if(dir_to_LCD)
          Init_Dir_From_LCD();
        RS_LINE_LCD = 1;
        RW_LINE_LCD = 1;
#if (LCD_CHARS==40 && LCD_LINES==4)
    if(c_col==40)
        {
            switch(r_row)
            {
                case 0:
                    r_row++;
                    c_col=0;
                    Ena_Line = E1;
                    braak;
                case 1:
                    r_row++;
                    c_col=0;
                    Ena_Line = E2;
                    break;
                case 2:
                    r_row++;
                    c_col=0;
                    Ena_Line = E2;
                    break;
                case 3:
                    r_row=0;
                    c_col=0;
                    Ena_Line = E1;
            }
        }
#endif // (LCD_CHARS==40 && LCD_LINES==4)        
#if (LCD_CHARS==20 && LCD_LINES==4)
        if(c_col==20)
        {
            switch(r_row)
            {
                case 0:
                    r_row++;
                    c_col=0;
                    lcd_busy=0;
                    Write_Command(DDRAM_SET_ADDRESS | 64);
                    lcd_busy=1;
                    goto Set_Timer;
                case 1:
                    r_row++;
                    c_col=0;
                    lcd_busy=0;
                    Write_Command(DDRAM_SET_ADDRESS | 20);
                    lcd_busy=1;
                    goto Set_Timer;
                case 2:
                    r_row++;
                    c_col=0;
                    lcd_busy=0;
                    Write_Command(DDRAM_SET_ADDRESS | 84);
                    lcd_busy=1;
                    goto Set_Timer;
                case 3:
                    r_row=0;
                    c_col=0;
            }
        }
#endif // (LCD_CHARS==20 && LCD_LINES==4)
        *p_data_read = Read_From_LCD();
        p_data_read++;
        number_of_chars--;
    }  
    else 
    {
        INT_FLAG_LCD = 0;
        TMR_ON_OFF = TMR_OFF;
        lcd_busy = 0;
    }
Set_Timer:
    c_col++;               
    if(lcd_busy) 
    {
        TIMER_H_REG = INIT_TMR_INT >> 8;
        TIMER_L_REG = (unsigned char) INIT_TMR_INT;
        TMR_ON_OFF = TMR_ON;
    }
    }
INT_FLAG = 0;
}// </editor-fold>
//------------------------------------------------------------------------------
void Int_Ena_Dis(void)
// <editor-fold defaultstate="collapsed" desc="{...}">
{
    if(INT_FLAG_LCD) 
    {
        INT_FLAG = 0;
        INT_ENA_BIT = 1;
        INT_ENA_GIE = 1;
        TMR_ON_OFF = TMR_ON;
        lcd_busy = 1;
    } 
    else 
    {
        TMR_ON_OFF = TMR_OFF;
        INT_ENA_BIT = 0;
        INT_FLAG = 0;
        //INT_ENA_GIE = 0;
    }
}// </editor-fold>
#endif //   WRITE_LCD_BY_INTERRUPT
//------------------------------------------------------------------------------

#ifdef	__cplusplus
}
#endif
