//Abhishek
#include "variables.h"
#include "task.h"
#include "ble.h"
#include "wifi_sta.h"
#include "wifi_ap.h"
#include "sd.h"
#include "services/gatt/ble_svc_gatt.h"
#include "nimble/nimble_port_freertos.h"

#define Relay_On 1
#define Relay_Off 0
#define WIFI_CONNECTED_BIT BIT0
#define ORANGE 4
#define PINK 5
#define VOLTAGE_PIN ADC1_CHANNEL_7//33
#define CURRENT_PIN ADC1_CHANNEL_6//34

extern volatile xQueueHandle voltage_queue;
extern volatile xQueueHandle current_queue;
extern char URL[50];
extern char PORT[50];
extern char OCPP_ID[50];
extern char result[100];
extern volatile EventGroupHandle_t wifi_event_group;
extern volatile bool form_submitted;
extern volatile bool form_connected;
extern volatile bool form_disconnected;
extern char SSID[2][33];
extern char PASSWORD[2][65];
extern char Current_Date_Time[100];

struct SendFlags sendFlags;
struct RecvFlags recvFlags;

static const char *TAG = "MAIN";
static const struct ble_gatt_svc_def gatt_svcs[] = {
	    {.type = BLE_GATT_SVC_TYPE_PRIMARY,
	     .uuid = BLE_UUID16_DECLARE(0x180),                 // Define UUID for device type
	     .characteristics = (struct ble_gatt_chr_def[]) {
	         {.uuid = BLE_UUID16_DECLARE(0xFEF4),           // Define UUID for reading
	          .flags = BLE_GATT_CHR_F_READ,
	          .access_cb = device_read
	         },
	         {.uuid = BLE_UUID16_DECLARE(0xDEAD),           // Define UUID for writing
	          .flags = BLE_GATT_CHR_F_WRITE,
	          .access_cb = device_write
	         },
	         {0}}},{0}
};
static TaskHandle_t Task1Handle = NULL;
static TaskHandle_t Task2Handle = NULL;
static TaskHandle_t Task3Handle = NULL;
static TaskHandle_t Task4Handle = NULL;
static TaskHandle_t Task5Handle = NULL;
static esp_adc_cal_characteristics_t adc1_chars;

void app_main()
{
	voltage_queue = xQueueCreate(10, sizeof(float));
	current_queue = xQueueCreate(10, sizeof(float));
	static httpd_handle_t server = NULL;
	gpio_reset_pin(15);
	gpio_set_direction(15, GPIO_MODE_OUTPUT);
	gpio_set_level(15, 1);

	printf("\n MAIN START \n");
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	SPIFFS_INIT();
	initialize_sd();
	wifi_init();
	RGB_LED_INIT();
	printf("Create hotspot \n");
	wifi_init_softap();
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, &connect_handler, &server));
	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));
	int retry_count = 0;
    while (retry_count <= 29)
	{
         LED_COLOR(ORANGE);
         vTaskDelay(1000 / portTICK_RATE_MS);
         if (form_connected == true)
         {
        	 	 if(form_submitted == true || form_disconnected == true){
        	 		ESP_LOGI(TAG, "FORM SUBMITTED or Disconnected");
        	 		break;
        	 	 }
         }
         else if(form_connected == false)
         {
        	retry_count++;
            ESP_LOGI(TAG, "Waiting for WiFi connection... (%d)", retry_count);
         }
	}
	if (retry_count == 30)
	{
				 LED_COLOR(ORANGE);
			     ESP_LOGI(TAG, "WiFi connection timed out. Turning off access point.");
			     esp_wifi_stop();
	}

	ESP_ERROR_CHECK(ret);
	set_ssid();
	read_ssid();
	set_url();
    read_url();
	NVS_Read("ssid1", SSID[0]);
	NVS_Read("password1", PASSWORD[0]);
	printf("ssid 1 stored in NVS:  %s\n",SSID[0]);
	printf("password 1 stored  in NVS: %s\n",PASSWORD[0]);
	NVS_Read("ssid2", SSID[1]);
	NVS_Read("password2", PASSWORD[1]);
	printf("ssid 2 stored in NVS:  %s\n",SSID[1]);
	printf("password 2 stored  in NVS: %s\n",PASSWORD[1]);
	NVS_Read("url", URL);
	NVS_Read("port", PORT);
	NVS_Read("ocppid", OCPP_ID);
	printf("URL stored in NVS:  %s\n",URL);
	printf("PORT stored  in NVS: %s\n",PORT);
	printf("ID stored  in NVS: %s\n",OCPP_ID);
	LED_COLOR(PINK);
	strcat(result, URL);
	strcat(result, ":");
	strcat(result, PORT);
	strcat(result, "/");
	strcat(result, OCPP_ID);
	printf("Result: %s\n", result);
	esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_10, 0, &adc1_chars);
	ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_10));
	ESP_ERROR_CHECK(adc1_config_channel_atten(CURRENT_PIN, ADC_ATTEN_DB_11));
	ESP_ERROR_CHECK(adc1_config_channel_atten(VOLTAGE_PIN, ADC_ATTEN_DB_11));
	vTaskDelay(500 / portTICK_RATE_MS);
	esp_nimble_hci_and_controller_init();      // 2 - Initialize ESP controller
	nimble_port_init();                        // 3 - Initialize the host stack
	ble_svc_gap_device_name_set("BLE-Server"); // 4 - Initialize NimBLE configuration - server name
	ble_svc_gap_init();                        // 4 - Initialize NimBLE configuration - gap service
	ble_svc_gatt_init();                       // 4 - Initialize NimBLE configuration - gatt service
	ble_gatts_count_cfg(gatt_svcs);            // 4 - Initialize NimBLE configuration - config gatt services
	ble_gatts_add_svcs(gatt_svcs);             // 4 - Initialize NimBLE configuration - queues gatt services.
	ble_hs_cfg.sync_cb = ble_app_on_sync;      // 5 - Initialize application
	nimble_port_freertos_init(host_task);      // 6 - Run the thread
	vTaskDelay(500 / portTICK_RATE_MS);
	if ((strlen(SSID[0]) > 3 && strlen(PASSWORD[0])>7) && (strlen(SSID[1]) > 3 && strlen(PASSWORD[1])>7))
	{
		wifi_init_sta(SSID[0],PASSWORD[0]);
	}
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));
	printf("MAIN END \n");
	xTaskCreatePinnedToCore(task4, "task4", 4096, NULL, 7, &Task4Handle,1);//kWh calibration
	xTaskCreatePinnedToCore(task3, "task3", 4096, NULL, 7, &Task3Handle,0);//Energy Meter ADC
	xTaskCreatePinnedToCore(voltageTask, "VoltageTask", 2048, NULL, 1, NULL, 1);
    xEventGroupWaitBits(wifi_event_group,WIFI_CONNECTED_BIT,pdFALSE,pdFALSE,portMAX_DELAY);
    {
				Set_SystemTime_SNTP();
			    vTaskDelay(500 / portTICK_RATE_MS);
				websocket_app_start();
				//LED_COLOR(ORANGE);
				sendFlags.BootNotification_sendflag = true;
				sendFlags.send_offline_transactions_flag = true;
				Get_current_date_time(Current_Date_Time);
				printf("current date and time is = %s\n", Current_Date_Time);
				xTaskCreatePinnedToCore(task1, "task1", 4096, NULL, 7, &Task1Handle,0);//OCPP Send Functions
				xTaskCreatePinnedToCore(task2, "task2", 4096, NULL, 7, &Task2Handle,1);//OCPP Heartbeat & Meter Values
				xTaskCreatePinnedToCore(task5, "task5", 4096, NULL, 7, &Task5Handle,0);// RGB LED Status
				xTaskCreatePinnedToCore(currentTask, "CurrentTask", 2048, NULL, 1, NULL, 1);
    }


}
