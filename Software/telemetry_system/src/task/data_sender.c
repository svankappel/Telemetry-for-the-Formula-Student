#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(sta, LOG_LEVEL_DBG);

#include <zephyr/kernel.h>
#include <errno.h>
#include <stdio.h>
#include <zephyr/data/json.h>
#include <zephyr/random/rand32.h>

#include "data_sender.h"
#include "memory_management.h"
#include "deviceInformation.h"
#include "config_read.h"


K_WORK_DEFINE(dataSendWork, Data_Sender);		//dataSendWork -> called by timer to send data

int udpQueueMesLength;		//max length of the json string
uint8_t keepAliveCounter;	//keepalive counter

//-----------------------------------------------------------------------------------------------------------------------
/*! data_Sender_timer_handler is called by the timer interrupt
* @brief data_Sender_timer_handler submit a new work that call Data_Sender task     
*/
void data_Sender_timer_handler()
{
    k_work_submit(&dataSendWork);
}

//-----------------------------------------------------------------------------------------------------------------------
/*! Data_Sender implements the Data_Sender task
* @brief Data_Sender reads the data in the sensor buffer array and
*        creates the json string to send via Wi-Fi to the base 
*        station. This string is placed in the UDP_Client queue.
*/
void Data_Sender() 
{
	//put some random datas in sensor buffer
	for(int i=0; i<configFile.sensorNumber;i++)
	{
		sensorBuffer[i].value=sys_rand32_get()%100;
	}

	//send data only if the system is connected and an IP address is assigned
	if(context.ip_assigned)
	{
		char * memPtr = k_heap_alloc(&messageHeap,udpQueueMesLength,K_NO_WAIT);		//memory allocation for message string

		if(memPtr != NULL)			//memory alloc success
		{
			sprintf(memPtr,"{");	// open json section

			bool first = true;		
			for(int i=0; i<configFile.sensorNumber;i++)	//loop for every sensor
			{
				if(sensorBuffer[i].live)
				{
					//print name and value in json string
					if(first)
						sprintf(memPtr,"%s\"%s\":%d",memPtr,sensorBuffer[i].name,sensorBuffer[i].value);		
					else
						sprintf(memPtr,"%s,\"%s\":%d",memPtr,sensorBuffer[i].name,sensorBuffer[i].value);
					first=false;
				}
			}
			sprintf(memPtr,"%s,\"KeepAliveCounter\":%d",memPtr,keepAliveCounter);		//print keepalive counter
			true ? sprintf(memPtr,"%s,\"LogRecordingSD\":true",memPtr) : sprintf(memPtr,"%s,\"LogRecordingSD\":false",memPtr) ;	//print log recording variable
			strcat(memPtr,"}");		//close json section

			k_queue_append(&udpQueue,memPtr);		//add message to the queue
		}
		else					 //memory alloc fail
		{
			LOG_ERR("data sender memory allocation failed");	//print error
		}	
	}

	keepAliveCounter = keepAliveCounter<99 ? keepAliveCounter+1 : 0 ;		//increment keepalive
}



//-----------------------------------------------------------------------------------------------------------------------
/*! Task_Data_Sender_Init initializes the task Data_Sender
*
* @brief Data Sender initialization
*/
void Task_Data_Sender_Init( void )
{
	keepAliveCounter = 0;		//initialize keep alive

	udpQueueMesLength = 0;				//initialize message length

	//calculate length of json message
	for(int i=0; i<configFile.sensorNumber;i++)		//loop for every sensor
	{
		if(sensorBuffer[i].live)			//if sensor is used in live telemetry
			udpQueueMesLength+=(strlen(sensorBuffer[i].name)+4+10);	//name length + 4 bytes for ,:"" + 10 bytes for number (32bits in decimal)
	}
	udpQueueMesLength+=50;		// space for {} , logRecording and keepalive
}
