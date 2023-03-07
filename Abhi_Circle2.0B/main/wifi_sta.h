/*
 * wifi.h
 *
 *  Created on: 14-Feb-2023
 *      Author: statiq163
 */

#ifndef WIFI_H_
#define WIFI_H_

#include "string.h"
#include "esp_log.h"
#include "variables.h"
#include "esp_wifi.h"
#include "freertos/event_groups.h"

void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
void wifi_init();
void wifi_init_sta(const char* ssid, const char* password);



#endif /* MAIN_WIFI_H_ */
