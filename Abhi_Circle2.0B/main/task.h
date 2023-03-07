/*
 * tasks.h
 *
 *  Created on: 11-Feb-2023
 *      Author: statiq163
 */

#ifndef TASKS_H_
#define TASKS_H_

#include "variables.h"
#include "stdio.h"
#include "string.h"
#include "ocpp.h"
#include "get_time.h"
#include "esp_adc_cal.h"


void voltageTask(void* pvParameters);
void currentTask(void* pvParameters);
void task1(void *arg);
void task2(void *arg);
void task3(void *arg);
void task4(void *arg);
void task5(void *arg);


#endif /* MAIN_TASKS_H_ */
