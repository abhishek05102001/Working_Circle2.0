#ifndef SPIFFS_H_
#define SPIFFS_H_

#include "esp_spiffs.h"
#include <sys/stat.h>
#include <sys/unistd.h>
#include "file_handling.h"
#include "esp_log.h"

int SPIFFS_INIT();
void delete_file_spiffs(const char* file_name);
void save_online_stop_transactions(int transactionId ,int meterstop,char *timestamp);
void delete_file(const char* file_name);
void offline_save_session_stop(char *meterstop,char *stop_timestamp);
void offline_save_session_start(char *idtag,int meterstart,char *timestamp);

#endif /* SPIFFS_H_ */
