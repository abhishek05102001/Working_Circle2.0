#include "ocpp.h"


static const int timeout = 120;
static volatile int elapsed_time = 0;
static volatile bool ongoing_charging = false;
extern char OCPP_Date_Time[100];
extern volatile int error_flag;
static char HBUUID[200];
extern char rs_accepted_serialnumber[200];
extern char rstop_accepted_serialnumber[200];
static char recdata[600] ;
static char bstatus[50];
static char snnumber[30];
static char Task_RemoteStartTransaction[50];
static char Task_RemoteStopTransaction[50];
static char StartTransaction_Status[50];
static char StopTransaction_Status[50];
extern volatile int ocpp_uuid;
static int a1 =0;
static int a2 =0;
static int ConnectorId = -1;
extern int transactionId;
static int transactionId2 =0;
static int HBnumber =0;
extern char IdTag[50];
char result[100];
#define GREEN 2
struct SendFlags sendFlags;
struct RecvFlags recvFlags;

static const char *TAG = "OCPP";
extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
	esp_websocket_client_config_t  websocket_cfg = {
			.uri = result,
			//.port = 443,
			.subprotocol = "ocpp1.6",
			.cert_pem = (const char *)server_cert_pem_start,
	};
esp_websocket_client_handle_t client;

static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)  {
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    switch (event_id) {
		case WEBSOCKET_EVENT_CONNECTED:  {
			ESP_LOGI(TAG, "WEBSOCKET_EVENT_CONNECTED");
			vTaskDelay(100 / portTICK_RATE_MS);
			Internet_connected = true;
			if(waiting_for_remotestart == false && online_charge ==false && offline_charge == false){
				sendFlags.BootNotification_sendflag = true;
				printf("bootnotification initiated \n");
			}
			//LED_COLOR(BLUE);
			break;
		}
		case WEBSOCKET_EVENT_DISCONNECTED:  {
			ESP_LOGI(TAG, "WEBSOCKET_EVENT_DISCONNECTED");
			break;
		}
		case WEBSOCKET_EVENT_DATA:  {
			ESP_LOGI(TAG, "WEBSOCKET_EVENT_DATA");
			ESP_LOGI(TAG, "Received opcode=%d", data->op_code);
			if (data->op_code == 0x08 && data->data_len == 2) {
				ESP_LOGW(TAG, "Received closed message with code=%d", 256*data->data_ptr[0] + data->data_ptr[1]);
			}
			else {
				ESP_LOGW(TAG, "Received=%.*s", data->data_len, (char *)data->data_ptr);
				// memset(recdata,0,600);
				for(int i = 0 ; i < data->data_len;i++)  {
					recdata[i] = data->data_ptr[i];
					// recdata2[i] = data->data_ptr[i];
				}
				printf("variable stored data = %s\n",recdata);
				//printf("variable stored data 2 = %s\n",recdata2);
				vTaskDelay(100 / portTICK_RATE_MS);

				if(recvFlags.BootNotification_recflag == true)  {
					vTaskDelay(100 / portTICK_RATE_MS);
					printf(" rec boot notification %s\n",recdata);
					char boot_uuid[100];
					rec_bootnotification(boot_uuid,OCPP_Date_Time,bstatus,recdata);
					if(strcmp(bstatus, "Accepted")==0)  {
						recvFlags.BootNotification_recflag = false;
						if(ongoing_charging == false){
							sendFlags.StatusNotification_sendflag = true;}
						else if(ongoing_charging == true){
							send_status_notification(ocpp_uuid,1, "NoError", "Charging","230.00",OCPP_Date_Time);
							sendFlags.StatusNotification_sendflag = false;
						}

						memset(recdata, 0, 600);
						memset(bstatus, 0, 50);
						printf("  boot notification received \n");
					}
				}
				else if(recvFlags.StatusNotification_recflag == true && Internet_connected == true)  {
					vTaskDelay(100 / portTICK_RATE_MS);
					printf("receiving status notification \n");
					printf("%s \n",recdata);


					rec_status_notification(&a1, snnumber,recdata);
					printf(" a1 value %d \n",a1);
					if(a1 >0)  {
						recvFlags.StatusNotification_recflag = false;
						sendFlags.HeartBeat_sendflag = true;
						recvFlags.RemoteStart_recflag = true;
						printf("  sn notification received \n");

						memset(recdata, 0, 600);
						a1 =0;
						vTaskDelay(100 / portTICK_RATE_MS);

						vTaskDelay(100 / portTICK_RATE_MS);
					}

				}
				else  if(error_flag==0 && recvFlags.RemoteStart_recflag == true && Internet_connected == true && online_charge == false &&offline_charge == false)  {
					waiting_for_remotestart = true;
					vTaskDelay(100 / portTICK_RATE_MS);
					printf("waiting for  RemoteStartTransaction  \n");
					//waiting_for_remotestart = true;
					printf("%s  \n",recdata);
					rec_remotestart_transaction(IdTag, &ConnectorId, recdata,rs_accepted_serialnumber);
					if(strcmp(Task_RemoteStartTransaction, "RemoteStartTransaction")==0)  {
						recvFlags.RemoteStart_recflag = false;
						sendFlags.RemoteStart_sendflag = true;
						memset(recdata, 0, 600);
						memset(Task_RemoteStartTransaction, 0, 50);
						printf("  RemoteStartTransaction received \n");
						waiting_for_remotestart = false;
						sendFlags.StartTransaction_sendflag = true;
					}
					if(offline_charge)  {
						recvFlags.RemoteStart_recflag = false;
						sendFlags.StartTransaction_sendflag = true;
					}
					vTaskDelay(100 / portTICK_RATE_MS);
				}
				else  if(recvFlags.StartTransaction_recflag && Internet_connected == true)  {
					vTaskDelay(100 / portTICK_RATE_MS);
					printf("waiting for  StartTransaction Accepted by central system    \n");
					printf("%s   \n",recdata);
					rec_start_transaction(&transactionId, StartTransaction_Status, recdata);
					if(strcmp(StartTransaction_Status, "Accepted")==0)  {
						vTaskDelay(100 / portTICK_RATE_MS);
						recvFlags.StartTransaction_recflag = false;
						sendFlags.StatusNotification_sendflag2 = true;
						// offline_charge = true;
						online_charge = true;
						memset(recdata, 0, 600);
						memset(StartTransaction_Status, 0, 50);
						printf("  StartTransaction Accepted \n");
						//gpio_set_level(15, 0);//on relay
						LED_COLOR(GREEN);
						ongoing_charging = true;
						char temp_char_transactionId[100];
						itoa(transactionId,temp_char_transactionId,10);
						append_file("/spiffs/transaction_id.txt", temp_char_transactionId);
						write_file("/spiffs/mode.txt", "2");
						delete_file("/spiffs/idtag.txt");
						delete_file("/spiffs/meterstart.txt");
						delete_file("/spiffs/start_timestamp.txt");
					}
				}

				else  if(recvFlags.StatusNotification_recflag2)  {
					vTaskDelay(100 / portTICK_RATE_MS);
					printf("receiving status notification \n");
					printf("%s\n",recdata);
					rec_status_notification(&a2, snnumber,recdata);
					if(a2 >0 )  {
						a2 = 0;
						recvFlags.StatusNotification_recflag2 = false;
						recvFlags.RemoteStop_recflag = true;
						start_energymeter = true;
						printf("  status notification2 received \n");
						memset(recdata, 0, 600);
						sendFlags.MeterValues_sendflag =true;
					}
					vTaskDelay(100 / portTICK_RATE_MS);
				}

				else  if(recvFlags.RemoteStop_recflag && Internet_connected == true)   {
					vTaskDelay(100 / portTICK_RATE_MS);
					printf("waiting for  RemoteStopTransaction  \n");
					printf(" data %s\n",recdata);
					rec_remotestop(Task_RemoteStopTransaction, &transactionId2, recdata,rstop_accepted_serialnumber);
					if(transactionId2 >0){
						recvFlags.RemoteStop_recflag = false;
						sendFlags.RemoteStop_sendflag = true;
						ongoing_charging = false;
						memset(recdata, 0, 600);
						printf("  RemoteStopTransaction received \n");

					}
				}
				else  if(recvFlags.StopTransaction_recflag && Internet_connected == true) {
					vTaskDelay(100 / portTICK_RATE_MS);
					printf("waiting for  Stop transaction Accepted by central system    \n");
					printf("%s   \n",recdata);
					rec_stoptransaction(StopTransaction_Status,recdata);

					if(strcmp(StopTransaction_Status, "Accepted")==0)  {

						recvFlags.StopTransaction_recflag = false;
						sendFlags.StatusNotification_sendflag = true;
						offline_charge = false;
						online_charge = false;
						memset(recdata, 0, 600);
						memset(StopTransaction_Status, 0, 50);
						transactionId2 =0;
						printf("  Stop transaction Accepted \n");
						//LED_COLOR(ORANGE);
						gpio_set_level(15, 1);//off relay
						delete_file("/spiffs/meterstop.txt");
						delete_file("/spiffs/stop_timestamp.txt");
						delete_file("/spiffs/transaction_id.txt");
						delete_file("/spiffs/mode.txt");
					}
				}
				//heartbeat rec start
				printf(" rec heartbeat data %s\n",recdata);
				vTaskDelay(100 / portTICK_RATE_MS);
				rec_heartbeat(&HBnumber,HBUUID, OCPP_Date_Time,recdata);
				if(HBnumber>0)  {
					HBnumber =0;
					//HeartBeat_recflag = false;
					//RemoteStart_recflag = true;
					memset(recdata, 0, 600);
					printf("  heartbeat received current time = %s \n",OCPP_Date_Time);
				}
				//heartbeat rec end

			}
			ESP_LOGW(TAG, "Total payload length=%d, data_len=%d, current payload offset=%d\r\n", data->payload_len, data->data_len, data->payload_offset);


			break;
		}
		case WEBSOCKET_EVENT_ERROR:  {
			ESP_LOGI(TAG, "WEBSOCKET_EVENT_ERROR");
			break;
		}
    }
}


void websocket_app_start(void)  {
	ESP_LOGI(TAG, "Connecting to %s...", websocket_cfg.uri);
	client = esp_websocket_client_init(&websocket_cfg);
	esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)client);
	esp_websocket_client_start(client);
}

void websocket_app_stop(void)
{
	printf("\n STOP STOP STOP STOP \n");
    esp_err_t err = esp_websocket_client_stop(client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to stop websocket client: %s", esp_err_to_name(err));
    }

}

void rec_bootnotification(char *rec_uuid ,char *currentTime,char *bootnotification_status ,char *received_data)  {

	cJSON *root = cJSON_Parse(received_data);
	cJSON *e1 = cJSON_GetArrayItem(root, 0);
	cJSON *e2 = cJSON_GetArrayItem(root, 1);
	if(e1)  {
		int a1 = e1->valueint;
		printf(" id %d \n", a1);
	}
	if(e2)  {
		if(e2->valuestring)  {
			strcpy(rec_uuid, e2->valuestring);
			printf(" rec_uuid = %s \n", rec_uuid);
		}

	}
	cJSON *e3 = cJSON_GetArrayItem(root, 2);
	cJSON *e31 = cJSON_GetObjectItem(e3, "currentTime");
	if(e31)  {
		if(e31->valuestring)  {
			strcpy(currentTime, e31->valuestring);
			printf(" bootnotification time %s \n", currentTime);

		}
	}
	cJSON *e32 = cJSON_GetObjectItem(e3, "interval");
	if(e32)  {
		if(e32->valueint)  {
			int interval = e32->valueint;
			printf(" interval %d \n", interval);
		}
	}
	cJSON *e33 = cJSON_GetObjectItem(e3, "status");
	if(e33)  {
		if(e33->valuestring)  {
			strcpy(bootnotification_status, e33->valuestring);
			printf(" bootnotification status %s \n", bootnotification_status);
		}
	}
	cJSON_Delete(root);
}
void rec_status_notification ( int *number,char *snnumber,char *received_data)   {

	cJSON *root = cJSON_Parse(received_data);

	cJSON *e1 = cJSON_GetArrayItem(root, 0);
	if(e1)  {
		*number = e1->valueint;
		printf("sn id %d \n", *number);
	}

	cJSON *e3 = cJSON_GetArrayItem(root, 1);
	if(e3)  {
		if(e3->valuestring)  {
			strcpy(snnumber, e3->valuestring);
			printf(" sn rec number %s \n", snnumber);
		}
	}

	cJSON_Delete(root);

}
void rec_heartbeat(int *heartbeat_number,char *rec_uuid ,char *Heartbeat_Received_Time,char *received_data)  {
	printf(" rec heartbeat \n");
	cJSON *root = cJSON_Parse(received_data);
	cJSON *e1 = cJSON_GetArrayItem(root, 0);
	if(e1)  {
		*heartbeat_number = e1->valueint;
		printf(" HBnumber %d \n", *heartbeat_number);
	}
	cJSON *e2 = cJSON_GetArrayItem(root, 1);
	if(e2)  {
		if(e2->valuestring)  {
			strcpy(rec_uuid, e2->valuestring);
			printf(" rec_uuid = %s \n", rec_uuid);
		}

	}
	cJSON *e3 = cJSON_GetArrayItem(root, 2);
	cJSON *e31 = cJSON_GetObjectItem(e3, "currentTime");
	if(e31)  {
		if(e31->valuestring)  {
			strcpy(Heartbeat_Received_Time, e31->valuestring);
			printf("currentTime %s \n", Heartbeat_Received_Time);
		}
	}
	cJSON_Delete(root);
}
void rec_remotestart_transaction(char *remotestart_transaction_IdTag,int *ConnectorId,char *received_data,char *rs_accepted_serialnumber1)    {

	cJSON *root = cJSON_Parse(received_data);
	cJSON *e1 = cJSON_GetArrayItem(root, 1);
	cJSON *e2 = cJSON_GetArrayItem(root, 2);
	cJSON *e3 = cJSON_GetArrayItem(root, 3);
	if(e1)  {
		if(e1->valuestring)  {
			memset(rs_accepted_serialnumber,0,200);
			strcpy(rs_accepted_serialnumber1, e1->valuestring);
			printf(" rs_accepted_serialnumber = %s \n", rs_accepted_serialnumber1);
		}
	}
	if (e2) {
		if(e2->valuestring)  {
			memset(Task_RemoteStartTransaction,0,50);
			strcpy(Task_RemoteStartTransaction, e2->valuestring);
			printf(" id %s \n", Task_RemoteStartTransaction);
		}
	}
	cJSON *e31 = cJSON_GetObjectItem(e3, "idTag");
	if (e31) {
		if(e31->valuestring)  {
			strcpy(remotestart_transaction_IdTag, e31->valuestring);
			//strcpy(IdTag, e31->valuestring);
			printf(" RemoteStartTransaction idtag %s \n", remotestart_transaction_IdTag);
		}
	}
	cJSON *e32 = cJSON_GetObjectItem(e3, "connectorId");
	if (e32) {
		*ConnectorId = e32->valueint;
		printf(" RemoteStartTransaction ConnectorId %d \n", *ConnectorId);
	}
	cJSON_Delete(root);

}
void rec_start_transaction(int *transactionId_start_transaction,char *StartTransaction_Status,char *received_data)  {
	cJSON *root = cJSON_Parse(received_data);
	cJSON *e3 = cJSON_GetArrayItem(root, 2);
	cJSON *e31 = cJSON_GetObjectItem(e3, "transactionId");
	if(e31)  {
		*transactionId_start_transaction = e31->valueint;
		printf(" id %d \n", *transactionId_start_transaction);
	}
	cJSON *e32 = cJSON_GetObjectItem(e3, "idTagInfo");
	cJSON *e33 = cJSON_GetObjectItem(e32, "status");
	if(e33)  {
		if(e33->valuestring)  {
			strcpy(StartTransaction_Status, e33->valuestring);
			printf(" id %s \n", StartTransaction_Status);
		}
	}
	cJSON_Delete(root);
}
void rec_remotestop(char *Task_RemoteStopTransaction,int *transactionId_restop,char *recdata,char *rstop_accepted_serialnumber1)     {
	cJSON *root = cJSON_Parse(recdata);
	cJSON *e0 = cJSON_GetArrayItem(root, 0);
	cJSON *e1 = cJSON_GetArrayItem(root, 1);
	cJSON *e2 = cJSON_GetArrayItem(root, 2);
	cJSON *e3 = cJSON_GetArrayItem(root, 3);
	int num1 =0;

	if(e0)  {
		num1 =  e0->valueint;
		printf("data index 0 %d",num1);
	}
	if(e1)  {
		if(e1->valuestring)  {
			memset(rstop_accepted_serialnumber,0,200);
			strcpy(rstop_accepted_serialnumber1, e1->valuestring);
			printf(" rstop_accepted_serialnumber = %s \n", rstop_accepted_serialnumber1);
		}
	}

	if(e2)  {
		if(e2->valuestring)  {
			strcpy(Task_RemoteStopTransaction, e2->valuestring);
			printf(" RemoteStopTransaction %s \n", Task_RemoteStopTransaction);
		}
	}

	cJSON *e31 = cJSON_GetObjectItem(e3, "transactionId");
	if(e31)  {
		*transactionId_restop =  e31->valueint;
		printf(" transactionId2 %d \n", *transactionId_restop);
	}
	cJSON_Delete(root);

}
void rec_stoptransaction (char * StopTransaction_Status,char * recdata){
	cJSON *root = cJSON_Parse(recdata);
	cJSON *e2 = cJSON_GetArrayItem(root, 2);
	cJSON *e22 = cJSON_GetObjectItem(e2, "idTagInfo");
	cJSON *e221 = cJSON_GetObjectItem(e22, "status");
	if(e221){
		if(e221->valuestring){
			strcpy(StopTransaction_Status, e221->valuestring);
			printf(" id %s \n", StopTransaction_Status);
		}
	}
	cJSON_Delete(root);
}
void rec_metervalues(int *a2 ,char *snnumber,char *recdata)  {

	cJSON *root = cJSON_Parse(recdata);
	cJSON *e1 = cJSON_GetArrayItem(root, 0);
	if(e1){
		*a2 = e1->valueint;
		printf("a2 value %d \n", *a2);
	}
	cJSON *e3 = cJSON_GetArrayItem(root, 1);
	if(e3){
		if(e3->valuestring){
			strcpy(snnumber, e3->valuestring);
			printf(" sn rec number %s \n", snnumber);
		}
	}
	cJSON_Delete(root);

}
void send_bootnotification (int ocpp_uuidbn , char *chargePointModel ,char *chargePointVendor)  {
	printf( "Serialize.....");
	char temp_ocpp_uuid[100];
	itoa(ocpp_uuidbn,temp_ocpp_uuid,10);
	cJSON *root;
	root = cJSON_CreateArray();

	cJSON *element;

	// Add simple element

	element = cJSON_CreateNumber(2);
	cJSON_AddItemToArray(root, element);
	element = cJSON_CreateString(temp_ocpp_uuid);
	cJSON_AddItemToArray(root, element);
	element = cJSON_CreateString("BootNotification");
	cJSON_AddItemToArray(root, element);

	// Add object element
	element = cJSON_CreateObject();

	cJSON_AddStringToObject(element, "chargePointModel", chargePointModel);
	cJSON_AddStringToObject(element, "chargePointVendor", chargePointVendor);
	cJSON_AddItemToArray(root, element);
	char *bootsend = cJSON_Print(root);
	size_t length = strlen(bootsend);
	ESP_LOGI(TAG, "my_json_string\n%s",bootsend);


	printf(" sending boot notification \n");



	esp_err_t ret = esp_websocket_client_send_text(client, bootsend, length, portMAX_DELAY);
	if(ret == ESP_FAIL)
	{
		websocket_app_start();
	}
	cJSON_Delete(root);
	cJSON_free(bootsend);

}
void send_status_notification(int sn_uuid,int connectorId,char *errorCode,char *status,char *info,char *timestamp){
	printf("sending status notification \n");
	char temp_ocpp_uuid[100];
	itoa(sn_uuid,temp_ocpp_uuid,10);
	ESP_LOGI(TAG, "Serialize.....");
	cJSON *root;
	root = cJSON_CreateArray();

	cJSON *element;

	// Add simple element

	element = cJSON_CreateNumber(2);
	cJSON_AddItemToArray(root, element);

	element = cJSON_CreateString(temp_ocpp_uuid);
	cJSON_AddItemToArray(root, element);

	element = cJSON_CreateString("StatusNotification");
	cJSON_AddItemToArray(root, element);

	// Add object element
	element = cJSON_CreateObject();

	cJSON_AddNumberToObject(element, "connectorId", connectorId);
	cJSON_AddStringToObject(element, "errorCode", errorCode);
	cJSON_AddStringToObject(element, "status", status);
	cJSON_AddStringToObject(element, "info", info);
	cJSON_AddStringToObject(element, "timestamp", timestamp);
	cJSON_AddItemToArray(root, element);
	char *StatusNotification = cJSON_Print(root);
	ESP_LOGI(TAG, "my_json_string\n%s",StatusNotification);
	cJSON_Delete(root);
	// Buffers returned by cJSON_Print must be freed by the caller.
	// Please use the proper API (cJSON_free) rather than directly calling stdlib free.

	size_t length1 = strlen(StatusNotification);

	esp_err_t ret = esp_websocket_client_send_text(client, StatusNotification, length1, portMAX_DELAY);
	if(ret == ESP_FAIL)
		{
			websocket_app_start();
		}
	cJSON_free(StatusNotification);
}
void send_heartbeat(int heartbeat_uuid){

	printf( "Serialize.....");
	char temp_ocpp_uuid[100];
	itoa(heartbeat_uuid,temp_ocpp_uuid,10);
	cJSON *root;
	root = cJSON_CreateArray();
	cJSON *element;
	// Add simple element
	element = cJSON_CreateNumber(2);
	cJSON_AddItemToArray(root, element);
	element = cJSON_CreateString(temp_ocpp_uuid);
	cJSON_AddItemToArray(root, element);
	element = cJSON_CreateString("Heartbeat");
	cJSON_AddItemToArray(root, element);
	// Add object element
	element = cJSON_CreateObject();
	cJSON_AddItemToArray(root, element);
	char *heartbeat = cJSON_Print(root);
	ESP_LOGI(TAG, "my_json_string\n%s",heartbeat);
	printf(" sending Heartbeat \n");
	size_t length1 = strlen(heartbeat);
	esp_err_t ret = esp_websocket_client_send_text(client, heartbeat, length1, portMAX_DELAY);
	if(ret == ESP_FAIL)
		{
			websocket_app_start();
		}
	cJSON_Delete(root);
	cJSON_free(heartbeat);


}
void send_remotestart_transaction(char * rs_accepted_serialnumber1)    {

	printf("sending RemoteStartTransaction accepted \n");
	// char *rstartstr = "[3,\"6b231700-de2b-41b0-b0fe-0ab92f7c734b\",{\"status\":\"Accepted\"}]";
	printf( "Serialize.....");
	cJSON *root;
	root = cJSON_CreateArray();
	cJSON *element;
	// Add simple element
	element = cJSON_CreateNumber(3);
	cJSON_AddItemToArray(root, element);
	element = cJSON_CreateString(rs_accepted_serialnumber1);
	cJSON_AddItemToArray(root, element);
	// Add object element
	element = cJSON_CreateObject();
		cJSON_AddStringToObject(element, "status", "Accepted");

	cJSON_AddItemToArray(root, element);
	char *rstartstr = cJSON_Print(root);
	ESP_LOGI(TAG, "my_json_string\n%s",rstartstr);
	size_t length1 = strlen(rstartstr);
	esp_err_t ret = esp_websocket_client_send_text(client, rstartstr, length1, portMAX_DELAY);
	if(ret == ESP_FAIL)
		{
			websocket_app_start();
		}
	cJSON_Delete(root);
	cJSON_free(rstartstr);
}
void send_start_transaction(int start_transaction_uuid,int connectorId,int meterStart,char *timestamp,char *idTag_start_transaction)   {
	printf("sending StartTransaction  \n");
	printf( "Serialize.....");
	char temp_ocpp_uuid[100];
	itoa(start_transaction_uuid,temp_ocpp_uuid,10);
	cJSON *root;
	root = cJSON_CreateArray();
	cJSON *element;
	// Add simple element
	element = cJSON_CreateNumber(2);
	cJSON_AddItemToArray(root, element);
	element = cJSON_CreateString(temp_ocpp_uuid);
	cJSON_AddItemToArray(root, element);
	element = cJSON_CreateString("StartTransaction");
	cJSON_AddItemToArray(root, element);
	// Add object element
	element = cJSON_CreateObject();
	cJSON_AddNumberToObject(element, "connectorId", connectorId);
	cJSON_AddNumberToObject(element, "meterStart", meterStart);
	cJSON_AddStringToObject(element, "timestamp", timestamp);
	cJSON_AddStringToObject(element, "idTag", idTag_start_transaction);
	cJSON_AddItemToArray(root, element);
	char *StartTransaction = cJSON_Print(root);
	ESP_LOGI(TAG, "my_json_string\n%s",StartTransaction);
	cJSON_Delete(root);
	// Buffers returned by cJSON_Print must be freed by the caller.
	// Please use the proper API (cJSON_free) rather than directly calling stdlib free.
	size_t length1 = strlen(StartTransaction);
	esp_err_t ret = esp_websocket_client_send_text(client, StartTransaction, length1, portMAX_DELAY);
	if(ret == ESP_FAIL)
		{
			websocket_app_start();
		}
	cJSON_free(StartTransaction);

}
void send_metervalues(int metervalues_uuid,char *timestamp ,int *metervalues_transactionId,float voltage ,float current ,float kWh_meter)    {

	/*
	gcvt (float value, int ndigits, char * buf);

	float value : It is the float or double value.
	int ndigits : It is number of digits.
	char * buf : It is character pointer, in this variable string converted value will be copied.
	*/

	printf("sending meter values\n");

	char voltage_buffer[20];
	char current_buffer[20];
	char kWh_meter_buffer[20];
	sprintf(voltage_buffer, "%f", voltage);
	sprintf(current_buffer, "%f", current);
	sprintf(kWh_meter_buffer, "%f", kWh_meter);


	char temp_ocpp_uuid[100];
	itoa(metervalues_uuid,temp_ocpp_uuid,10);
	ESP_LOGI(TAG, "Serialize.....");
	cJSON *root ,*root2,*root3;
	cJSON *element;
	root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "timestamp", timestamp);
	cJSON *array,*array2;
	array= cJSON_CreateArray();


	cJSON *object;


	object = cJSON_CreateObject();
	cJSON_AddStringToObject(object, "context", "Sample.Periodic");
	cJSON_AddStringToObject(object, "location", "Outlet");
	cJSON_AddStringToObject(object, "measurand", "Energy.Active.Import.Register");
	cJSON_AddStringToObject(object, "unit","kWh");
	cJSON_AddStringToObject(object, "value",kWh_meter_buffer);
	cJSON_AddItemToArray(array, object);

	object = cJSON_CreateObject();
	cJSON_AddStringToObject(object, "context", "Sample.Periodic");
	cJSON_AddStringToObject(object, "location", "Outlet");
	cJSON_AddStringToObject(object, "measurand", "Energy.Active.Import.Register");
	cJSON_AddStringToObject(object, "unit","V");
	cJSON_AddStringToObject(object, "value",voltage_buffer);
	cJSON_AddItemToArray(array, object);

	object = cJSON_CreateObject();
	cJSON_AddStringToObject(object, "context", "Sample.Periodic");
	cJSON_AddStringToObject(object, "location", "Outlet");
	cJSON_AddStringToObject(object, "measurand", "Energy.Active.Import.Register");
	cJSON_AddStringToObject(object, "unit","A");
	cJSON_AddStringToObject(object, "value",current_buffer);
	cJSON_AddItemToArray(array, object);
	cJSON_AddItemToObject(root, "sampledValue", array);

	root2 = cJSON_CreateObject();
	cJSON_AddNumberToObject(root2, "connectorId", 1);
	array2= cJSON_CreateArray();
	cJSON_AddItemToArray(array2, root);
	cJSON_AddItemToObject(root2, "meterValue", array2);
	cJSON_AddNumberToObject(root2, "transactionId", *metervalues_transactionId);


	root3 = cJSON_CreateArray();
	element = cJSON_CreateNumber(2);
	cJSON_AddItemToArray(root3, element);
	element = cJSON_CreateString(temp_ocpp_uuid);
	cJSON_AddItemToArray(root3, element);
	element = cJSON_CreateString("MeterValues");
	cJSON_AddItemToArray(root3, element);
	cJSON_AddItemToArray(root3, root2);
	char *metervalues = cJSON_Print(root3);
	ESP_LOGI(TAG, "my_json_string\n%s",metervalues);
	cJSON_Delete(root3);
	size_t length1 = strlen(metervalues);

	esp_err_t ret = esp_websocket_client_send_text(client, metervalues, length1, portMAX_DELAY);
	if(ret == ESP_FAIL)
		{
			websocket_app_start();
		}
	cJSON_free(metervalues);

}
void send_remotestop(char *rstop_accepted_serialnumber1)  {
	printf("sending RemoteStop accepted \n");
	printf( "Serialize.....");
	cJSON *root;
	root = cJSON_CreateArray();
	cJSON *element;
	// Add simple element
	element = cJSON_CreateNumber(3);
	cJSON_AddItemToArray(root, element);
	element = cJSON_CreateString(rstop_accepted_serialnumber1);
	cJSON_AddItemToArray(root, element);
	// Add object element
	element = cJSON_CreateObject();
	cJSON_AddStringToObject(element, "status", "Accepted");
	cJSON_AddItemToArray(root, element);
	char *snstr = cJSON_Print(root);
	ESP_LOGI(TAG, "my_json_string\n%s",snstr);
	size_t length1 = strlen(snstr);
	esp_err_t ret = esp_websocket_client_send_text(client, snstr, length1, portMAX_DELAY);
	if(ret == ESP_FAIL)
		{
			websocket_app_start();
		}
	cJSON_Delete(root);
	cJSON_free(snstr);
}
void send_stoptransaction (int stoptransaction_uuid,int meterStop,char *timestamp,int OCPP_TransactionId)   {
	ESP_LOGI(TAG, "Serialize.....");
	char temp_ocpp_uuid[100];
	itoa(stoptransaction_uuid,temp_ocpp_uuid,10);
	cJSON *root;
	root = cJSON_CreateArray();
	cJSON *element;
	// Add simple element
	element = cJSON_CreateNumber(2);
	cJSON_AddItemToArray(root, element);
	element = cJSON_CreateString(temp_ocpp_uuid);
	cJSON_AddItemToArray(root, element);
	element = cJSON_CreateString("StopTransaction");
	cJSON_AddItemToArray(root, element);
	// Add object element
	element = cJSON_CreateObject();

	cJSON_AddNumberToObject(element, "meterStop", meterStop);
	cJSON_AddStringToObject(element, "timestamp", timestamp);
	cJSON_AddNumberToObject(element, "transactionId",OCPP_TransactionId);
	cJSON_AddItemToArray(root, element);
	char *stoptransactionsend = cJSON_Print(root);
	size_t length = strlen(stoptransactionsend);
	ESP_LOGI(TAG, "my_json_string\n%s",stoptransactionsend);
	printf(" sending stop transaction \n");
	esp_err_t ret = esp_websocket_client_send_text(client, stoptransactionsend, length, portMAX_DELAY);
	if(ret == ESP_FAIL)
		{
			websocket_app_start();
		}
	cJSON_Delete(root);
	cJSON_free(stoptransactionsend);
}

void send_offline_transactions() {
	printf("send offline transaction function start\n");
	int mode_size = get_file_size("/spiffs/mode.txt");
	int mode_line = file_line_count("/spiffs/mode.txt",mode_size);
	char char_mode[200];
	read_nth_line("/spiffs/mode.txt", mode_line, char_mode);
	int int_mode = atoi(char_mode);
	if(int_mode == 1)  {
	   printf("mode %d = sending start transaction + stop transaction\n",int_mode);
	}
	else if(int_mode == 2)  {
		   printf("mode %d = sending only stop transaction\n",int_mode);
	}
	int idtag_size = get_file_size("/spiffs/idtag.txt");
	int meterstart_size = get_file_size("/spiffs/meterstart.txt");
	int start_timestamp_size = get_file_size("/spiffs/start_timestamp.txt");
	printf("idtag_size =%d meterstart_size = %d start_timestamp_size =%d\n",idtag_size,meterstart_size,start_timestamp_size);

	int idtag_line = file_line_count("/spiffs/idtag.txt",idtag_size);
	int total_line_mode1 = idtag_line;
	int meterstart_line = file_line_count("/spiffs/idtag.txt",idtag_size);
	int start_timestamp_line = file_line_count("/spiffs/idtag.txt",idtag_size);
	printf("idtag total line =%d meterstart total line = %d  total start_timestamp_line =%d\n",idtag_line,meterstart_line,start_timestamp_line);

	int meterstop_size = get_file_size("/spiffs/meterstop.txt");
	int stop_timestamp_size = get_file_size("/spiffs/stop_timestamp.txt");
	printf("meterstop_size =%d stop_timestamp_size = %d \n",meterstop_size,stop_timestamp_size);

	int meterstop_line = file_line_count("/spiffs/meterstop.txt",meterstop_size);
	int stop_timestamp_line = file_line_count("/spiffs/stop_timestamp.txt",stop_timestamp_size);
	printf("meterstop total line =%d stop_timestamp total line = %d\n",meterstop_line,stop_timestamp_line);

	int transaction_id_size = get_file_size("/spiffs/transaction_id.txt");
	int transaction_id_line = file_line_count("/spiffs/transaction_id.txt", transaction_id_size);
	int total_line_mode2= transaction_id_line;
	printf("transaction id size = %d transaction id total line = %d \n",transaction_id_size,transaction_id_line);
    printf("after data adjustment \n");
	printf("meterstop total line =%d stop_timestamp total line = %d\n",meterstop_line,stop_timestamp_line);
    printf("All offline data\n");
	print_all_line("/spiffs/idtag.txt");
	print_all_line("/spiffs/meterstart.txt");
	print_all_line("/spiffs/start_timestamp.txt");
	print_all_line("/spiffs/meterstop.txt");
	print_all_line("/spiffs/stop_timestamp.txt");
	print_all_line("/spiffs/transaction_id.txt");
	printf("total_line_mode1 = %d\n",total_line_mode1);
	printf("total_line_mode2 = %d\n",total_line_mode2);
	bool temp_StartTransaction_recflag = false;
	bool temp_stop_Transaction_send_flag = false;
	bool temp_stop_Transaction_rec_flag = false;
	char temp_idtag[100];
	char temp_meterstart[100];
	char temp_meterstop[100];
	int temp_meterstart_integer =0;
	int temp_meterstop_integer =0;
	char temp_start_timestamp[100];
	char temp_stop_timestamp[100];
	char temp_StopTransaction_Status[100];
	int temp_transactionId =0;
	char char_transactionid[100];
	int transactionid_mode2 =0;
	char temp_StartTransaction_Status[100] ;
	if(int_mode == 1)  {
		 for(int i =0 ; i <total_line_mode1 ;i++)  {
			 vTaskDelay(100 / portTICK_RATE_MS);
			 read_nth_line("/spiffs/idtag.txt", idtag_line, temp_idtag);
			 read_nth_line("/spiffs/meterstart.txt", meterstart_line, temp_meterstart);
			 read_nth_line("/spiffs/start_timestamp.txt", start_timestamp_line, temp_start_timestamp);
			 temp_meterstart_integer = atoi(temp_meterstart);
			 ocpp_uuid++;
			 send_start_transaction(ocpp_uuid,1, temp_meterstart_integer,"2023-02-02T18:21:15Z", temp_idtag);
			 temp_StartTransaction_recflag = true;
			 while(temp_StartTransaction_recflag)  {
				vTaskDelay(100 / portTICK_RATE_MS);
				rec_start_transaction(&temp_transactionId, temp_StartTransaction_Status, recdata);
				elapsed_time++;
				if(strcmp(temp_StartTransaction_Status, "Accepted")==0)  {
					vTaskDelay(100 / portTICK_RATE_MS);
					temp_StartTransaction_recflag = false;
					temp_stop_Transaction_send_flag = true;
					memset(recdata, 0, 600);
					memset(temp_StartTransaction_Status, 0, 100);
					printf("  StartTransaction Accepted \n");
					elapsed_time = 0;
				}
				if(elapsed_time >= timeout) {
			     printf("Timed out waiting for start transaction status to be accepted\n");
				elapsed_time = 0;
			    break;
					}
			 }
			 while(temp_stop_Transaction_send_flag)  {
				 vTaskDelay(100 / portTICK_RATE_MS);
				 read_nth_line("/spiffs/meterstop.txt", meterstop_line, temp_meterstop);
				 read_nth_line("/spiffs/stop_timestamp.txt", stop_timestamp_line, temp_stop_timestamp);
				 temp_meterstop_integer = atoi(temp_meterstop);
				 ocpp_uuid++;
				 send_stoptransaction(ocpp_uuid,temp_meterstop_integer,"2023-02-02T18:21:15Z", temp_transactionId);
				 temp_stop_Transaction_send_flag = false;
				 temp_stop_Transaction_rec_flag = true;
			}
			 while(temp_stop_Transaction_rec_flag){
				 vTaskDelay(100 / portTICK_RATE_MS);
				rec_stoptransaction(temp_StopTransaction_Status,recdata);
				elapsed_time ++;
				if(strcmp(temp_StopTransaction_Status, "Accepted")==0)  {
					memset(recdata, 0, 600);
					memset(temp_StopTransaction_Status, 0, 100);
					printf("  Stop transaction Accepted \n");
					temp_stop_Transaction_rec_flag = false;
					elapsed_time = 0;
				}
				if(elapsed_time >= timeout) {
									   printf("Timed out waiting for stop transaction status to be accepted mode 1\n");
									   elapsed_time = 0;
									   break;
									}

			 }
			 delete_line("/spiffs/idtag.txt",idtag_line,"/spiffs/temp.txt");
			 delete_line("/spiffs/meterstart.txt",meterstart_line,"/spiffs/temp.txt");
			 delete_line("/spiffs/start_timestamp.txt",start_timestamp_line,"/spiffs/temp.txt");
			 delete_line("/spiffs/meterstop.txt",meterstop_line,"/spiffs/temp.txt");
			 delete_line("/spiffs/stop_timestamp.txt",stop_timestamp_line,"/spiffs/temp.txt");
			 idtag_line--;
			 meterstart_line--;
			 start_timestamp_line--;
			 meterstop_line--;
			 stop_timestamp_line--;
			 printf("line number after deleted\n");
			 printf("idtag_line = %d meterstart_line = %d start_timestamp_line = %d meterstop_line= %d stop_timestamp_line= %d\n",idtag_line,meterstart_line,start_timestamp_line,meterstop_line,stop_timestamp_line);
		 }
	}
	else if(int_mode == 2)   {
		 for(int i =0 ; i <total_line_mode2 ;i++)  {
			ocpp_uuid++;
			vTaskDelay(100 / portTICK_RATE_MS);
			read_nth_line("/spiffs/transaction_id.txt", transaction_id_line, char_transactionid);
			transactionid_mode2 = atoi(char_transactionid);
			read_nth_line("/spiffs/meterstop.txt", meterstop_line, temp_meterstop);
			temp_meterstop_integer = atoi(temp_meterstop);
			read_nth_line("/spiffs/stop_timestamp.txt", stop_timestamp_line, temp_stop_timestamp);
			printf("beforesending tid = %d",transactionid_mode2);
			send_stoptransaction(ocpp_uuid,temp_meterstop_integer, "2023-02-02T18:21:15Z", transactionid_mode2);
			temp_stop_Transaction_send_flag = false;
			temp_stop_Transaction_rec_flag = true;
			while(temp_stop_Transaction_rec_flag) {
					vTaskDelay(100 / portTICK_RATE_MS);
					rec_stoptransaction(temp_StopTransaction_Status,recdata);
					elapsed_time++;
				if(strcmp(temp_StopTransaction_Status, "Accepted")==0)  {
					memset(recdata, 0, 600);
					memset(temp_StopTransaction_Status, 0, 100);
					printf("  Stop transaction Accepted \n");
					temp_stop_Transaction_rec_flag = false;
					elapsed_time = 0;
				}
			  if(elapsed_time >= timeout) {
					   printf("Timed out waiting for stop transaction status to be accepted mode 2\n");
					   elapsed_time = 0;
					   break;
					}

			}
			 delete_line("/spiffs/meterstop.txt",meterstop_line,"/spiffs/temp.txt");
			 delete_line("/spiffs/stop_timestamp.txt",stop_timestamp_line,"/spiffs/temp.txt");
			 delete_line("/spiffs/transaction_id.txt",stop_timestamp_line,"/spiffs/temp.txt");
			 meterstop_line--;
			 stop_timestamp_line--;
			 transaction_id_line--;
			 printf("line number after deleted\n");
			 printf("meterstop_line = %d stop_timestamp_line = %d transaction_id_line= %d\n",meterstop_line,stop_timestamp_line,transaction_id_line);
	    }
	}
	else {
		printf("Offline transactions not available \n");
	}
	printf("Send offline transactions completed \n");
	vTaskDelay(100 / portTICK_RATE_MS);
	idtag_size = get_file_size("/spiffs/idtag.txt");
	meterstart_size = get_file_size("/spiffs/meterstart.txt");
	start_timestamp_size = get_file_size("/spiffs/start_timestamp.txt");
	printf("idtag_size =%d meterstart_size = %d start_timestamp_size =%d\n",idtag_size,meterstart_size,start_timestamp_size);
	meterstop_size = get_file_size("/spiffs/meterstop.txt");
	stop_timestamp_size = get_file_size("/spiffs/stop_timestamp.txt");
	printf("meterstop_size =%d stop_timestamp_size = %d \n",meterstop_size,stop_timestamp_size);
	delete_file("/spiffs/idtag.txt");
	delete_file("/spiffs/meterstart.txt");
	delete_file("/spiffs/start_timestamp.txt");
	delete_file("/spiffs/meterstop.txt");
	delete_file("/spiffs/stop_timestamp.txt");
	delete_file("/spiffs/transaction_id.txt");
	delete_file("/spiffs/mode.txt");
	printf("send offline transaction function end\n");
}
