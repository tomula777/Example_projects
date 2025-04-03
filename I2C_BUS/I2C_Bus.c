

#include<xc.h>

#include"I2C_Bus.h"
#ifdef	__cplusplus
extern "C" {
#endif


void Init_I2C(void)
{
    Check_Bus_I2C();
}

void Start_I2C(void)
{
   Check_Bus_I2C();
   SDA_OUT_I2C = LOW;
   DELAY_CYCLE(1);
   SCL_OUT_I2C = LOW;
   delay_us(1);
}

void Stop_I2C(void)
{
   DELAY_CYCLE(1);
   SDA_OUT_I2C = LOW;
   delay_us(1);
   SCL_OUT_I2C = HIGH;
   DELAY_CYCLE(1);
   SDA_OUT_I2C = HIGH;
}

void Check_Bus_I2C(void)
{
   unsigned char i=3;
   while(i--)
   {
        SDA_LINE_DIR = INPUT_SET;
        SCL_LINE_DIR = INPUT_SET;
        DELAY_CYCLE(3);
        if(!(SDA_IN_I2C && SCL_IN_I2C)) Reset_Bus_I2C();
        else 
        {
            SCL_OUT_I2C = HIGH;
            SDA_OUT_I2C = HIGH;
            SDA_LINE_DIR = OUTPUT_SET;
            SCL_LINE_DIR = OUTPUT_SET;
            return;
        }
   }
   Error_I2C_Bus(0);
}

void Reset_Bus_I2C(void)
{
    
}

void Write_Data_I2C(unsigned char device_addr, unsigned int word_addr, unsigned char* ptr_data, unsigned char num_data)
{
    I2C_write.data_write= device_addr | 0;     // write operation
    Start_I2C();
    unsigned char i;
    for(i=0;i<8;i++)
    {
       SDA_OUT_I2C = I2C_write.bit_write;
       Period_SCL_I2C(1);
       I2C_write.data_write<<=1;
    }
    Check_ACK_I2C();
#ifdef WORD_ADDRESS_16_BIT
    I2C_write.data_write = word_addr>>8;
    for(i=0;i<8;i++)
    {
       DELAY_CYCLE(1);
       SDA_OUT_I2C = I2C_write.bit_write;
       Period_SCL_I2C(1);
       I2C_write.data_write<<=1;
    }
    Check_ACK_I2C();
#endif // 16_BIT_WORD ADDRESS      
    I2C_write.data_write = word_addr;
    for(i=0;i<8;i++)
    {
       DELAY_CYCLE(1);
       SDA_OUT_I2C = I2C_write.bit_write;
       Period_SCL_I2C(1);
       I2C_write.data_write<<=1;
    }
    Check_ACK_I2C();
    while(num_data--)
    {
        for(i=0;i<8;i++)
       {
        DELAY_CYCLE(1);
        I2C_write.data_write = *ptr_data++;
        SDA_OUT_I2C = I2C_write.bit_write;
        Period_SCL_I2C(1);
       }
        Check_ACK_I2C();
    }
    Stop_I2C();
}

void Read_Data_I2C(unsigned char device_addr,unsigned int word_addr, unsigned char* ptr_data, unsigned char num_data)
{
    I2C_write.data_write = device_addr | 1;     // read operation
    Start_I2C();
    unsigned char i;
    for(i=0;i<8;i++)
    {
       SDA_OUT_I2C = I2C_write.bit_write;
       Period_SCL_I2C(1);
       I2C_write.data_write<<=1;
    }
    Check_ACK_I2C();
#ifdef WORD_ADDRESS_16_BIT
    I2C_write.data_write = word_addr>>8;
    for(i=0;i<8;i++)
    {
       DELAY_CYCLE(1);
       SDA_OUT_I2C = I2C_write.bit_write;
       Period_SCL_I2C(1);
       I2C_write.data_write<<=1;
    }
    Check_ACK_I2C();
#endif // 16_BIT_WORD ADDRESS   
    I2C_write.data_write = word_addr;
    for(i=0;i<8;i++)
    {
       DELAY_CYCLE(1);
       SDA_OUT_I2C = I2C_write.bit_write;
       Period_SCL_I2C(1);
       I2C_write.data_write<<=1;
    }
    Check_ACK_I2C();
    while(num_data--)
    {
        SDA_LINE_DIR = INPUT_SET;
        for(i=0;i<7;i++)
       {
        DELAY_CYCLE(1);
        I2C_read.bit_read = SDA_IN_I2C;
        Period_SCL_I2C(1);
        if(i==6)    *ptr_data = I2C_read.data_read;
        else        I2C_read.data_read<<=1;
       }
        if(num_data)    Set_ACK_I2C(0);
        else            Set_ACK_I2C(1);
    }
    Stop_I2C();
}

void Check_ACK_I2C()
{
    SDA_LINE_DIR = INPUT_SET;
    unsigned char i=3;
    while(SDA_IN_I2C)
    {
        DELAY_CYCLE(1);
        if(!i)  Error_I2C_Bus(1);
        i--;
    }
    Period_SCL_I2C(1);
    SDA_OUT_I2C = HIGH;
    SDA_LINE_DIR = OUTPUT_SET;
}

void Set_ACK_I2C(unsigned char ack)
{
    switch (ack)
    {
        case 0:
           SDA_OUT_I2C = LOW;
           break;
        case 1:
           SDA_OUT_I2C = HIGH;
    }  
    SDA_LINE_DIR = OUTPUT_SET;       
    Period_SCL_I2C(1);
}

void Error_I2C_Bus(unsigned char flag)
{
    switch (flag)
    {
       case 0:
            //Write_Data_DDRAM("ERROR I2C BUS",0,0);
            //Write_Data_DDRAM("shorted to GND",1,0); 
            break;
       case 1:
            //Write_Data_DDRAM("ERROR I2C BUS",0,0);   
            //Write_Data_DDRAM("NO ACK SLAVE",1,0);
            break; 
    }
    
    while(1);              
}

void Period_SCL_I2C(unsigned int i)
{
    while(i--)
    {
        DELAY_CYCLE(1);
        SCL_OUT_I2C = HIGH;
        delay_us(1);
        DELAY_CYCLE(1);
        SCL_OUT_I2C = LOW;
        delay_us(1); 
    }
}

void Check_SDA_I2C()
{
    unsigned char i=3;
   while(i--)
   {
        SDA_LINE_DIR = INPUT_SET;
        DELAY_CYCLE(3);
        if(!(SDA_IN_I2C)) Reset_Bus_I2C();
        else 
        {
            SDA_OUT_I2C = HIGH;
            SDA_LINE_DIR = OUTPUT_SET;
            return;
        }
   }
   Error_I2C_Bus(0);
}

#ifdef	__cplusplus
}
#endif