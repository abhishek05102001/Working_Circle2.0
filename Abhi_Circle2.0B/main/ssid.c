#include "ssid.h"


static const char *TAG = "SSID";
#define WHITE 0
#define RED 1
#define GREEN 2
#define BLUE 3
#define ORANGE 4
#define PINK 5
#define YELLOW 6

char SSID[2][33];
char PASSWORD[2][65];
char URL[50];
char PORT[50];
char OCPP_ID[50];
char BLE_RAW_DATA[50];
void read_ssid(){
	ESP_LOGI(TAG, "Reading ssid file");
	char tempssid1[100];
	char tempssid2[100];
	read_nth_line("/spiffs/ssid.txt", 1, tempssid1);
	tempssid1[strlen(tempssid1)-1] = '\0';
	strcpy(SSID[0],tempssid1);
	read_nth_line("/spiffs/ssid.txt", 2, tempssid2);
	tempssid2[strlen(tempssid2)-1] = '\0';
	strcpy(SSID[1],tempssid2);
	//password
	ESP_LOGI(TAG, "Reading password file");
	char temppass1[100];
	char temppass2[100];
	read_nth_line("/spiffs/password.txt", 1, temppass1);
	temppass1[strlen(temppass1)-1] = '\0';
	strcpy(PASSWORD[0],temppass1);
	read_nth_line("/spiffs/password.txt", 2, temppass2);
	temppass2[strlen(temppass2)-1] = '\0';
	strcpy(PASSWORD[1],temppass2);
}

void read_url(){
	ESP_LOGI(TAG, "Reading Complete URL");
	char tempurl[100];
    char tempport[100];
    char tempid[100];
    read_nth_line("/spiffs/URL.txt", 1, tempurl);
    tempurl[strlen(tempurl)-1] = '\0';
    strcpy(URL,tempurl);
    read_nth_line("/spiffs/URL.txt", 2, tempport);
    tempport[strlen(tempport)-1] = '\0';
    strcpy(PORT,tempport);
    read_nth_line("/spiffs/URL.txt", 3, tempid);
    tempid[strlen(tempid)-1] = '\0';
    strcpy(OCPP_ID,tempid);
}


void set_ssid()   {

	char ss1[100];//ssid 1
	char pp1[100];  //password 1
	char ss2[100];//ssid 2
	char pp2[100];  //password 2

	if ((strncmp(BLE_RAW_DATA, "ssid1", 5)==0)) {

		int j = 6;
		int k = 0;
		for( k = 0 ; k < strlen(BLE_RAW_DATA);k++) {
			ss1[k] = BLE_RAW_DATA[j];
			j++;
		}
		printf("writing ssid data to file =  %s\n",ss1);
		write_file("/spiffs/ssid.txt", ss1);
		ESP_LOGI(TAG, "ssid written = %s",ss1);
		LED_COLOR(YELLOW);
		vTaskDelay(500 / portTICK_RATE_MS);
		LED_COLOR(WHITE);
		vTaskDelay(500 / portTICK_RATE_MS);
		LED_COLOR(YELLOW);

	}
	if((strncmp(BLE_RAW_DATA, "ssid2", 5)==0))
	{

		        int j = 6;
				int k = 0;
				for( k = 0 ; k < strlen(BLE_RAW_DATA);k++) {
					ss2[k] = BLE_RAW_DATA[j];
					j++;
				}
				printf("writing ssid data to file =  %s\n",ss2);
				write_file("/spiffs/ssid.txt", ss2);
				ESP_LOGI(TAG, "ssid written = %s",ss2);
				LED_COLOR(YELLOW);
				vTaskDelay(500 / portTICK_RATE_MS);
				LED_COLOR(WHITE);
				vTaskDelay(500 / portTICK_RATE_MS);
				LED_COLOR(YELLOW);
	}
	if ((strncmp(BLE_RAW_DATA, "pass1", 5)==0))   {
		int j = 6;
		int k = 0;
		for( k = 0 ; k < strlen(BLE_RAW_DATA);k++) {
			pp1[k] = BLE_RAW_DATA[j];
			j++;
		}
		printf("writing password data to file %s\n",pp1);
		write_file("/spiffs/password.txt", pp1);
		ESP_LOGI(TAG, "password written");
		LED_COLOR(YELLOW);
		vTaskDelay(500 / portTICK_RATE_MS);
		LED_COLOR(WHITE);
		vTaskDelay(500 / portTICK_RATE_MS);
		LED_COLOR(YELLOW);
		esp_restart();
	}
	if ((strncmp(BLE_RAW_DATA, "pass2", 5)==0))   {
			int j = 6;
			int k = 0;
			for( k = 0 ; k < strlen(BLE_RAW_DATA);k++) {
				pp2[k] = BLE_RAW_DATA[j];
				j++;
			}
			printf("writing password data to file %s\n",pp2);
			write_file("/spiffs/password.txt", pp2);
			ESP_LOGI(TAG, "password written");
			LED_COLOR(YELLOW);
			vTaskDelay(500 / portTICK_RATE_MS);
			LED_COLOR(WHITE);
			vTaskDelay(500 / portTICK_RATE_MS);
			LED_COLOR(YELLOW);
			esp_restart();
	}
}

void set_url()   {

	char url2[100];
    char port2[100];
	char id2[100];
	if ((strncmp(BLE_RAW_DATA, "url", 3)==0))   {
				int j = 4;
				int k = 0;
				for( k = 0 ; k < strlen(BLE_RAW_DATA);k++) {
					url2[k] = BLE_RAW_DATA[j];
					j++;
				}
				printf("writing url data to file %s\n",url2);
				write_file("/spiffs/URL.txt", url2);
				ESP_LOGI(TAG, "url written");
				LED_COLOR(YELLOW);
				vTaskDelay(500 / portTICK_RATE_MS);
				LED_COLOR(WHITE);
				vTaskDelay(500 / portTICK_RATE_MS);
				LED_COLOR(YELLOW);
				esp_restart();
			}

	if ((strncmp(BLE_RAW_DATA, "port", 4)==0))   {
					int j = 5;
					int k = 0;
					for( k = 0 ; k < strlen(BLE_RAW_DATA);k++) {
						port2[k] = BLE_RAW_DATA[j];
						j++;
					}
					printf("writing port data to file %s\n",port2);
					write_file("/spiffs/URL.txt", port2);
					ESP_LOGI(TAG, "port written");
					LED_COLOR(YELLOW);
					vTaskDelay(500 / portTICK_RATE_MS);
					LED_COLOR(WHITE);
					vTaskDelay(500 / portTICK_RATE_MS);
					LED_COLOR(YELLOW);
					esp_restart();
				}
	if ((strncmp(BLE_RAW_DATA, "occpid", 6)==0))   {
					int j = 7;
					int k = 0;
					for( k = 0 ; k < strlen(BLE_RAW_DATA);k++) {
						id2[k] = BLE_RAW_DATA[j];
						j++;
					}
					printf("writing ID data to file %s\n",id2);
					write_file("/spiffs/URL.txt", id2);
					ESP_LOGI(TAG, "ID written");
					LED_COLOR(YELLOW);
					vTaskDelay(500 / portTICK_RATE_MS);
					LED_COLOR(WHITE);
					vTaskDelay(500 / portTICK_RATE_MS);
					LED_COLOR(YELLOW);
					esp_restart();
				}
}



