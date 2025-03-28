/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wwrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
FreeRTOS is a market leading RTOS from Real Time Engineers Ltd. that supports
31 architectures and receives 77500 downloads a year. It is professionally
developed, strictly quality controlled, robust, supported, and free to use in
commercial products without any requirement to expose your proprietary source
code.

This simple FreeRTOS demo does not make use of any IO ports, so will execute on
any Cortex-M3 of Cortex-M4 hardware.  Look for TODO markers in the code for
locations that may require tailoring to, for example, include a manufacturer
specific header file.

This is a starter project, so only a subset of the RTOS features are
demonstrated.  Ample source comments are provided, along with web links to
relevant pages on the http://www.FreeRTOS.org site.

Here is a description of the project's functionality:

The main() Function:
main() creates the tasks and software timers described in this section, before
starting the scheduler.

The Queue Send Task:
The queue send task is implemented by the prvQueueSendTask() function.
The task uses the FreeRTOS vTaskDelayUntil() and xQueueSend() API functions to
periodically send the number 100 on a queue.  The period is set to 200ms.  See
the comments in the function for more details.
http://www.freertos.org/vtaskdelayuntil.html
http://www.freertos.org/a00117.html

The Queue Receive Task:
The queue receive task is implemented by the prvQueueReceiveTask() function.
The task uses the FreeRTOS xQueueReceive() API function to receive values from
a queue.  The values received are those sent by the queue send task.  The queue
receive task increments the ulCountOfItemsReceivedOnQueue variable each time it
receives the value 100.  Therefore, as values are sent to the queue every 200ms,
the value of ulCountOfItemsReceivedOnQueue will increase by 5 every second.
http://www.freertos.org/a00118.html

An example software timer:
A software timer is created with an auto reloading period of 1000ms.  The
timer's callback function increments the ulCountOfTimerCallbackExecutions
variable each time it is called.  Therefore the value of
ulCountOfTimerCallbackExecutions will count seconds.
http://www.freertos.org/RTOS-software-timer.html

The FreeRTOS RTOS tick hook (or callback) function:
The tick hook function executes in the context of the FreeRTOS tick interrupt.
The function 'gives' a semaphore every 500th time it executes.  The semaphore
is used to synchronise with the event semaphore task, which is described next.

The event semaphore task:
The event semaphore task uses the FreeRTOS xSemaphoreTake() API function to
wait for the semaphore that is given by the RTOS tick hook function.  The task
increments the ulCountOfReceivedSemaphores variable each time the semaphore is
received.  As the semaphore is given every 500ms (assuming a tick frequency of
1KHz), the value of ulCountOfReceivedSemaphores will increase by 2 each second.

The idle hook (or callback) function:
The idle hook function queries the amount of free FreeRTOS heap space available.
See vApplicationIdleHook().

The malloc failed and stack overflow hook (or callback) functions:
These two hook functions are provided as examples, but do not contain any
functionality.
*/

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include "stm32f4_discovery.h"
/* Kernel includes. */
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"



/*-----------------------------------------------------------*/
#define mainQUEUE_LENGTH 100

#define green  	1
#define red  	2
#define blue  	3


/*-----------------------------------------------------------*/
/* Defining values used in program execution */
/*-----------------------------------------------------------*/
#define green_led	LED4
#define red_led		LED5
#define blue_led	LED6

#define TASK1_ID  1
#define TASK2_ID  2
#define TASK3_ID  3

//TASK PRIORITIES TODO: Find and define the correct values
#define TASK_CREATION_PRIORITY        1
#define DD_TASK_GENERATOR_PRIORITY    2
#define TASK_EXECUTION_PRIORITY       0
#define MONITOR_PRIORITY              0
#define DD_TASK_SCHEDULER_PRIORITY    3

/*-----------------------------------------------------------*/
/* Defining Test Benches */
/* Only 1 should be uncommented at a time */
/*-----------------------------------------------------------*/

// Test Bench 1
#define TASK1_EXECUTION_TIME   95
#define TASK1_PERIOD           500
#define TASK2_EXECUTION_TIME   150
#define TASK2_PERIOD           500
#define TASK3_EXECUTION_TIME   250
#define TASK3_PERIOD           500

//// Test Bench 2
//#define TASK1_EXECUTION_TIME  95
//#define TASK1_PERIOD         250
//#define TASK2_EXECUTION_TIME 150
//#define TASK2_PERIOD         500
//#define TASK3_EXECUTION_TIME 250
//#define TASK3_PERIOD         750
//
//// Test Bench 3
//#define TASK1_EXECUTION_TIME 100
//#define TASK1_PERIOD         500
//#define TASK2_EXECUTION_TIME 200
//#define TASK2_PERIOD         500
//#define TASK3_EXECUTION_TIME 200
//#define TASK3_PERIOD         500


/*-----------------------------------------------------------*/
/* Defining Structures Recommended in Slides and Lab Manual */
/*-----------------------------------------------------------*/

typedef enum {PERIODIC, APERIODIC} task_type;

typedef struct dd_task {
	TaskHandle_t t_handle;
	task_type type;
	uint32_t task_id;
	uint32_t release_time;
	uint32_t absolute_deadline;
	uint32_t completion_time;
} dd_task;

typedef struct dd_task_node_t {
	dd_task task;
	struct dd_task_node_t *next_task;
} dd_task_node_t;

// This is the enumerator for the different requests that can be sent to the DDS
typedef enum {TASK_CREATED, GET_ACTIVE_LIST, GET_COMPLETED_LIST, GET_OVERDUE_LIST} request_type;

/*
 * TODO: Implement this function for any hardware specific clock configuration
 * that was not already performed before main() was called.
 */
static void prvSetupHardware( void );

/*
 * The queue send and receive tasks as described in the comments at the top of
 * this file.
 */

/*-----------------------------------------------------------*/
/* Defining Functions */
/*-----------------------------------------------------------*/

static dd_task* create_dd_task( TaskHandle_t t_handle, task_type type, uint32_t task_id, uint32_t absolute_deadline );
static void user_defined_task_1( void *pvParameters );
static void user_defined_task_2( void *pvParameters );
static void user_defined_task_3( void *pvParameters );
static void delete_dd_task( uint32_t task_id );
static void release_dd_task( dd_task *ddtask );

static void DD_Task_Generator_1( void *pvParameters );
static void DD_Task_Generator_2( void *pvParameters );
static void DD_Task_Generator_3( void *pvParameters );

static dd_task_node_t *insert_dd_task_to_other_list( dd_task task, dd_task_node_t *head);
static dd_task_node_t *insert_dd_task_to_active_list( dd_task task, dd_task_node_t *head);
static void DD_Scheduler( void *pvParameters );

//The three declarations below are recommended in the lab manual, but fail the build
//static **dd_task_list get_active_dd_task_list(void);
//static **dd_task_list get_complete_dd_task_list(void);
//static **dd_task_list get_overdue_dd_task_list(void);

/* HANDLES */

QueueHandle_t request_queue = NULL;
QueueHandle_t dd_task_queue = NULL;

TaskHandle_t dd_task_1_generator_handle = NULL;
TaskHandle_t dd_task_2_generator_handle = NULL;
TaskHandle_t dd_task_3_generator_handle = NULL;

SemaphoreHandle_t xTaskExecutionMutex = NULL;


/*-----------------------------------------------------------*/

int main(void)
{

	/* Initialize LEDs needed for Project 2 */
	STM_EVAL_LEDInit(green_led);
	STM_EVAL_LEDInit(red_led);
	STM_EVAL_LEDInit(blue_led);

	/* Configure the system ready to run the demo.  The clock configuration
	can be done here if it was not done before main() was called. */
	prvSetupHardware();


	/* Create the queue used by the DDS to receive requests .*/
	request_queue = xQueueCreate( 	mainQUEUE_LENGTH,		/* The number of items the queue can hold. */
							sizeof( request_type ) );	/* The size of each item the queue holds. */

	/* Create the queue used by the generator to place created dd_tasks */
	dd_task_queue = xQueueCreate( mainQUEUE_LENGTH, sizeof(dd_task) );

	/* Add to the registry, for the benefit of kernel aware debugging. */
	vQueueAddToRegistry( request_queue, "RequestQueue" );
	vQueueAddToRegistry( dd_task_queue, "DDTaskQueue" );
	xTaskExecutionMutex = xSemaphoreCreateBinary();
	xSemaphoreGive(xTaskExecutionMutex); //Start with the semaphore available

	xTaskCreate(DD_Task_Generator_1, "DD Task 1 Generator", configMINIMAL_STACK_SIZE, NULL, DD_TASK_GENERATOR_PRIORITY, &dd_task_1_generator_handle);
	xTaskCreate(DD_Task_Generator_2, "DD Task 2 Generator", configMINIMAL_STACK_SIZE, NULL, DD_TASK_GENERATOR_PRIORITY, &dd_task_2_generator_handle);
	//xTaskCreate(DD_Task_Generator_3, "DD Task 3 Generator", configMINIMAL_STACK_SIZE, NULL, DD_TASK_GENERATOR_PRIORITY, &dd_task_3_generator_handle);

	xTaskCreate(DD_Scheduler, "DD Scheduler", configMINIMAL_STACK_SIZE, NULL, DD_TASK_SCHEDULER_PRIORITY, NULL);

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	return 0;
}

/*-----------------------------------------------------------*/

static dd_task* create_dd_task(TaskHandle_t t_handle, task_type type, uint32_t task_id, uint32_t absolute_deadline){
	dd_task *ddtask;
	ddtask = (dd_task*)pvPortMalloc(sizeof(dd_task));

	if(ddtask == NULL){
		return NULL;
	}

	ddtask->t_handle = t_handle;
	ddtask->type = type;
	ddtask->task_id = task_id;
	ddtask->absolute_deadline = absolute_deadline;

	return ddtask;
}

/*-----------------------------------------------------------*/

static void release_dd_task( dd_task *ddtask ){
	request_type request = TASK_CREATED;
	//Put dd_task on queue
	xQueueSend(dd_task_queue, (void *)ddtask, portMAX_DELAY);
	//Put task_created onto request queue
	xQueueSend(request_queue, &request, portMAX_DELAY);
}

/*-----------------------------------------------------------*/

static void DD_Task_Generator_1( void *pvParameters )
{
	printf("DD_Task_Generator_1 Started\n");
	TickType_t current_time;
	dd_task *created_dd_task;

	while(1)
	{
		current_time = xTaskGetTickCount();

		/* Creating the DD_Task */
		created_dd_task = create_dd_task(NULL, PERIODIC, TASK1_ID, (current_time + TASK1_PERIOD));
		xTaskCreate(user_defined_task_1, "dd_task_1", configMINIMAL_STACK_SIZE, (void*)created_dd_task, TASK_CREATION_PRIORITY, &(created_dd_task->t_handle));

		//Suspend Task execution to allow DDS to handle scheduling
	    vTaskSuspend(created_dd_task->t_handle);

		printf("dd_task_generator 1 released task!\n");
		release_dd_task(created_dd_task); //TODO: Implement or remove and simply place task and message on queues here

		// Wait for the Tasks's period before generating the task again
		vTaskDelay(TASK1_PERIOD);
	}
}

/*-----------------------------------------------------------*/

static void DD_Task_Generator_2( void *pvParameters )
{
	printf("DD_Task_Generator_2 Started\n");
	TickType_t current_time;
	dd_task *created_dd_task;

	while(1)
	{
		current_time = xTaskGetTickCount();

		/* Creating the DD_Task */
		created_dd_task = create_dd_task(NULL, PERIODIC, TASK2_ID, (current_time + TASK2_PERIOD));
		xTaskCreate(user_defined_task_2, "dd_task_2", configMINIMAL_STACK_SIZE, (void*)created_dd_task, TASK_CREATION_PRIORITY, &(created_dd_task->t_handle));
		vTaskSuspend(created_dd_task->t_handle);

		printf("dd_task_generator 2 released task!\n");
		release_dd_task(created_dd_task);

		// Wait for the Tasks's period before generating the task again
		vTaskDelay(TASK2_PERIOD);
	}
}

/*-----------------------------------------------------------*/

static void DD_Task_Generator_3( void *pvParameters )
{
	printf("DD_Task_Generator_3 Started\n");
	TickType_t current_time;
	dd_task *created_dd_task;

	while(1)
	{
		current_time = xTaskGetTickCount();

		/* Creating the DD_Task */
		created_dd_task = create_dd_task(NULL, PERIODIC, TASK3_ID, (current_time + TASK3_PERIOD)); //TODO: Create this function
		xTaskCreate(user_defined_task_3, "dd_task_3", configMINIMAL_STACK_SIZE, (void*)created_dd_task, TASK_CREATION_PRIORITY, &(created_dd_task->t_handle));
		vTaskSuspend(created_dd_task->t_handle);

		printf("dd_task_generator 3 released task!\n");
		release_dd_task(created_dd_task);
		// Wait for the Tasks's period before generating the task again
		vTaskDelay(TASK3_PERIOD);
	}
}

/*-----------------------------------------------------------*/

static void user_defined_task_1( void *pvParameters )
{
	printf("User Defined Task 1 Started\n");
	TaskHandle_t current_task_handle = xTaskGetCurrentTaskHandle();
	if(xSemaphoreTake(xTaskExecutionMutex, portMAX_DELAY) == pdTRUE) {
		STM_EVAL_LEDOn(blue_led);
		vTaskDelay(TASK1_EXECUTION_TIME);
		STM_EVAL_LEDOff(blue_led);
		xSemaphoreGive(xTaskExecutionMutex);
	}

	vTaskSuspend(current_task_handle);
}

/*-----------------------------------------------------------*/

static void user_defined_task_2( void *pvParameters )
{
	printf("User Defined Task 2 Started\n");
	TaskHandle_t current_task_handle = xTaskGetCurrentTaskHandle();

	if(xSemaphoreTake(xTaskExecutionMutex, portMAX_DELAY) == pdTRUE) {
		STM_EVAL_LEDOn(green_led);
		vTaskDelay(TASK2_EXECUTION_TIME);
		STM_EVAL_LEDOff(green_led);
		xSemaphoreGive(xTaskExecutionMutex);
	}

	vTaskSuspend(current_task_handle);
}

/*-----------------------------------------------------------*/

static void user_defined_task_3( void *pvParameters )
{
	printf("User Defined Task 3 Started\n");
	TaskHandle_t current_task_handle = xTaskGetCurrentTaskHandle();

	if(xSemaphoreTake(xTaskExecutionMutex, portMAX_DELAY) == pdTRUE) {
		STM_EVAL_LEDOn(red_led);
		vTaskDelay(TASK3_EXECUTION_TIME);
		STM_EVAL_LEDOff(red_led);
		xSemaphoreGive(xTaskExecutionMutex);
	}

	vTaskSuspend(current_task_handle);
}

/*-----------------------------------------------------------*/

static dd_task_node_t *insert_dd_task_to_other_list( dd_task task, dd_task_node_t *head ){
	//1. Creating the dd_task_node
	dd_task_node_t *new_node;
	new_node = (dd_task_node_t*)pvPortMalloc(sizeof(dd_task_node_t));

	if(new_node == NULL){
		return NULL;
	}

	new_node->task = task;
	new_node->next_task = NULL;

	// 2. Insert the dd_task_node at the end of the list
	if (head == NULL){
		head = new_node;
		return head;
	}

	dd_task_node_t *temp = head;
	while (temp->next_task != NULL){
		temp = temp->next_task;
	}
	temp->next_task = new_node;
	return head;
}

/*-----------------------------------------------------------*/

static dd_task_node_t *insert_dd_task_to_active_list( dd_task task, dd_task_node_t *head ) {
	//1. Creating the dd_task_node
	dd_task_node_t *new_node;
	new_node = (dd_task_node_t*)pvPortMalloc(sizeof(dd_task_node_t));

	if(new_node == NULL){
		return NULL;
	}

	new_node->task = task;
	new_node->next_task = NULL;

	//2. If the list is empty, return the new node as the list
	if (head == NULL){
		head = new_node;
		return head;
	}

	//3. If the new task has the earliest deadline out of all the tasks in the list
	if (task.absolute_deadline <= head->task.absolute_deadline){
		new_node->next_task = head;
		head = new_node;
		return head;
	}

	//4. Traversing the list to find the correct, sorted postion for the new node based on deadline
	dd_task_node_t *temp = head;
	while (temp->next_task != NULL && temp->next_task->task.absolute_deadline < task.absolute_deadline) {
		temp = temp->next_task;
	}

	//5. Add the new node to the list
	new_node->next_task = temp->next_task;
	temp->next_task = new_node;
	return head;
}

/*-----------------------------------------------------------*/

static void DD_Scheduler( void *pvParameters )
{
	printf("DD Scheduler Started\n");

	dd_task_node_t *active_tasks = NULL;
	dd_task_node_t *completed_tasks = NULL;
	dd_task_node_t *overdue_tasks = NULL;

	request_type received_request;
	dd_task received_dd_task;

	TickType_t release_time = 0;

	while(1){
		if(xQueueReceive(request_queue, &received_request, portMAX_DELAY) == pdPASS){ //Only proceed if we received a request
			switch(received_request){
				case TASK_CREATED:
					xQueueReceive(dd_task_queue, &received_dd_task, portMAX_DELAY);

					release_time = xTaskGetTickCount();
					/* TESTING CODE - Must be removed */
//					dd_task *created_dd_task2;
//					created_dd_task2 = create_dd_task(NULL, PERIODIC, TASK2_ID, (release_time + TASK2_PERIOD));
//					dd_task *created_dd_task3;
//					created_dd_task3 = create_dd_task(NULL, PERIODIC, TASK3_ID, (release_time + TASK3_PERIOD));
//					created_dd_task2->release_time = release_time;
//					created_dd_task3->release_time = release_time;
					/* End of testing code */

					received_dd_task.release_time = release_time;

					active_tasks = insert_dd_task_to_active_list(received_dd_task, active_tasks);

//					//TESTING CODE: Remove
//					active_tasks = insert_dd_task_to_other_list(*created_dd_task2, active_tasks);
//					active_tasks = insert_dd_task_to_other_list(*created_dd_task3, active_tasks);
//
//					vTaskPrioritySet(received_dd_task->t_handle, tskIDLE_PRIORITY + 1); //TODO: Set task priority and resume
//					vTaskResume(received_dd_task->task_handle);
					break;
				case GET_ACTIVE_LIST: //TODO: Implement

					break;
				case GET_COMPLETED_LIST: //TODO: Implement
					break;
				case GET_OVERDUE_LIST: //TODO: Implement
					break;
				default:
				//Error
					break;
			}
		}
		vTaskDelay(200);
	}
}


/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* The malloc failed hook is enabled by setting
	configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software 
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  pxCurrentTCB can be
	inspected in the debugger if the task name passed into this function is
	corrupt. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Ensure all priority bits are assigned as preemption priority bits.
	http://www.freertos.org/RTOS-Cortex-M3-M4.html */
	NVIC_SetPriorityGrouping( 0 );

	/* TODO: Setup the clocks, etc. here, if they were not configured before
	main() was called. */
}
