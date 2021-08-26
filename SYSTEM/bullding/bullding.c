#include "bullding.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "string.h"
#include "rtc.h"
#include "iwdg.h"
#include "SHT2x.h"              //��ʪ�ȿ�

TaskHandle_t StartTask_Handler;         //��ʼ����������
//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
    
     POINT_COLOR=BLACK;
    LCD_DrawLine(0,25,480,25);	//����
    LCD_ShowString(10,40,200,24,24,"temp:");
    LCD_ShowString(10,80,200,24,24,"humi:");
    LCD_ShowChar(38,5,'.',16,1);
    LCD_ShowChar(62,5,'.',16,1);
    LCD_ShowChar(190,5,'.',16,1);
    LCD_ShowChar(215,5,'.',16,1);
    
    //����ʱ���ӡ��������
    xTaskCreate((TaskFunction_t )output_timr,     	
                (const char*    )"output_timr",   	
                (uint16_t       )OUTPUT_TIMR_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )OUTPUT_TIMR_PRIO,
                (TaskHandle_t*  )&Output_Timr);
      
    //����ʱ���ӡ��������
    xTaskCreate((TaskFunction_t )set_timr,     	
                (const char*    )"set_timr",   	
                (uint16_t       )SET_TIMR_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )SET_TIMR_PRIO,
                (TaskHandle_t*  )&Set_Timr);      

     //����LED0����
    xTaskCreate((TaskFunction_t )led0_task,     	
                (const char*    )"led0_task",   	
                (uint16_t       )LED0_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LED0_TASK_PRIO,	
                (TaskHandle_t*  )&LED0Task_Handler);
                
    //����QUERY����          
     xTaskCreate((TaskFunction_t )query_task,     
                (const char*    )"query_task",   
                (uint16_t       )QUERY_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )QUERY_TASK_PRIO,
                (TaskHandle_t*  )&QueryTask_Handler);
    //����ι������          
     xTaskCreate((TaskFunction_t )feed_dog,     
                (const char*    )"feed_dog",   
                (uint16_t       )FEED_DOG_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )FEED_DOG_TASK_PRIO,
                (TaskHandle_t*  )&FEED_DOG);                
   
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}




TaskHandle_t Output_Timr;//������

int humiduty;//ʪ��
int temperature;//�¶�

//��ӡ����
unsigned char temp[10];
void output_timr(void *pvParameters)
{
   while(1)
    {
        taskENTER_CRITICAL();           //�����ٽ���
        Get_TempHumi(temp);//��ȡ��ʪ�ȴ�������ֵ
         temp[0] = 0x01;//д��Ϊ���¶�
        
         temperature = (temp[1] << 16) + (temp[2] << 8) + temp[3];
         temperature = temperature / 100;//ת���¶���ֵ
         if(temp[0] == 0)
         {
          temperature = -temperature;
         } 
         humiduty = (temp[4] << 8) + temp[5]; 
         humiduty = humiduty / 100;//ת��ʪ����ֵ
         
         LCD_ShowNum(70,40,temperature,2,24);//��ӡ�¶�ֵ
         LCD_ShowNum(70,80,humiduty,2,24);//��ӡʪ��ֵ
         
        taskEXIT_CRITICAL();            //�˳��ٽ���
        vTaskDelay(2000);
    }
}


TaskHandle_t Set_Timr;

//��ѯʱ�亯��
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
			LCD_ShowNum(65,5,calendar.w_date,2,16);//��ӡ������	 
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
       // vTaskDelete(NULL); //ɾ����ʼ����
    }
}

TaskHandle_t LED0Task_Handler;//������

//LED1������  

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


TaskHandle_t QueryTask_Handler;//������
char InfoBuffer[1000];				//������Ϣ������

//query������
void query_task(void *pvParameters)
{
	u32 TotalRunTime;
	UBaseType_t ArraySize,x;
	TaskStatus_t *StatusArray;
	
	//��һ��������uxTaskGetSystemState()��ʹ��
	printf("/********��һ��������uxTaskGetSystemState()��ʹ��**********/\r\n");
	ArraySize=uxTaskGetNumberOfTasks();		//��ȡϵͳ��������
	StatusArray=pvPortMalloc(ArraySize*sizeof(TaskStatus_t));//�����ڴ�
	if(StatusArray!=NULL)					//�ڴ�����ɹ�
	{
		ArraySize=uxTaskGetSystemState((TaskStatus_t* 	)StatusArray, 	//������Ϣ�洢����
									   (UBaseType_t		)ArraySize, 	//������Ϣ�洢�����С
								       (uint32_t*		)&TotalRunTime);//����ϵͳ�ܵ�����ʱ��
		printf("TaskName\t\tPriority\t\tTaskNumber\t\t\r\n");
		for(x=0;x<ArraySize;x++)
		{
			//ͨ�����ڴ�ӡ����ȡ����ϵͳ������й���Ϣ�������������ơ�
			//�������ȼ��������š�
			printf("%s\t\t%d\t\t\t%d\t\t\t\r\n",				
					StatusArray[x].pcTaskName,
					(int)StatusArray[x].uxCurrentPriority,
					(int)StatusArray[x].xTaskNumber);
			
		}
	}
	vPortFree(StatusArray);	//�ͷ��ڴ�
	printf("/**************************����***************************/\r\n");
	printf("����KEY_UP������!\r\n\r\n\r\n");
	//while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);		//�ȴ�KEY_UP������
	
	//�ڶ���������vTaskGetInfo()��ʹ��
	TaskHandle_t TaskHandle;	
	TaskStatus_t TaskStatus;
	
	printf("/************�ڶ���������vTaskGetInfo()��ʹ��**************/\r\n");
	TaskHandle=xTaskGetHandle("led0_task");			//������������ȡ��������
	//��ȡLED0_Task��������Ϣ
	vTaskGetInfo((TaskHandle_t	)TaskHandle, 		//������
				 (TaskStatus_t*	)&TaskStatus, 		//������Ϣ�ṹ��
				 (BaseType_t	)pdTRUE,			//����ͳ�������ջ��ʷ��Сʣ���С
			     (eTaskState	)eInvalid);			//�����Լ���ȡ��������׳̬
	//ͨ�����ڴ�ӡ��ָ��������й���Ϣ��
	printf("������:                %s\r\n",TaskStatus.pcTaskName);
	printf("������:              %d\r\n",(int)TaskStatus.xTaskNumber);
	printf("����׳̬:              %d\r\n",TaskStatus.eCurrentState);
	printf("����ǰ���ȼ�:        %d\r\n",(int)TaskStatus.uxCurrentPriority);
	printf("��������ȼ�:          %d\r\n",(int)TaskStatus.uxBasePriority);
	printf("�����ջ����ַ:        %#x\r\n",(int)TaskStatus.pxStackBase);
	printf("�����ջ��ʷʣ����Сֵ:%d\r\n",TaskStatus.usStackHighWaterMark);
	printf("/**************************����***************************/\r\n");
	printf("����KEY_UP������!\r\n\r\n\r\n");
	//while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);		//�ȴ�KEY_UP������
	
	//������������eTaskGetState()��ʹ��	
	eTaskState TaskState;
	char TaskInfo[10];
	printf("/***********������������eTaskGetState()��ʹ��*************/\r\n");
	TaskHandle=xTaskGetHandle("query_task");		//������������ȡ��������
	TaskState=eTaskGetState(TaskHandle);			//��ȡquery_task���������׳̬
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
	printf("����׳ֵ̬:%d,��Ӧ��׳̬Ϊ:%s\r\n",TaskState,TaskInfo);
	printf("/**************************����**************************/\r\n");
	printf("����KEY_UP������!\r\n\r\n\r\n");
	//while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);		//�ȴ�KEY_UP������
	
	//���Ĳ�������vTaskList()��ʹ��	
	printf("/*************������������vTaskList()��ʹ��*************/\r\n");
	vTaskList(InfoBuffer);							//��ȡ�����������Ϣ
	printf("%s\r\n",InfoBuffer);					//ͨ�����ڴ�ӡ�����������Ϣ
	printf("/**************************����**************************/\r\n");
	while(1)
	{
		LED1=~LED1;
        vTaskDelay(1000);                           //��ʱ1s��Ҳ����1000��ʱ�ӽ���	
	}
}


TaskHandle_t FEED_DOG;//ι������������


//ι��   500msι��һ��
void feed_dog(void *pvParameters)
{
    while(1)
    {
        IWDG_Feed();
         vTaskDelay(800);
    }
}


