idf_component_register(SRCS "app_mesh.c" "ocpp.c" "spiffs.c" "led_color.c" "nvs.c" "variables.c" "get_time.c" "sd.c" "ble.c" "ssid.c" "task.c" "wifi_sta.c" "wifi_ap.c" "file_handling.c" 
                 INCLUDE_DIRS "."
                   EMBED_TXTFILES "server_certs/ca_cert.pem")