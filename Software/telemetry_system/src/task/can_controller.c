#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(can);
#include <nrfx_clock.h>
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/can.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/byteorder.h>

#include "can_controller.h"
#include "memory_management.h"
#include "config_read.h"



//! Can controller thread priority level
#define CAN_CONTROLLER_STACK_SIZE 8192
//! Can controller thread priority level
#define CAN_CONTROLLER_PRIORITY 2


//! Can controller stack definition
K_THREAD_STACK_DEFINE(CAN_CONTROLLER_STACK, CAN_CONTROLLER_STACK_SIZE);
//! Variable to identify the can controller thread
static struct k_thread canControllerThread;


//sensor buffer and mutex to protect it
tSensor sensorBuffer[MAX_SENSORS];
K_MUTEX_DEFINE(sensorBufferMutex);

//can device
const struct device *const can_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));

//can receive message queue
CAN_MSGQ_DEFINE(can_msgq, 5);


//-----------------------------------------------------------------------------------------------------------------------
/*! CAN_Controller implements the CAN_Controller task
* @brief CAN_Controller read the CAN Bus and fill the sensorBuffer array 
*        
*/
void CAN_Controller(void)
{
	//check if canbus device is ready	
	if (!device_is_ready(can_dev)) {
		printk("CAN: Device %s not ready.\n", can_dev->name);
		return;
	}

	//start canbus controller
	int ret = can_start(can_dev);
	if (ret != 0) {
		printk("Error starting CAN controller [%d]", ret);
		return;
	}
	
	//set can filter (mask = 0, takes all frames std and ext)
	const struct can_filter filter =
	{
		.flags=CAN_FILTER_DATA | CAN_FILTER_IDE,
		.id = 0,
		.mask = 0
	};
	can_add_rx_filter_msgq(can_dev, &can_msgq, &filter);

	//frame struct
	struct can_frame frame;

	while (1) 
	{
		k_msgq_get(&can_msgq, &frame, K_FOREVER);

		k_mutex_lock(&sensorBufferMutex,K_FOREVER);
		for(int i = 0; i<configFile.sensorCount;i++)
		{
			if(sensorBuffer[i].canID==frame.id)
			{
				if(sensorBuffer[i].B1==-1 && sensorBuffer[i].B2==-1 &&		//if no bytes assigned (config file error)
					sensorBuffer[i].B1==-1 && sensorBuffer[i].B2==-1)
				{
					break;													//break loop
				}

				if(sensorBuffer[i].dlc != frame.dlc)						//break lool if dlc error (config file error)
					break;

				sensorBuffer[i].value = 
				frame.data[sensorBuffer[i].B1] + 
				(sensorBuffer[i].B2 != -1) ? frame.data[sensorBuffer[i].B2] << 8 : 0 +
				(sensorBuffer[i].B3 != -1) ? frame.data[sensorBuffer[i].B3] << 16 : 0 +
				(sensorBuffer[i].B4 != -1) ? frame.data[sensorBuffer[i].B4] << 24 : 0 ;
	
				printk("Frame : 0x%x 0x%x\n",frame.data[0],frame.data[1]);
				printk("value : 0x%x - %d\n",sensorBuffer[i].value,sensorBuffer[i].value);

			}
		}
		k_mutex_unlock(&sensorBufferMutex);
	}
}


//-----------------------------------------------------------------------------------------------------------------------
/*! Task_CAN_Controller_Init implements the CAN_Controller task initialization
* @brief CAN_Controller thread initialization
*      
*/
void Task_CAN_Controller_Init( void )
{
	k_thread_create	(&canControllerThread,
					CAN_CONTROLLER_STACK,										        
					CAN_CONTROLLER_STACK_SIZE,
					(k_thread_entry_t)CAN_Controller,
					NULL,
					NULL,
					NULL,
					CAN_CONTROLLER_PRIORITY,
					0,
					K_NO_WAIT);	

	 k_thread_name_set(&canControllerThread, "canController");
	 k_thread_start(&canControllerThread);
}



