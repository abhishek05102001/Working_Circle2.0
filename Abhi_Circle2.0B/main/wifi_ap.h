/*
 * wifi_ap.h
 *
 *  Created on: 15-Feb-2023
 *      Author: statiq163
 */

#ifndef WIFI_AP_H_
#define WIFI_AP_H_

#include "variables.h"
#include "esp_wifi.h"
#include "esp_http_server.h"
#include "cJSON.h"
#include "esp_log.h"
#include "led_color.h"
#include "nvs.h"
#include <sys/param.h>


esp_err_t submitted_handler(httpd_req_t *req);
esp_err_t servePage_get_handler(httpd_req_t *req);
esp_err_t psw_ssid_get_handler(httpd_req_t *req);
httpd_handle_t start_webserver(void);
void stop_webserver(httpd_handle_t server);
void disconnect_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data);
void connect_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data) ;
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err);
void wifi_event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data);
void wifi_init_softap(void);


#endif /* MAIN_WIFI_AP_H_ */
