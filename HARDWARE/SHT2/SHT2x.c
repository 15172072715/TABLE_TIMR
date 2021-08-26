/** 
 * @file     SHT2x.c
 * @brief    温湿度传感器
 * @details  
 * @author   华兄
 * @email    591881218@qq.com
 * @date     2015
 * @version  vX.XX
 * @par Copyright (c):  
 *           华兄电子
 * @par History:          
 *   version: author, date, desc\n 
 */
#include "stm32f10x_gpio.h"
#include "SHT2x.h"
#include "delay.h"

#include "string.h"


long Get_TempHumi(unsigned char *outbuff);
void save_result_TempHumi(unsigned char SN_num_sub,unsigned char *inbuff);
void SHT2x_Delay(u32 n)
{
    delay_us(n*2);
}

void SHT2x_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;


    GPIO_InitStructure.GPIO_Pin =  SHT2x_SCL_PIN | SHT2x_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SHT2x_I2C_PORT, &GPIO_InitStructure);

    SHT2x_SCL_HIGH();
    SHT2x_SDA_HIGH();

    SHT2x_Delay(80);

    SHT2x_SoftReset();
}

void SHT2x_SCL_OUTPUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;


    GPIO_InitStructure.GPIO_Pin =  SHT2x_SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SHT2x_I2C_PORT, &GPIO_InitStructure);
}

void SHT2x_SCL_INPUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;


    GPIO_InitStructure.GPIO_Pin =  SHT2x_SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING;
    GPIO_Init(SHT2x_I2C_PORT, &GPIO_InitStructure);
}

void SHT2x_SDA_OUTPUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;


    GPIO_InitStructure.GPIO_Pin =  SHT2x_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SHT2x_I2C_PORT, &GPIO_InitStructure);
}

void SHT2x_SDA_INPUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;


    GPIO_InitStructure.GPIO_Pin =  SHT2x_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING;
    GPIO_Init(SHT2x_I2C_PORT, &GPIO_InitStructure);
}

void SHT2x_I2cStartCondition(void)
{
    SHT2x_SCL_OUTPUT();
    SHT2x_SDA_OUTPUT();
    
    SHT2x_SDA_HIGH();
    SHT2x_SCL_HIGH();
    SHT2x_SDA_LOW();
    SHT2x_Delay(30);
    SHT2x_SCL_LOW();
    SHT2x_Delay(30);
}

void SHT2x_I2cStopCondition(void)
{
    SHT2x_SCL_OUTPUT();
    SHT2x_SDA_OUTPUT();
    
    SHT2x_SDA_LOW();
    SHT2x_SCL_LOW();
    SHT2x_SCL_HIGH();
    SHT2x_Delay(30);
    SHT2x_SDA_HIGH();
    SHT2x_Delay(30);
}

void SHT2x_I2cAcknowledge(void)
{
    SHT2x_SCL_OUTPUT();
    SHT2x_SDA_OUTPUT();

    SHT2x_SDA_LOW();
    
    SHT2x_SCL_HIGH();
    SHT2x_Delay(10);//SHT2x_Delay(80);
    SHT2x_SCL_LOW();   
    SHT2x_Delay(10);//SHT2x_Delay(80);
}

void SHT2x_I2cNoAcknowledge(void)
{
    SHT2x_SCL_OUTPUT();
    SHT2x_SDA_OUTPUT();

    SHT2x_SDA_HIGH();
    
    SHT2x_SCL_HIGH();
    SHT2x_Delay(10);//SHT2x_Delay(80);
    SHT2x_SCL_LOW();   
    SHT2x_Delay(10);//SHT2x_Delay(80);
}

u8 SHT2x_I2cReadByte(void)
{
    u8 i, val = 0;


    SHT2x_SCL_OUTPUT();
    SHT2x_SDA_OUTPUT();

    SHT2x_SDA_HIGH();
    
    SHT2x_SDA_INPUT();
        
    for(i = 0; i < 8; i++)
    {
        val <<= 1; 
        
        SHT2x_SCL_HIGH();   
        
        if(Bit_SET == SHT2x_SDA_STATE()) 
        {
            val |= 0x01;
        }
        
        SHT2x_SCL_LOW();    
    }

    SHT2x_SDA_OUTPUT();

    SHT2x_SDA_HIGH();
    
    return (val);
}

u8 SHT2x_I2cWriteByte(u8 byte)
{
    u8 i, ack;


    SHT2x_SCL_OUTPUT();
    SHT2x_SDA_OUTPUT();
    
    for(i = 0; i < 8; i++)
    {
        if(byte & 0x80) 
        {
            SHT2x_SDA_HIGH();
        }
        else 
        {
            SHT2x_SDA_LOW();
        }
        
        SHT2x_SCL_HIGH();
        SHT2x_Delay(10);//SHT2x_Delay(80);
        SHT2x_SCL_LOW();   
        SHT2x_Delay(10);//SHT2x_Delay(80);
        
        byte <<= 1;
    }

    SHT2x_SDA_INPUT();
    
    SHT2x_SCL_HIGH();
    
    if(Bit_SET == SHT2x_SDA_STATE()) 
    {
        ack = ACK_ERROR;
    }
    else
    {
        ack = ACK_OK;
    }
    
    SHT2x_SCL_LOW();  

    SHT2x_SDA_OUTPUT();

    SHT2x_SDA_HIGH();
    
    return (ack);
}

float SHT2x_MeasureTempHM(void)
{
    float TEMP;
    u8 tmp1, tmp2;
    u16 ST;
    

    SHT2x_SCL_OUTPUT();
    
    SHT2x_I2cStartCondition();                            
    SHT2x_I2cWriteByte(I2C_ADR_W);
    SHT2x_I2cWriteByte(TRIG_TEMP_MEASUREMENT_HM);

    SHT2x_I2cStartCondition();
    SHT2x_I2cWriteByte(I2C_ADR_R);

    SHT2x_SCL_HIGH();

    SHT2x_SCL_INPUT();

    while(Bit_RESET == SHT2x_SCL_STATE())
    {
        SHT2x_Delay(20);
    }
    
    tmp1 = SHT2x_I2cReadByte();
    SHT2x_I2cAcknowledge();
    tmp2 = SHT2x_I2cReadByte();
    SHT2x_I2cNoAcknowledge();
    SHT2x_I2cStopCondition();
    
    ST = (tmp1 << 8) | (tmp2 << 0);
    ST &= ~0x0003;
    TEMP = ((float)ST * 0.00268127) - 46.85;

    SHT2x_SCL_OUTPUT();

    return (TEMP);	  
}

float SHT2x_MeasureHumiHM(void)
{
    float HUMI;
    u8 tmp1, tmp2;    
    u16 SRH;


    SHT2x_SCL_OUTPUT();
    
    SHT2x_I2cStartCondition();                               
    SHT2x_I2cWriteByte(I2C_ADR_W);
    SHT2x_I2cWriteByte(TRIG_HUMI_MEASUREMENT_HM);

    SHT2x_I2cStartCondition();
    SHT2x_I2cWriteByte(I2C_ADR_R);

    SHT2x_SCL_HIGH();

    SHT2x_SCL_INPUT();

    while(Bit_RESET == SHT2x_SCL_STATE())
    {
        SHT2x_Delay(20);
    }
    
    tmp1 = SHT2x_I2cReadByte();
    SHT2x_I2cAcknowledge();
    tmp2 = SHT2x_I2cReadByte();
    SHT2x_I2cNoAcknowledge();
    SHT2x_I2cStopCondition();
    
    SRH = (tmp1 << 8) | (tmp2 << 0);
    SRH &= ~0x0003;
    HUMI = ((float)SRH * 0.00190735) - 6;

    SHT2x_SCL_OUTPUT();

    return (HUMI);
}

float SHT2x_MeasureTempPoll(void)
{
    float TEMP;
    u8 ack, tmp1, tmp2;
    u16 ST;
    
    
    SHT2x_I2cStartCondition();                            
    SHT2x_I2cWriteByte(I2C_ADR_W);
    SHT2x_I2cWriteByte(TRIG_TEMP_MEASUREMENT_POLL);

    do {
        delay_ms(10);               
        SHT2x_I2cStartCondition();
        ack = SHT2x_I2cWriteByte(I2C_ADR_R);
    } while(ACK_ERROR == ack);
    
    tmp1 = SHT2x_I2cReadByte();
    SHT2x_I2cAcknowledge();
    tmp2 = SHT2x_I2cReadByte();
    SHT2x_I2cNoAcknowledge();
    SHT2x_I2cStopCondition();
    
    ST = (tmp1 << 8) | (tmp2 << 0);
    ST &= ~0x0003;
    TEMP = ((float)ST * 0.00268127) - 46.85;

    return (TEMP);	  
}

float SHT2x_MeasureHumiPoll(void)
{
    float HUMI;
    u8 ack, tmp1, tmp2;    
    u16 SRH;

    
    SHT2x_I2cStartCondition();                               
    SHT2x_I2cWriteByte(I2C_ADR_W);
    SHT2x_I2cWriteByte(TRIG_HUMI_MEASUREMENT_POLL);
    
    do {
        delay_ms(10);
        SHT2x_I2cStartCondition();
        ack = SHT2x_I2cWriteByte(I2C_ADR_R);
    } while(ACK_ERROR == ack);
    
    tmp1 = SHT2x_I2cReadByte();
    SHT2x_I2cAcknowledge();
    tmp2 = SHT2x_I2cReadByte();
    SHT2x_I2cNoAcknowledge();
    SHT2x_I2cStopCondition();
    
    SRH = (tmp1 << 8) | (tmp2 << 0);
    SRH &= ~0x0003;
    HUMI = ((float)SRH * 0.00190735) - 6;

    return (HUMI);
}


u8 SHT2x_ReadUserReg(void)    
{
    u8 reg;

    
    SHT2x_I2cStartCondition();                 
    SHT2x_I2cWriteByte(I2C_ADR_W);
    SHT2x_I2cWriteByte(USER_REG_R);
    SHT2x_I2cStartCondition(); 
    SHT2x_I2cWriteByte(I2C_ADR_R);
    reg = SHT2x_I2cReadByte();
    SHT2x_I2cNoAcknowledge();
    SHT2x_I2cStopCondition();
    
    return (reg); 
}

u8 SHT2x_WriteUserReg(u8 reg)
{
    u8 ack;

    
    SHT2x_I2cStartCondition();
    SHT2x_I2cWriteByte(I2C_ADR_W);
    SHT2x_I2cWriteByte(USER_REG_W);
    ack = SHT2x_I2cWriteByte(reg);
    SHT2x_I2cStopCondition();  
    
    return (ack);
}

void SHT2x_SoftReset(void)
{
    SHT2x_I2cStartCondition();
    SHT2x_I2cWriteByte(I2C_ADR_W);
    SHT2x_I2cWriteByte(SOFT_RESET);
    SHT2x_I2cStopCondition();

    SHT2x_Delay(80);
}

void SHT2x_GetSerialNumber(u8 *buf)
{
    /* Read from memory location 1 */
    SHT2x_I2cStartCondition();
    SHT2x_I2cWriteByte(I2C_ADR_W); //I2C address
    SHT2x_I2cWriteByte(0xFA); //Command for readout on-chip memory
    SHT2x_I2cWriteByte(0x0F); //on-chip memory address
    SHT2x_I2cStartCondition();
    SHT2x_I2cWriteByte(I2C_ADR_R); //I2C address
    buf[5] = SHT2x_I2cReadByte(); //Read SNB_3
    SHT2x_I2cAcknowledge();
    SHT2x_I2cReadByte(); //Read CRC SNB_3 (CRC is not analyzed)
    SHT2x_I2cAcknowledge();
    buf[4] = SHT2x_I2cReadByte(); //Read SNB_2
    SHT2x_I2cAcknowledge();
    SHT2x_I2cReadByte(); //Read CRC SNB_2 (CRC is not analyzed)
    SHT2x_I2cAcknowledge();
    buf[3] = SHT2x_I2cReadByte(); //Read SNB_1
    SHT2x_I2cAcknowledge();
    SHT2x_I2cReadByte(); //Read CRC SNB_1 (CRC is not analyzed)
    SHT2x_I2cAcknowledge();
    buf[2] = SHT2x_I2cReadByte(); //Read SNB_0
    SHT2x_I2cAcknowledge();
    SHT2x_I2cReadByte(); //Read CRC SNB_0 (CRC is not analyzed)
    SHT2x_I2cNoAcknowledge();
    SHT2x_I2cStopCondition();

    /* Read from memory location 2 */
    SHT2x_I2cStartCondition();
    SHT2x_I2cWriteByte(I2C_ADR_W); //I2C address
    SHT2x_I2cWriteByte(0xFC); //Command for readout on-chip memory
    SHT2x_I2cWriteByte(0xC9); //on-chip memory address
    SHT2x_I2cStartCondition();
    SHT2x_I2cWriteByte(I2C_ADR_R); //I2C address
    buf[1] = SHT2x_I2cReadByte(); //Read SNC_1
    SHT2x_I2cAcknowledge();
    buf[0] = SHT2x_I2cReadByte(); //Read SNC_0
    SHT2x_I2cAcknowledge();
    SHT2x_I2cReadByte(); //Read CRC SNC0/1 (CRC is not analyzed)
    SHT2x_I2cAcknowledge();
    buf[7] = SHT2x_I2cReadByte(); //Read SNA_1
    SHT2x_I2cAcknowledge();
    buf[6] = SHT2x_I2cReadByte(); //Read SNA_0
    SHT2x_I2cAcknowledge();
    SHT2x_I2cReadByte(); //Read CRC SNA0/1 (CRC is not analyzed)
    SHT2x_I2cNoAcknowledge();
    SHT2x_I2cStopCondition();
}

void SHT2x_Test(void)
{   
    unsigned char buff[32]={0};
    //g_sht2x_param.TEMP_HM = SHT2x_MeasureTempHM();
    //g_sht2x_param.HUMI_HM = SHT2x_MeasureHumiHM();

    //g_sht2x_param.TEMP_POLL = SHT2x_MeasureTempPoll();
    //g_sht2x_param.HUMI_POLL = SHT2x_MeasureHumiPoll();
    
    SHT2x_GetSerialNumber(buff);
}







long Get_TempHumi(unsigned char *outbuff)
{
	static unsigned char buff[10];    
    float wendu,wendu1;
    float shidu;
    unsigned long temp_long; 
    wendu = SHT2x_MeasureTempPoll();
    shidu = SHT2x_MeasureHumiPoll();
	if(wendu <= 0) wendu = SHT2x_MeasureTempPoll();
	if(shidu <= 0) shidu = SHT2x_MeasureHumiPoll();
    if(wendu < -20) wendu = -20;
    if(wendu > 100) wendu = 100;
	wendu = wendu - 1.5;
    if(wendu >= 0)
	{
        wendu1 = wendu;
	}
    else
	{
        wendu1 = -wendu;
	}
    temp_long = wendu1 * 100; 
	if((temp_long % 10)!=0)//7669
	{
		temp_long=((temp_long / 10)+1) * 10;
	}    
    memset(buff,0,sizeof(buff));      
    if(wendu >= 0)    
        buff[0] = 1; 
    else     
        buff[0] = 0;    
	buff[0] = 1;//写死为正温度
	buff[1] = (temp_long & 0xff0000) >> 16;
	buff[2] = (temp_long & 0x00ff00) >> 8;
	buff[3] = (temp_long & 0x0000ff);
	
    if(shidu < 0)   shidu = 0;
    if(shidu > 100) shidu = 100; 
    temp_long = shidu * 100; 
	temp_long = temp_long / 10;
	temp_long = temp_long * 10;
    buff[4] = (temp_long & 0xff00) >> 8;
	buff[5] = (temp_long & 0xff);
	
	memcpy(outbuff,buff,6);	
	SHT2x_SCL_HIGH();
    SHT2x_SDA_HIGH();
// 	SHT2x_I2cStopCondition();
	return 0;
}


