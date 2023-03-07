/*
 * nvs.h
 *
 *  Created on: 08-Feb-2023
 *      Author: statiq163
 */

#ifndef NVS_H_
#define NVS_H_

#include "esp_err.h"
#include "nvs_flash.h"
#include "string.h"
void NVS_Read(char *data,char *Get_Data);
void NVS_Write(char *data,char *write_string);


#endif /*NVS_H_ */
