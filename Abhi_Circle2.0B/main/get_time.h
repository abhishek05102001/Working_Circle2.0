#ifndef GET_TIME_H_
#define GET_TIME_H_


#include <sys/time.h>
#include "string.h"
#include "esp_log.h"
#include "esp_sntp.h"

void time_sync_notification_cb(struct timeval *tv);\
void Get_current_date_time(char *date_time);
void initialize_sntp(void);
void obtain_time(void);
void Set_SystemTime_SNTP(void);

#endif /*GET_TIME_H_ */
