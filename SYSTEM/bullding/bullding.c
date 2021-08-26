#include "bullding.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "string.h"
#include "rtc.h"
#include "iwdg.h"
#include "SHT2x.h"              //温湿度库

TaskHandle_t StartTask_Handler;         //开始函数任务句柄
//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    
     POINT_COLOR=BLACK;
    LCD_DrawLine(0,25,480,25);	//画线
    LCD_ShowString(10,40,200,24,24,"temp:");
    LCD_ShowString(10,80,200,24,24,"humi:");
    LCD_ShowChar(38,5,'.',16,1);
    LCD_ShowChar(62,5,'.',16,1);
    LCD_ShowChar(190,5,'.',16,1);
    LCD_ShowChar(215,5,'.',16,1);
    
    //创建时间打印函数任务
    xTaskCreate((TaskFunction_t )output_timr,     	
                (const char*    )"output_timr",   	
                (uint16_t       )OUTPUT_TIMR_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )OUTPUT_TIMR_PRIO,
                (TaskHandle_t*  )&Output_Timr);
      
    //创建时间打印函数任务
    xTaskCreate((TaskFunction_t )set_timr,     	
                (const char*    )"set_timr",   	
                (uint16_t       )SET_TIMR_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )SET_TIMR_PRIO,
                (TaskHandle_t*  )&Set_Timr);      

     //创建LED0任务
    xTaskCreate((TaskFunction_t )led0_task,     	
                (const char*    )"led0_task",   	
                (uint16_t       )LED0_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LED0_TASK_PRIO,	
                (TaskHandle_t*  )&LED0Task_Handler);
                
    //创建QUERY任务          
     xTaskCreate((TaskFunction_t )query_task,     
                (const char*    )"query_task",   
                (uint16_t       )QUERY_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )QUERY_TASK_PRIO,
                (TaskHandle_t*  )&QueryTask_Handler);
    //创建喂狗任务          
     xTaskCreate((TaskFunction_t )feed_dog,     
                (const char*    )"feed_dog",   
                (uint16_t       )FEED_DOG_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )FEED_DOG_TASK_PRIO,
                (TaskHandle_t*  )&FEED_DOG);                
   
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}




TaskHandle_t Output_Timr;//任务句柄

int humiduty;//湿度
int temperature;//温度

//打印函数
unsigned char temp[10];
void output_timr(void *pvParameters)
{
   while(1)
    {
        taskENTER_CRITICAL();           //进入临界区
        Get_TempHumi(temp);//获取温湿度传感器数值
         temp[0] = 0x01;//写死为正温度
        
         temperature = (temp[1] << 16) + (temp[2] << 8) + temp[3];
         temperature = temperature / 100;//转换温度数值
         if(temp[0] == 0)
         {
          temperature = -temperature;
         } 
         humiduty = (temp[4] << 8) + temp[5]; 
         humiduty = humiduty / 100;//转换湿度数值
         
         LCD_ShowNum(70,40,temperature,2,24);//打印温度值
         LCD_ShowNum(70,80,humiduty,2,24);//打印湿度值
         
        taskEXIT_CRITICAL();            //退出临界区
        vTaskDelay(2000);
    }
}


TaskHandle_t Set_Timr;

//查询时间函数
void set_timr(void *pvParameters)
{
    u8 t;
    
    while(1)
    {
        if(t!=calendar.sec)
		{
			t=calendar.sec;
			LCD_ShowNum(5,5,calendar.w_year,4,16);									  
			LCD_ShowNum(45,5,calendar.w_month,2,16);									  
			LCD_ShowNum(65,5,calendar.w_date,2,16);//打印年月日	 
			switch(calendar.week)
			{
				case 0:
					LCD_ShowString(90,5,200,16,16,"Sunday   ");
					break;
				case 1:
					LCD_ShowString(90,5,200,16,16,"Monday   ");
					break;
				case 2:
					LCD_ShowString(90,5,200,16,16,"Tuesday  ");
					break;
				case 3:
					LCD_ShowString(90,5,200,16,16,"Wednesday");
					break;
				case 4:
					LCD_ShowString(90,5,200,16,16,"Thursday ");
					break;
				case 5:
					LCD_ShowString(90,5,200,16,16,"Friday   ");
					break;
				case 6:
					LCD_ShowString(90,5,200,16,16,"Saturday ");
					break;  
			}
			LCD_ShowNum(170,5,calendar.hour,2,16);									  
			LCD_ShowNum(195,5,calendar.min,2,16);									  
			LCD_ShowNum(220,5,calendar.sec,2,16);
            
		}	
		 vTaskDelay(500);
       // vTaskDelete(NULL); //删除开始任务
    }
}

TaskHandle_t LED0Task_Handler;//任务句柄

//LED1任务函数  

void led0_task(void *pvParameters)
{
    while(1)
    {
        LED0=0;
        vTaskDelay(500);
        LED0=1;
        vTaskDelay(500);
        
    }
}


TaskHandle_t QueryTask_Handler;//任务句柄
char InfoBuffer[1000];				//保存信息的数组

//query任务函数
void query_task(void *pvParameters)
{
	u32 TotalRunTime;
	UBaseType_t ArraySize,x;
	TaskStatus_t *StatusArray;
	
	//第一步：函数uxTaskGetSystemState()的使用
	printf("/********第一步：函数uxTaskGetSystemState()的使用**********/\r\n");
	ArraySize=uxTaskGetNumberOfTasks();		//获取系统任务数量
	StatusArray=pvPortMalloc(ArraySize*sizeof(TaskStatus_t));//申请内存
	if(StatusArray!=NULL)					//内存申请成功
	{
		ArraySize=uxTaskGetSystemState((TaskStatus_t* 	)StatusArray, 	//任务信息存储数组
									   (UBaseType_t		)ArraySize, 	//任务信息存储数组大小
								       (uint32_t*		)&TotalRunTime);//保存系统总的运行时间
		printf("TaskName\t\tPriority\t\tTaskNumber\t\t\r\n");
		for(x=0;x<ArraySize;x++)
		{
			//通过串口打印出获取到的系统任务的有关信息，比如任务名称、
			//任务优先级和任务编号。
			printf("%s\t\t%d\t\t\t%d\t\t\t\r\n",				
					StatusArray[x].pcTaskName,
					(int)StatusArray[x].uxCurrentPriority,
					(int)StatusArray[x].xTaskNumber);
			
		}
	}
	vPortFree(StatusArray);	//释放内存
	printf("/**************************结束***************************/\r\n");
	printf("按下KEY_UP键继续!\r\n\r\n\r\n");
	//while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);		//等待KEY_UP键按下
	
	//第二步：函数vTaskGetInfo()的使用
	TaskHandle_t TaskHandle;	
	TaskStatus_t TaskStatus;
	
	printf("/************第二步：函数vTaskGetInfo()的使用**************/\r\n");
	TaskHandle=xTaskGetHandle("led0_task");			//根据任务名获取任务句柄。
	//获取LED0_Task的任务信息
	vTaskGetInfo((TaskHandle_t	)TaskHandle, 		//任务句柄
				 (TaskStatus_t*	)&TaskStatus, 		//任务信息结构体
				 (BaseType_t	)pdTRUE,			//允许统计任务堆栈历史最小剩余大小
			     (eTaskState	)eInvalid);			//函数自己获取任务运行壮态
	//通过串口打印出指定任务的有关信息。
	printf("任务名:                %s\r\n",TaskStatus.pcTaskName);
	printf("任务编号:              %d\r\n",(int)TaskStatus.xTaskNumber);
	printf("任务壮态:              %d\r\n",TaskStatus.eCurrentState);
	printf("任务当前优先级:        %d\r\n",(int)TaskStatus.uxCurrentPriority);
	printf("任务基优先级:          %d\r\n",(int)TaskStatus.uxBasePriority);
	printf("任务堆栈基地址:        %#x\r\n",(int)TaskStatus.pxStackBase);
	printf("任务堆栈历史剩余最小值:%d\r\n",TaskStatus.usStackHighWaterMark);
	printf("/**************************结束***************************/\r\n");
	printf("按下KEY_UP键继续!\r\n\r\n\r\n");
	//while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);		//等待KEY_UP键按下
	
	//第三步：函数eTaskGetState()的使用	
	eTaskState TaskState;
	char TaskInfo[10];
	printf("/***********第三步：函数eTaskGetState()的使用*************/\r\n");
	TaskHandle=xTaskGetHandle("query_task");		//根据任务名获取任务句柄。
	TaskState=eTaskGetState(TaskHandle);			//获取query_task任务的任务壮态
	memset(TaskInfo,0,10);						
	switch((int)TaskState)
	{
		case 0:
			sprintf(TaskInfo,"Running");
			break;
		case 1:
			sprintf(TaskInfo,"Ready");
			break;
		case 2:
			sprintf(TaskInfo,"Suspend");
			break;
		case 3:
			sprintf(TaskInfo,"Delete");
			break;
		case 4:
			sprintf(TaskInfo,"Invalid");
			break;
	}
	printf("任务壮态值:%d,对应的壮态为:%s\r\n",TaskState,TaskInfo);
	printf("/**************************结束**************************/\r\n");
	printf("按下KEY_UP键继续!\r\n\r\n\r\n");
	//while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);		//等待KEY_UP键按下
	
	//第四步：函数vTaskList()的使用	
	printf("/*************第三步：函数vTaskList()的使用*************/\r\n");
	vTaskList(InfoBuffer);							//获取所有任务的信息
	printf("%s\r\n",InfoBuffer);					//通过串口打印所有任务的信息
	printf("/**************************结束**************************/\r\n");
	while(1)
	{
		LED1=~LED1;
        vTaskDelay(1000);                           //延时1s，也就是1000个时钟节拍	
	}
}


TaskHandle_t FEED_DOG;//喂狗函数任务句柄


//喂狗   500ms喂狗一次
void feed_dog(void *pvParameters)
{
    while(1)
    {
        IWDG_Feed();
         vTaskDelay(800);
    }
}


