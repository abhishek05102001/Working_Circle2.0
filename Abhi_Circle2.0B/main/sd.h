/*
 * sd.h
 *
 *  Created on: 08-Feb-2023
 *      Author: statiq163
 */

#ifndef SD_H_
#define SD_H_

#include "sdmmc_cmd.h"
#include "esp_log.h"
#include "driver/spi_common.h"
#include "esp_vfs_fat.h"
#include <sys/stat.h>
#include <sys/unistd.h>

void initialize_sd(void);
void delete_file_sd(char *file_name);



#endif /* MAIN_SD_H_ */
