#ifndef OCPP_H
#define OCPP_H

#include "cJSON.h"
#include "variables.h"
#include "stdio.h"
#include "esp_websocket_client.h"
#include "spiffs.h"
#include "driver/gpio.h"
#include "led_color.h"

void websocket_app_start(void);
void websocket_app_stop(void);
void rec_bootnotification(char *rec_uuid ,char *currentTime,char *bootnotification_status ,char *received_data);
void rec_status_notification ( int *number,char *snnumber,char *received_data);
void rec_heartbeat(int *heartbeat_number,char *rec_uuid ,char *Heartbeat_Received_Time,char *received_data);
void rec_remotestart_transaction(char *remotestart_transaction_IdTag,int *ConnectorId,char *received_data,char *rs_accepted_serialnumber1);
void rec_start_transaction(int *transactionId_start_transaction,char *StartTransaction_Status,char *received_data);
void rec_remotestop(char *Task_RemoteStopTransaction,int *transactionId_restop,char *recdata,char *rstop_accepted_serialnumber1);
void rec_stoptransaction (char * StopTransaction_Status,char * recdata);
void rec_metervalues(int *a2 ,char *snnumber,char *recdata);
void send_bootnotification (int ocpp_uuidbn , char *chargePointModel ,char *chargePointVendor);
void send_status_notification(int sn_uuid,int connectorId,char *errorCode,char *status,char *info,char *timestamp);
void send_heartbeat(int heartbeat_uuid);
void send_remotestart_transaction(char * rs_accepted_serialnumber1);
void send_start_transaction(int start_transaction_uuid,int connectorId,int meterStart,char *timestamp,char *idTag_start_transaction);
void send_metervalues(int metervalues_uuid,char *timestamp ,int *metervalues_transactionId,float voltage ,float current ,float kWh_meter);
void send_remotestop(char *rstop_accepted_serialnumber1);
void send_stoptransaction (int stoptransaction_uuid,int meterStop,char *timestamp,int OCPP_TransactionId);
void send_offline_transactions();

#endif /* OCPP_H */
