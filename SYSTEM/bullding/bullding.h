#ifndef __BULLDING_H
#define __BULLDING_H

//基础功能函数库
#include "sys.h"
#include "FreeRTOS.h"
#include "task.h"

extern unsigned char temp[10];

extern  TaskHandle_t StartTask_Handler;   //开始函数任务句柄
#define  START_TASK_PRIO		1          //任务优先级
#define  START_STK_SIZE 		60         //任务堆栈大小	


extern  TaskHandle_t Output_Timr;         //打印函数任务句柄
#define  OUTPUT_TIMR_PRIO		1          //任务优先级
#define  OUTPUT_TIMR_SIZE 		40         //任务堆栈大小	


extern TaskHandle_t Set_Timr;             //打印函数任务句柄
#define  SET_TIMR_PRIO		2              //任务优先级
#define  SET_TIMR_SIZE 		60             //任务堆栈大小



extern  TaskHandle_t LED0Task_Handler;    //LED0闪烁任务句柄
#define LED0_TASK_PRIO		3              //任务优先级
#define LED0_STK_SIZE 		40             //任务堆栈大小	  



extern  TaskHandle_t QueryTask_Handler;   //查询任务信息函数任务句柄
#define QUERY_TASK_PRIO		4              //任务优先级
#define QUERY_STK_SIZE 		256            //任务堆栈大小	
extern char InfoBuffer[1000];			   //保存信息的数组


extern TaskHandle_t FEED_DOG;             //喂狗函数任务句柄
#define FEED_DOG_TASK_PRIO		5              //任务优先级
#define FEED_DOG_STK_SIZE 		30            //任务堆栈大小
 
void output_timr(void *pvParameters);    //打印时间函数
void set_timr(void *pvParameters);       //查询时间函数
void led0_task(void *pvParameters);
void query_task(void *pvParameters);     //query任务函数
void feed_dog(void *pvParameters);       //喂狗   500ms喂狗一次

void start_task(void *pvParameters);     //开始任务
#endif



