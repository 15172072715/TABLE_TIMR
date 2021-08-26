#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "include_define.h"      
#include "lcd.h"                //��ʾ����
#include "bullding.h"           //�������ܺ�����
#include "rtc.h"
#include "iwdg.h"
#include "SHT2x.h"              //��ʪ�ȿ�



int main(void)
{
    	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	 
	delay_init();	    				//��ʱ������ʼ��	  
	uart_init(115200);					//��ʼ������
	LED_Init();		  					//��ʼ��LED
    LCD_Init();
    IWDG_Init(5,625);       //���Ƶ��Ϊ128,����ֵΪ625,���ʱ��Ϊ2s
    SHT2x_Init();           //��ʪ�ȴ�������ʼ��
    while(RTC_Init())		//RTC��ʼ��	��һ��Ҫ��ʼ���ɹ�
	{ 
		LCD_ShowString(60,130,200,16,16,"RTC ERROR!   ");	
		delay_ms(800);
		LCD_ShowString(60,130,200,16,16,"RTC Trying...");	
	}		
  
	//������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
}



