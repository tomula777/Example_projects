/* 
 * File:   I2C_Bus.h
 * Author: Dell
 *
 * Created on 18 maja 2024, 16:23
 */

#ifndef I2C_BUS_H
#define	I2C_BUS_H

#ifdef	__cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
//          THE DIRECTIVES                          ( Customize to Your display )
//------------------------------------------------------------------------------

#define _XTAL_FREQ                  16000000           // Frequency oscilator in Hz;
#define delay_us(x)                 __delay_us(x)      // Macro for user's compiler macro e.g. __delay_us(x)
#define delay_ms(x)                 __delay_ms(x)      // Macro for user's compiler macro e.g. __delay_ms(x)
    
//--------------------------------------------------------------
#define DELAY_LSPEED                                
#ifndef DELAY_LSPEED
    #define DELAY_HSPEED
    #define NUM_FLO_CYC             (1.25*(_XTAL_FREQ / 4000000.0))       
#else                               
    #define NUM_FLO_CYC             (5*(_XTAL_FREQ / 4000000.0))
#endif // DELAY_LSPEED  
#define INTEGER                     ((unsigned long int)(NUM_FLO_CYC))
#define DECIMAL                     ((unsigned long int)((1000*(NUM_FLO_CYC - (double) INTEGER))))
#define NUM_CYCLE                   (INTEGER + (DECIMAL%(DECIMAL - 1)))
#define DELAY_CYCLE(x)               _delay((x)*NUM_CYCLE)

//--------------------------------------------------------------        

#define SDA_LINE_DIR                TRISAbits.TRISA4   //*
#define SCL_LINE_DIR                TRISAbits.TRISA3   //*
#define SDA_OUT_I2C                 LATAbits.LA4       //*
#define SCL_OUT_I2C                 LATAbits.LA3       //*
#define SDA_IN_I2C                  PORTAbits.RA4      //*
#define SCL_IN_I2C                  PORTAbits.RA3      //*

#define LOW                         0
#define HIGH                        1
#define OUTPUT_SET                  0           // used with the data direction register to set MCU port register;
#define INPUT_SET                   1           // used with the data direction register to set MCU port register;     
#define WORD_ADDRESS_16_BIT  
   
    void Init_I2C(void);
    inline void Start_I2C(void);
    inline void Stop_I2C(void);
    void Write_Data_I2C(unsigned char,unsigned int, unsigned char*,unsigned char);
    void Read_Data_I2C(unsigned char,unsigned int, unsigned char*, unsigned char);
    void Set_Address_Slave(unsigned char);
    void Reset_Bus_I2C(void);
    inline void Check_Bus_I2C(void);
    inline void Check_SDA_I2C(void);
    inline void Check_ACK_I2C(void);
    inline void Set_ACK_I2C(unsigned char);
    inline void Period_SCL_I2C(unsigned int);
    void Error_I2C_Bus(unsigned char);
volatile union DATA_WRITE_I2C
{
    unsigned data_write     :8;
    struct
    {
        unsigned            :7;
        unsigned bit_write  :1;
    };
}I2C_write;

volatile union DATA_READ_I2C
{
    unsigned data_read      :8;
    unsigned bit_read       :1;
}I2C_read;




#ifdef	__cplusplus
}
#endif

#endif	/* I2C_BUS_H */

