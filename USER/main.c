#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "include_define.h"      
#include "lcd.h"                //显示屏库
#include "bullding.h"           //基础功能函数库
#include "rtc.h"
#include "iwdg.h"
#include "SHT2x.h"              //温湿度库



int main(void)
{
    	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 
	delay_init();	    				//延时函数初始化	  
	uart_init(115200);					//初始化串口
	LED_Init();		  					//初始化LED
    LCD_Init();
    IWDG_Init(5,625);       //与分频数为128,重载值为625,溢出时间为2s
    SHT2x_Init();           //温湿度传感器初始化
    while(RTC_Init())		//RTC初始化	，一定要初始化成功
	{ 
		LCD_ShowString(60,130,200,16,16,"RTC ERROR!   ");	
		delay_ms(800);
		LCD_ShowString(60,130,200,16,16,"RTC Trying...");	
	}		
  
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}



