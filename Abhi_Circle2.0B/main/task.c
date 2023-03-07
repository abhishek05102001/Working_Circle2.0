#include "task.h"


#define WHITE 0
#define RED 1
#define GREEN 2
#define BLUE 3
#define ORANGE 4
#define PINK 5
#define YELLOW 6
#define VOLTAGE_PIN ADC1_CHANNEL_7//33
#define CURRENT_PIN ADC1_CHANNEL_6//34

xQueueHandle voltage_queue;
xQueueHandle current_queue;
int error_flag = 0;
int ocpp_uuid = 531531530;
int transactionId = 0;

static float adc_raw_voltage[1000];
static volatile float max_adc_raw_voltage = 0;
static volatile float AC_Voltage_RMS =0;
static volatile float AC_Current_RMS =0;
static volatile float ocpp_kWh =0,ocpp_voltage =0,ocpp_current=0,kWh = 0 ,W = 0;

char IdTag[50];
char rs_accepted_serialnumber[200];
char rstop_accepted_serialnumber[200];
char Current_Date_Time[100] = "2022-05-21T17:03:23.000Z";
char OCPP_Date_Time[100] = "2022-05-21T17:03:23.000Z";
int offline_meter_start;
struct SendFlags sendFlags;
struct RecvFlags recvFlags;

void voltageTask(void* pvParameters) {
    float voltage;
    while (1) {
        if (xQueueReceive(voltage_queue, &voltage, portMAX_DELAY) == pdPASS) {
            printf("AC voltage out of range: (TASK) %fV\n", voltage);
            gpio_set_level(15, 1);
            sendFlags.MeterValues_sendflag = false;
            sendFlags.StopTransaction_sendflag = true;
            recvFlags.RemoteStop_recflag = false;
            waiting_for_remotestart = false;
            online_charge = false;
            offline_charge = false;
            if(AC_Voltage_RMS < 180){
            send_status_notification(ocpp_uuid,1, "UnderVoltage", "Faulted","170.00",OCPP_Date_Time);}
            if(AC_Voltage_RMS > 270){
            send_status_notification(ocpp_uuid,1, "OverVoltage", "Faulted","270.00",OCPP_Date_Time);}
        }
    }
}

void currentTask(void* pvParameters) {
    float current;
    while (1)
    {
        if (xQueueReceive(current_queue, &current, portMAX_DELAY) == pdPASS) {
            printf("AC current out of range:(TASK) %fA\n", current);
            gpio_set_level(15, 1);
            sendFlags.MeterValues_sendflag = false;
            sendFlags.StopTransaction_sendflag = true;
            recvFlags.RemoteStop_recflag = false;
            waiting_for_remotestart = true;
            online_charge = false;
            offline_charge = false;
        }
    }
}



void task1(void *arg)  {


	while(1)  {
		//printf(" task 1 running \n");
		vTaskDelay(100 / portTICK_RATE_MS);
		//if(Internet_connected && !websocket_disconnect)
		if(Internet_connected)  {
			while(sendFlags.send_offline_transactions_flag)  {
				printf("Internet connected sending offline transaction\n");
				send_offline_transactions();
				sendFlags.send_offline_transactions_flag = false;
			}
			if(sendFlags.BootNotification_sendflag && Internet_connected == true)   {
				ocpp_uuid++;
				send_bootnotification(ocpp_uuid,"smartplug", "statiq");
				recvFlags.BootNotification_recflag = true;
				sendFlags.BootNotification_sendflag = false;
				vTaskDelay(100 / portTICK_RATE_MS);
				append_file("/sdcard/boot.txt", "bootnotification");
			}
			else if(sendFlags.StatusNotification_sendflag && Internet_connected == true)  {
				ocpp_uuid++;
				Get_current_date_time(Current_Date_Time);
				send_status_notification(ocpp_uuid,1, "NoError", "Available","230.00",OCPP_Date_Time);//
				sendFlags.StatusNotification_sendflag = false;
				recvFlags.StatusNotification_recflag = true;
				vTaskDelay(100 / portTICK_RATE_MS);

			}

			else if(sendFlags.RemoteStart_sendflag && Internet_connected == true )  {
				send_remotestart_transaction(rs_accepted_serialnumber);
				sendFlags.RemoteStart_sendflag = false;
				sendFlags.StartTransaction_sendflag = true;
				vTaskDelay(100 / portTICK_RATE_MS);
			}
			else if(sendFlags.StartTransaction_sendflag && Internet_connected == true)  {
					ocpp_uuid++;
					//Get_current_date_time(Current_Date_Time);
					append_file("/sdcard/start1.txt", OCPP_Date_Time);
				if(offline_charge) {
					send_start_transaction(ocpp_uuid,1, offline_meter_start,OCPP_Date_Time, IdTag);
				}
				else  {
					append_file("/sdcard/start1.txt", OCPP_Date_Time);
					append_file("/spiffs/meterstop.txt", "0000");
					append_file("/spiffs/stop_timestamp.txt",OCPP_Date_Time);
					send_start_transaction(ocpp_uuid,1, 93,OCPP_Date_Time, IdTag);
					//Relay(1);
					gpio_set_level(15, 0);
					LED_COLOR(GREEN);
				}

				recvFlags.StartTransaction_recflag = true;
				sendFlags.StartTransaction_sendflag = false;

				vTaskDelay(100 / portTICK_RATE_MS);

			}
			else if(sendFlags.StatusNotification_sendflag2 )  {

				printf("sending status notification 2 \n");
				ocpp_uuid++;
				printf("sending status notification \n");
				//Get_current_date_time(Current_Date_Time);
				send_status_notification(ocpp_uuid,1, "NoError", "Charging","230.00",OCPP_Date_Time);
				sendFlags.StatusNotification_sendflag2 = false;
				recvFlags.StatusNotification_recflag2 = true;

				vTaskDelay(100 / portTICK_RATE_MS);

			}
			else if(sendFlags.RemoteStop_sendflag && Internet_connected == true)  {
				send_remotestop(rstop_accepted_serialnumber);
				memset(rstop_accepted_serialnumber, 0, 200);
				sendFlags.RemoteStop_sendflag = false;
				sendFlags.StopTransaction_sendflag = true;
				vTaskDelay(100 / portTICK_RATE_MS);

			}
			else if(sendFlags.StopTransaction_sendflag && Internet_connected == true){
				ocpp_uuid++;
				//Get_current_date_time(Current_Date_Time);
				start_energymeter = false;
				sendFlags.MeterValues_sendflag = false;
				send_stoptransaction(ocpp_uuid,300,OCPP_Date_Time, transactionId);
				append_file("/sdcard/stop1.txt", OCPP_Date_Time);
				gpio_set_level(15, 1);
				//LED_COLOR(ORANGE);
				recvFlags.StopTransaction_recflag = true;
				sendFlags.StopTransaction_sendflag = false;
				vTaskDelay(100 / portTICK_RATE_MS);
				vTaskDelay(100 / portTICK_RATE_MS);
			}
			vTaskDelay(100 / portTICK_RATE_MS);
		}
		//else if(websocket_disconnect && Internet_connected)
		//{
			//websocket_app_start();
		//}

	}
	  vTaskDelete(NULL);

}

void task2(void *arg)  {

	while(1)  {
		//printf("task 2 running \n");
		vTaskDelay(100 / portTICK_RATE_MS);
		if(sendFlags.HeartBeat_sendflag ==true && Internet_connected == true )  {
			printf("sending heartbeat \n");
			ocpp_uuid++;
			send_heartbeat(ocpp_uuid);
			//HeartBeat_sendflag = false;
			recvFlags.HeartBeat_recflag = true;
			vTaskDelay(5000 / portTICK_RATE_MS);
		}

		if(sendFlags.MeterValues_sendflag ==true && Internet_connected == true && start_energymeter == true)  {

			//MeterValues_sendflag = false;
			ocpp_uuid++;
			recvFlags.MeterValues_recflag = true;
			//Get_current_date_time(Current_Date_Time);
			send_metervalues(ocpp_uuid,OCPP_Date_Time, &transactionId,ocpp_voltage,ocpp_current,ocpp_kWh);
			vTaskDelay(5000 / portTICK_RATE_MS);

		}
	}
	  vTaskDelete(NULL);

}
void task3(void *arg)  {

	while(1)  {

		 vTaskDelay(5000/ portTICK_PERIOD_MS);
		 memset(adc_raw_voltage, 0, sizeof(adc_raw_voltage));
		 max_adc_raw_voltage = 0;
		 AC_Voltage_RMS =0;
		 for(int j=0;j<2;j++){
		 for (int i = 0; i < 600; i++)
		 {
		    //adc_raw_current[i] = adc1_get_raw(ADC1_CHANNEL_6);//34
		    //if ( adc_raw_current[i] >= max_adc_raw_current)
			//{
		    //max_adc_raw_current = adc_raw_current[i];
		    //}
			adc_raw_voltage[i] = adc1_get_raw(VOLTAGE_PIN);//34
			if ( adc_raw_voltage[i] >= max_adc_raw_voltage)
			{
			max_adc_raw_voltage = adc_raw_voltage[i];
			}
		  }
		 for (int i = 0; i < 600; i++)
		 		 {
		 		    //adc_raw_current[i] = adc1_get_raw(ADC1_CHANNEL_6);//34
		 		    //if ( adc_raw_current[i] >= max_adc_raw_current)
		 			//{
		 		    //max_adc_raw_current = adc_raw_current[i];
		 		    //}
		 			adc_raw_voltage[i] = adc1_get_raw(VOLTAGE_PIN);//34
		 			if ( adc_raw_voltage[i] >= max_adc_raw_voltage)
		 			{
		 			max_adc_raw_voltage = adc_raw_voltage[i];
		 			}
		 		  }
		 for (int i = 0; i < 600; i++)
		 		 {
		 		    //adc_raw_current[i] = adc1_get_raw(ADC1_CHANNEL_6);//34
		 		    //if ( adc_raw_current[i] >= max_adc_raw_current)
		 			//{
		 		    //max_adc_raw_current = adc_raw_current[i];
		 		    //}
		 			adc_raw_voltage[i] = adc1_get_raw(VOLTAGE_PIN);//34
		 			if ( adc_raw_voltage[i] >= max_adc_raw_voltage)
		 			{
		 			max_adc_raw_voltage = adc_raw_voltage[i];
		 			}
		 		  }
		 for (int i = 0; i < 600; i++)
		 		 {
		 		    //adc_raw_current[i] = adc1_get_raw(ADC1_CHANNEL_6);//34
		 		    //if ( adc_raw_current[i] >= max_adc_raw_current)
		 			//{
		 		    //max_adc_raw_current = adc_raw_current[i];
		 		    //}
		 			adc_raw_voltage[i] = adc1_get_raw(VOLTAGE_PIN);//34
		 			if ( adc_raw_voltage[i] >= max_adc_raw_voltage)
		 			{
		 			max_adc_raw_voltage = adc_raw_voltage[i];
		 			}
		 		  }
		 for (int i = 0; i < 600; i++)
		 		 		 {
		 		 		    //adc_raw_current[i] = adc1_get_raw(ADC1_CHANNEL_6);//34
		 		 		    //if ( adc_raw_current[i] >= max_adc_raw_current)
		 		 			//{
		 		 		    //max_adc_raw_current = adc_raw_current[i];
		 		 		    //}
		 		 			adc_raw_voltage[i] = adc1_get_raw(VOLTAGE_PIN);//34
		 		 			if ( adc_raw_voltage[i] >= max_adc_raw_voltage)
		 		 			{
		 		 			max_adc_raw_voltage = adc_raw_voltage[i];
		 		 			}
		 		 		  }
		 }

		  	  	  /*AC_Current_RMS =(0.0345*max_adc_raw_current) - 16.872;

		  	  	  if(AC_Current_RMS<0)
		  	  	  {
		  	  		AC_Current_RMS = 0;
		  	  	  }
		  	  	  else if(AC_Current_RMS>0.5&&AC_Current_RMS<5)
		  	  	  {
		  	  			  AC_Current_RMS -= 0.3;
		  	  	  }
		  	  	  else if(AC_Current_RMS>=5.5 && AC_Current_RMS<10)
		  	  	  {
		  	  		AC_Current_RMS += 0.2;
		  	  	  }
		  	  	  else if(AC_Current_RMS>=11 && AC_Current_RMS<12)
		  	  	  {
		  	  		AC_Current_RMS -= 0.20;
		  	  	  }
		  	  	  else if(AC_Current_RMS>=12)
		  	  	  {
		  	  		AC_Current_RMS -= 0.6;
		  	  	  }*/
						//printf("Largest value A = %f AC Current RMS = %0.1f \n",max_adc_raw_current,AC_Current_RMS);
		        AC_Voltage_RMS =  (max_adc_raw_voltage - 477)/(1.186);
		        if(AC_Voltage_RMS<0)
		        {
		       		 AC_Voltage_RMS = 0;
		       	}
		printf("Largest value V = %f AC Voltage RMS = %f \n",max_adc_raw_voltage,AC_Voltage_RMS);
		if (AC_Voltage_RMS > 270 || AC_Voltage_RMS < 180) {
		       if (error_flag == 0) {
		                float voltage = AC_Voltage_RMS;
		                xQueueSend(voltage_queue, &voltage, portMAX_DELAY);
		                error_flag = 1;
		            }
		  }
		else if (AC_Voltage_RMS < 270 || AC_Voltage_RMS > 180)
		{
		          if (error_flag == 1) {
		                // AC voltage back to normal
		                waiting_for_remotestart = true;
		                sendFlags.BootNotification_sendflag = true;
		                online_charge = false;
		                offline_charge = false;
		                waiting_for_remotestart = true;
		                error_flag = 0;
		            }
		 }
	    if(AC_Current_RMS < 0 && start_energymeter == false && recvFlags.StartTransaction_recflag == true)
		{

	    	for(int i = 0;i<30;i++)
	    		    {
	    		    	printf("TIME BEFORE CUTOFF: %d \n",i+1);
	    		    	vTaskDelay(1000 / portTICK_RATE_MS);
	    		    }
	    	float current = AC_Current_RMS;
		    xQueueSend(current_queue, &current, portMAX_DELAY);

		}
	    	if(AC_Current_RMS < 0 && sendFlags.MeterValues_sendflag == true)
	    	{
	    		    	float current = AC_Current_RMS;
	    			    xQueueSend(current_queue, &current, portMAX_DELAY);
	    	}


		vTaskDelay(1000 / portTICK_RATE_MS);
		//AC_Current_RMS = AC_Current_RMS - 0.55;
		ocpp_current = AC_Current_RMS;
		ocpp_voltage = AC_Voltage_RMS;
		W = AC_Voltage_RMS * AC_Current_RMS;
	    //printf("Watts =  %f \n ",W);

	}
	  vTaskDelete(NULL);
}

void task4(void *arg)  {

	while(1)  {
		//printf("task 4 running \n");
		kWh = kWh + (( W * 0.000277)/1000);
		ocpp_kWh = kWh;
		//printf("AC_Voltage_RMS = %f AC_Current_RMS = %f kWh = %f \n",AC_Voltage_RMS,AC_Current_RMS,kWh);
		vTaskDelay(1000 / portTICK_RATE_MS);

	}
	  vTaskDelete(NULL);
}

void task5(void *arg)  {

	while(1)  {
		//printf("task 5 running \n");
		vTaskDelay(100 / portTICK_RATE_MS);
		if(Internet_connected == true && waiting_for_remotestart == true && online_charge == false && offline_charge == false) {
		    LED_COLOR(BLUE);
		}
		else if(Internet_connected == false &&  online_charge == false && offline_charge == false)  {
		    LED_COLOR(PINK);
		}
		else if(Internet_connected == true &&  online_charge == true && offline_charge == false)  {
			LED_COLOR(GREEN);
			vTaskDelay(500 / portTICK_RATE_MS);
	         //LED_COLOR(BLUE);
			//vTaskDelay(500 / portTICK_RATE_MS);
		}
		else if(Internet_connected == true &&  online_charge == true && offline_charge == true)  {
			LED_COLOR(GREEN);
			vTaskDelay(500 / portTICK_RATE_MS);
			//LED_COLOR(BLUE);
			//vTaskDelay(500 / portTICK_RATE_MS);
		}
		else if(Internet_connected == false &&  online_charge == false && offline_charge == true)  {
			//LED_COLOR(GREEN);
			//vTaskDelay(500 / portTICK_RATE_MS);
			LED_COLOR(PINK);
			vTaskDelay(500 / portTICK_RATE_MS);
		}
		else if(Internet_connected == false &&  online_charge == true && offline_charge == false)  {
			//LED_COLOR(GREEN);
			//vTaskDelay(500 / portTICK_RATE_MS);
			LED_COLOR(PINK);
			vTaskDelay(500 / portTICK_RATE_MS);
		}
		else if(sendFlags.MeterValues_sendflag == false && sendFlags.StopTransaction_sendflag == true && recvFlags.RemoteStop_recflag == false && waiting_for_remotestart == false &&  online_charge == false && offline_charge == false){
			LED_COLOR(RED);
			vTaskDelay(500 / portTICK_RATE_MS);
		}

	}
	  vTaskDelete(NULL);
}
