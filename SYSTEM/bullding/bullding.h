#ifndef __BULLDING_H
#define __BULLDING_H

//�������ܺ�����
#include "sys.h"
#include "FreeRTOS.h"
#include "task.h"

extern unsigned char temp[10];

extern  TaskHandle_t StartTask_Handler;   //��ʼ����������
#define  START_TASK_PRIO		1          //�������ȼ�
#define  START_STK_SIZE 		60         //�����ջ��С	


extern  TaskHandle_t Output_Timr;         //��ӡ����������
#define  OUTPUT_TIMR_PRIO		1          //�������ȼ�
#define  OUTPUT_TIMR_SIZE 		40         //�����ջ��С	


extern TaskHandle_t Set_Timr;             //��ӡ����������
#define  SET_TIMR_PRIO		2              //�������ȼ�
#define  SET_TIMR_SIZE 		60             //�����ջ��С



extern  TaskHandle_t LED0Task_Handler;    //LED0��˸������
#define LED0_TASK_PRIO		3              //�������ȼ�
#define LED0_STK_SIZE 		40             //�����ջ��С	  



extern  TaskHandle_t QueryTask_Handler;   //��ѯ������Ϣ����������
#define QUERY_TASK_PRIO		4              //�������ȼ�
#define QUERY_STK_SIZE 		256            //�����ջ��С	
extern char InfoBuffer[1000];			   //������Ϣ������


extern TaskHandle_t FEED_DOG;             //ι������������
#define FEED_DOG_TASK_PRIO		5              //�������ȼ�
#define FEED_DOG_STK_SIZE 		30            //�����ջ��С
 
void output_timr(void *pvParameters);    //��ӡʱ�亯��
void set_timr(void *pvParameters);       //��ѯʱ�亯��
void led0_task(void *pvParameters);
void query_task(void *pvParameters);     //query������
void feed_dog(void *pvParameters);       //ι��   500msι��һ��

void start_task(void *pvParameters);     //��ʼ����
#endif



