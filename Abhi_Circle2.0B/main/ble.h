#ifndef BLE_H_
#define BLE_H_

#include "variables.h"
#include "stdint.h"
#include "string.h"
#include "driver/gpio.h"
#include "esp_nimble_hci.h"
#include "get_time.h"
#include "spiffs.h"
#include "file_handling.h"
#include "ssid.h"
#include "nimble/nimble_port.h"
#include "services/gap/ble_svc_gap.h"
#include "host/ble_hs.h"


void ble_mode(void);
int device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);
int device_read(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);
int ble_gap_event(struct ble_gap_event *event, void *arg);
void ble_app_advertise(void);
void ble_app_on_sync(void);
void host_task(void *param);

#endif /* BLE_H_ */
