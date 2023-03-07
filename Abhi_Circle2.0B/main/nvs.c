#include "nvs.h"


void NVS_Read(char *data,char *Get_Data)  {
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
	ESP_ERROR_CHECK( err );
	// Open
	printf("\n");
	printf("Opening Non-Volatile Storage (NVS) handle... ");
	nvs_handle_t my_handle;
	err = nvs_open("storage", NVS_READWRITE, &my_handle);
	if (err != ESP_OK) {
		printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
	}
	else {
		printf("Done\n");
		size_t required_size = 0;
		nvs_get_str(my_handle, data, NULL, &required_size);
		char *server_name = malloc(required_size);
		err =   nvs_get_str(my_handle, data, server_name, &required_size);
		switch (err) {
			case ESP_OK: {
				printf("Done\n");
				// printf("Restart counter = %d\n", restart_counter);
				printf("Read data: %s\n", server_name);
				strcpy(Get_Data,server_name);
				break;
			}
			case ESP_ERR_NVS_NOT_FOUND:{
				printf("The value is not initialized yet!\n");
				break;
			}
			default :{
				printf("Error (%s) reading!\n", esp_err_to_name(err));
			}
		}

	}
	nvs_close(my_handle);
 }
 void NVS_Write(char *data,char *write_string)  {
 	 // Initialize NVS
 	    esp_err_t err = nvs_flash_init();
 	    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
 	        // NVS partition was truncated and needs to be erased
 	        // Retry nvs_flash_init
 	        ESP_ERROR_CHECK(nvs_flash_erase());
 	        err = nvs_flash_init();
 	    }
 	    ESP_ERROR_CHECK( err );
 	    // Open
 	    printf("\n");
 	    printf("Opening Non-Volatile Storage (NVS) handle... ");
 	    nvs_handle_t my_handle;
 	    err = nvs_open("storage", NVS_READWRITE, &my_handle);
 	    if (err != ESP_OK) {
 	        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
 	    }
 	    else {
 	    	//char *write_string = "Some string new...\0";
 			nvs_set_str(my_handle, data, write_string);
 			printf("Write data: %s\n", write_string);
 			printf("Committing updates in NVS ... ");
 			err = nvs_commit(my_handle);
 			printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
 	    }
 	    nvs_close(my_handle);
 }


