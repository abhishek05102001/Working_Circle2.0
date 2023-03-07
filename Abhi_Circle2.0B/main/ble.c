#include "ble.h"

uint8_t ble_addr_type;
extern char BLE_RAW_DATA[50];
char Current_Date_Time[100];
static char offline_start_time[100];
char IdTag[50];
extern volatile int offline_meter_start;
struct SendFlags sendFlags;
struct RecvFlags recvFlags;
void ble_mode(void) {
    if(Internet_connected == false)  {
		if ((strncmp(BLE_RAW_DATA, "start+IdTag", 5)==0))   {
			int L2 = 5;//get data from 5 th index values
			for(int i = 0 ; i < strlen(BLE_RAW_DATA);i++)  {
			   IdTag[i] = BLE_RAW_DATA[L2];
			   L2++;
			}
			vTaskDelay(100 / portTICK_RATE_MS);
			printf("charging \n");
			gpio_set_level(15, 0);
			//LED_COLOR(GREEN);
			 Get_current_date_time(offline_start_time);
			 offline_meter_start = 50;
			offline_save_session_start(IdTag, offline_meter_start,offline_start_time);
			append_file("/sdcard/start2.txt", offline_start_time);
			write_file("/spiffs/mode.txt", "1");

			offline_charge = true;
			sendFlags.send_offline_transactions_flag = false;
		}
		if ((strncmp(BLE_RAW_DATA, "stop+IdTag", 4)==0) )   {
			if(offline_charge)  {
				vTaskDelay(100 / portTICK_RATE_MS);
				printf("stop charging \n");
				gpio_set_level(15, 1);
				//LED_COLOR(PINK);
				 Get_current_date_time(Current_Date_Time);
				offline_save_session_stop("500", Current_Date_Time);
				append_file("/sdcard/stop2.txt", Current_Date_Time);
				write_file("/spiffs/mode.txt", "1");

				offline_charge = false;
				sendFlags.send_offline_transactions_flag = true;
			}
			if(online_charge)  {
				vTaskDelay(100 / portTICK_RATE_MS);
				printf("stop charging \n");
				gpio_set_level(15, 1);
				//LED_COLOR(PINK);
				//delete_transaction("/spiffs/online_tid.txt");
				//delete_transaction("/spiffs/online_meterstop.txt");
				Get_current_date_time(Current_Date_Time);
				offline_save_session_stop("500", Current_Date_Time);
				append_file("/sdcard/stop2.txt", Current_Date_Time);
				write_file("/spiffs/mode.txt", "2");

				//save_online_stop_transactions(transactionId, 200, Current_Date_Time);


				offline_charge = false;
				online_charge = false;
				recvFlags.RemoteStop_recflag =false;
				sendFlags.StatusNotification_sendflag = true;
				sendFlags.send_offline_transactions_flag = true;
				  //delete_transaction("/spiffs/online_timestamp.txt");
			}
		}
    }
    else if(Internet_connected == true)  {


		if ((strncmp(BLE_RAW_DATA, "start+IdTag", 5)==0)  && waiting_for_remotestart == true)   {
			int L2 = 5;//get data from 5 th index values
			for(int i = 0 ; i < strlen(BLE_RAW_DATA);i++)  {
			   IdTag[i] = BLE_RAW_DATA[L2];
			   L2++;
			}
			vTaskDelay(100 / portTICK_RATE_MS);
			printf("charging \n");
			waiting_for_remotestart = false;
			sendFlags.StartTransaction_sendflag = true;
			recvFlags.RemoteStart_recflag = false;

		}
		else if ((strncmp(BLE_RAW_DATA, "stop+IdTag", 4)==0) && online_charge == true)   {


				vTaskDelay(100 / portTICK_RATE_MS);
				printf("stop charging \n");
				recvFlags.RemoteStop_recflag = false;
				sendFlags.StopTransaction_sendflag = true;


		}

    }
}

// Write data to ESP32 defined as server
int device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)  {
	printf("Data from the client: %.*s\n", ctxt->om->om_len, ctxt->om->om_data);
	for(int i = 0 ; i < ctxt->om->om_len;i++)  {
		//printf("receimes messages  = %c", data->data_ptr[i]);
		BLE_RAW_DATA[i] = ctxt->om->om_data[i];
	}
	//strcpy(BLE_RAW_DATA,ctxt->om->om_data );
	printf("BLE_RAW_DATA length from the client: %d\n", ctxt->om->om_len);
	printf("BLE_RAW_DATA from the client: %s\n", BLE_RAW_DATA);
	set_ssid();
	ble_mode();
	return 0;
}

// Read data from ESP32 defined as server
int device_read(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)  {
    os_mbuf_append(ctxt->om, "Data from the server", strlen("Data from the server"));
    return 0;
}

// Array of pointers to other service definitions
// UUID - Universal Unique Identifier
// BLE event handling
int ble_gap_event(struct ble_gap_event *event, void *arg)  {
	switch (event->type)  {
		// Advertise if connected
		case BLE_GAP_EVENT_CONNECT: {
			ESP_LOGI("GAP", "BLE GAP EVENT CONNECT %s", event->connect.status == 0 ? "OK!" : "FAILED!");
			if (event->connect.status != 0)  {
			  ble_app_advertise();
			}
			break;
		}
		// Advertise if connected
		case BLE_GAP_EVENT_DISCONNECT:
		ESP_LOGI("GAP", "BLE GAP EVENT DISCONNECTED %s", event->connect.status == 1 ? "OK!" : "DISCONNECTED!");
		if (event->connect.status != 1)  {
		  ble_app_advertise();
		}
		break;
		// Advertise again after completion of the event
		case BLE_GAP_EVENT_ADV_COMPLETE:
		ESP_LOGI("GAP", "BLE GAP EVENT");
		ble_app_advertise();
		break;
		default:
		break;
	}
	return 0;
}

// Define the BLE connection
void ble_app_advertise(void)  {
    // GAP - device name definition
    struct ble_hs_adv_fields fields;
    const char *device_name;
    memset(&fields, 0, sizeof(fields));
    device_name = ble_svc_gap_device_name(); // Read the BLE device name
    fields.name = (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;
    ble_gap_adv_set_fields(&fields);

    // GAP - device connectivity definition
    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND; // connectable or non-connectable
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN; // discoverable or non-discoverable
    ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
}

// The application
void ble_app_on_sync(void)  {
    ble_hs_id_infer_auto(0, &ble_addr_type); // Determines the best address type automatically
    ble_app_advertise();                     // Define the BLE connection
}

// The infinite task
void host_task(void *param)  {
    nimble_port_run(); // This function will return only when nimble_port_stop() is executed
}
