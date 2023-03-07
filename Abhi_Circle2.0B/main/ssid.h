/*
 * ssid.h
 *
 *  Created on: 08-Feb-2023
 *      Author: statiq163
 */

#ifndef SSID_H_
#define SSID_H_

#include "string.h"
#include "stdio.h"
#include "led_color.h"
#include "esp_log.h"
#include "file_handling.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void read_ssid(void);
void set_ssid(void);
void read_url(void);
void set_url(void);

#endif /* MAIN_SSID_H_ */
