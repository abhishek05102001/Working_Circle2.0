#include "spiffs.h"
static const char *TAG = "SPIFFS";

int SPIFFS_INIT()  {
	 ESP_LOGI(TAG, "Initializing SPIFFS");
	    esp_vfs_spiffs_conf_t conf = {
	      .base_path = "/spiffs",
	      .partition_label = NULL,
	      .max_files = 5,
	      .format_if_mount_failed = true
	    };
	    // Use settings defined above to initialize and mount SPIFFS filesystem.
	    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
	    esp_err_t ret = esp_vfs_spiffs_register(&conf);
	    if (ret != ESP_OK) {
	        if (ret == ESP_FAIL) {
	            ESP_LOGE(TAG, "Failed to mount or format filesystem");
	        } else if (ret == ESP_ERR_NOT_FOUND) {
	            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
	        } else {
	            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
	        }
	        return 0;
	    }
	    size_t total = 0, used = 0;
	    ret = esp_spiffs_info(conf.partition_label, &total, &used);
	    if (ret != ESP_OK) {
	        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
	    } else {
	        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
	    }
	  return 0;
}

void delete_file_spiffs(const char* file_name)  {

    struct stat st;
      if (stat(file_name, &st) == 0) {
          // Delete it if it exists
          unlink(file_name);
          printf("deleted file =  %s ",file_name);
      }
}

void offline_save_session_start(char *idtag,int meterstart,char *timestamp)    {
	char meterstart_buffer[20];
	itoa(meterstart,meterstart_buffer,10);
	append_file("/spiffs/idtag.txt", idtag);
	append_file("/spiffs/meterstart.txt",meterstart_buffer);
	append_file("/spiffs/start_timestamp.txt",timestamp);
	append_file("/spiffs/meterstop.txt", "0000");
    append_file("/spiffs/stop_timestamp.txt","2000-05-21T17:02:43.000Z");
}


void offline_save_session_stop(char *meterstop,char *stop_timestamp)   {
	int temps1 = get_file_size("/spiffs/meterstop.txt");
	int templ1 = file_line_count("/spiffs/meterstop.txt", temps1);
	delete_line("/spiffs/meterstop.txt", templ1,"/spiffs/temp.txt");
	append_file("/spiffs/meterstop.txt", meterstop);

	int temps2 = get_file_size("/spiffs/stop_timestamp.txt");
	int templ2 = file_line_count("/spiffs/stop_timestamp.txt", temps2);
	delete_line("/spiffs/stop_timestamp.txt", templ2,"/spiffs/temp.txt");
	append_file("/spiffs/stop_timestamp.txt",stop_timestamp);

	//char transaction_id_buffer[20];
	//itoa(transaction_id,transaction_id_buffer,2);
	//append_file("/spiffs/transaction_id.txt",transaction_id_buffer);
}

void delete_file(const char* file_name)   {

	 // Check if destination file exists before renaming
	    struct stat st;
	    if (stat(file_name, &st) == 0) {
	        // Delete it if it exists
	        unlink(file_name);
	        printf("file deleted successfully %s\n",file_name);
	    }
	    else {
	    	printf("file not available = %s\n",file_name);
	    }
}
void save_online_stop_transactions(int transactionId ,int meterstop,char *timestamp)  {
	char temp_char_transactionId[100];
	itoa(transactionId,temp_char_transactionId,10);
	append_file("/spiffs/online_tid.txt", temp_char_transactionId);

	char temp_char_meterstop[100];
	itoa(meterstop,temp_char_meterstop,10);
	append_file("/spiffs/online_meterstop.txt", temp_char_meterstop);

	append_file("/spiffs/online_timestamp.txt", timestamp);

}

