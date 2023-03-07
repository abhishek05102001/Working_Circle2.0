#include "wifi_sta.h"


#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
char SSID[2][33];
char PASSWORD[2][65];

static const char *TAG = "WIFI_STA";
EventGroupHandle_t wifi_event_group;
static volatile int retry_count = 0;
static volatile int current_ssid = 0;


extern volatile struct SendFlags sendFlags;
//extern volatile struct RecvFlags recvFlags;

void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    	printf("WIFI started...\n");
        //esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
    	//printf("INITIAL COUNT = %d \n",retry_count);
        if (retry_count < 10) {

            ESP_LOGI(TAG, "Retrying to connect to Network");
            printf("COUNT is %d \n",retry_count+1);
            retry_count++;
            esp_wifi_connect();
            vTaskDelay(1000 / portTICK_RATE_MS);
            Internet_connected = false;

        } else {

            retry_count = 0;
            xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
            esp_wifi_disconnect();
            vTaskDelay(2000 / portTICK_RATE_MS);
            current_ssid = (current_ssid + 1) % 2;
            wifi_init_sta(SSID[current_ssid], PASSWORD[current_ssid]);
            Internet_connected = false;
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
        ESP_LOGI(TAG, "Got ip: " IPSTR, IP2STR(&event->ip_info.ip));
        retry_count = 0;
        current_ssid = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        sendFlags.BootNotification_sendflag = true;
    }
}

void wifi_init_sta(const char* ssid, const char* password)
{
    ESP_LOGI(TAG, "Connecting to SSID %s with password %s", ssid, password);
    wifi_config_t wifi_config2 = {
        .sta = {
            .ssid = "",
            .password = "",
        },
    };
    memset(&wifi_config2, 0, sizeof(wifi_config_t));
    strcpy((char*)wifi_config2.sta.ssid, ssid);
    strcpy((char*)wifi_config2.sta.password, password);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config2) );
    ESP_ERROR_CHECK(esp_wifi_start() );
    esp_wifi_connect();
}

void wifi_init()
{
	wifi_event_group = xEventGroupCreate();
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_create_default_wifi_sta();
	wifi_init_config_t cfg2 = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg2));
	esp_event_handler_instance_t instance_any_id;
	esp_event_handler_instance_t instance_got_ip;
	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
															ESP_EVENT_ANY_ID,
															event_handler,
															NULL,
															&instance_any_id));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
															IP_EVENT_STA_GOT_IP,
															event_handler,
															NULL,
															&instance_got_ip));

	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
						WIFI_EVENT_STA_DISCONNECTED,
						event_handler,
						NULL,
						NULL));


}






