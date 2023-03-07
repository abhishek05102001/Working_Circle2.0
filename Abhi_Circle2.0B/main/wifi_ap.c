#include "wifi_ap.h"


#define WHITE 0
#define RED 1
#define GREEN 2
#define BLUE 3
#define ORANGE 4
#define PINK 5
#define YELLOW 6

char URL[50];
char PORT[50];
char OCPP_ID[50];
char SSID[2][33];
char PASSWORD[2][65];
static const char *TAG = "WIFI_AP";

#define EXAMPLE_ESP_WIFI_SSID      "STATIQ Set SSID and Password"
#define EXAMPLE_ESP_WIFI_PASS      "12345678"
#define EXAMPLE_ESP_WIFI_CHANNEL   1
#define EXAMPLE_MAX_STA_CONN       4

bool form_submitted = false;
bool form_connected = false;
bool form_disconnected = false;

esp_err_t root_handler(httpd_req_t *req)
{
 	esp_err_t error;
 	const char *response = (const char *) req->user_ctx;
 	error = httpd_resp_send(req, response, strlen(response));
 	if (error != ESP_OK)  {
 		ESP_LOGI(TAG, "Error %d while sending Response", error);
 	}
 	else  {
 		ESP_LOGI(TAG, "Response sent Successfully");
 	}
 	return error;
 }

static const httpd_uri_t root = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = root_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = "<!DOCTYPE html>\
    		<html>\
    		<head>\
    		<style>\
    		.button {\
    		  border: none;\
    		  color: white;\
    		  padding: 15px 32px;\
    		  text-align: center;\
    		  text-decoration: none;\
    		  display: inline-block;\
    		  font-size: 16px;\
    		  margin: 4px 2px;\
    		  cursor: pointer;\
    		}\
    		.button1 {background-color: #4CAF50;} /* Green */\
    		.button2 {background-color: #008CBA;} /* Blue */\
    		</style>\
    		</head>\
    		<body>\
    		<h1>STATIQ</h1>\
    		<p>Choose your Option:</p>\
    		<button class=\"button button1\" onclick= \"window.location.href='/QC'\" >QC</button>\
    		<button class=\"button button2\" onclick= \"window.location.href='/customer'\">Update SSID or PASSWORD</button>\
    		</body>\
    		</html>\
    		"
};


esp_err_t submitted_handler(httpd_req_t *req)  {
 	esp_err_t error;

 	const char *response = (const char *) req->user_ctx;
 	error = httpd_resp_send(req, response, strlen(response));
 	if (error != ESP_OK)  {
 		ESP_LOGI(TAG, "Error %d while sending Response", error);
 	}
 	else  {
 		ESP_LOGI(TAG, "Response sent Successfully");
 		form_submitted = true;
 	}
 	return error;
 }

const httpd_uri_t submitted = {
     .uri = "/submitted",
     .method = HTTP_GET,
     .handler = submitted_handler,
     .user_ctx = "<!DOCTYPE html>\
                   <html>\
                   <head>\
                       <title>Submitted</title>\
                   </head>\
                   <body>\
                       <h1 style='color:green;font-size:40px;'>&nbsp;&nbsp;SSID and Password Submitted Successfully</h1>\
                   </body>\
                   </html>"
 };

esp_err_t relayON_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG,"RELAY TURNED ON");
    gpio_set_level(15,0);
    const char *response = (const char *) req -> user_ctx;
    esp_err_t   error = httpd_resp_send(req, response, strlen(response));

       if(error != ESP_OK)
       {
       	ESP_LOGI(TAG,"Error %d while sending response", error);
       }
       else ESP_LOGI(TAG,"Response sent Successfully");

       return error;
}

const httpd_uri_t relayon = {
    .uri       = "/relayon",
    .method    = HTTP_GET,
    .handler   = relayON_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = "RELAY ON"
};

esp_err_t relayOFF_handler(httpd_req_t *req)
{

    ESP_LOGI(TAG,"RELAY TURNED OFF");
    gpio_set_level(15,1);
    const char *response = (const char *) req -> user_ctx;
       esp_err_t   error = httpd_resp_send(req, response, strlen(response));

          if(error != ESP_OK)
          {
          	ESP_LOGI(TAG,"Error %d while sending response", error);
          }
          else ESP_LOGI(TAG,"Response sent Successfully");

          return error;
}

const httpd_uri_t relayoff = {
    .uri       = "/relayoff",
    .method    = HTTP_GET,
    .handler   = relayOFF_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = "RELAY OFF"
};


esp_err_t servePage1_get_handler(httpd_req_t *req)  {
	  const char* html_page =
	  "<!DOCTYPE html>"
	  "<head>"
	  "<style>"
	  "form {display: grid;padding: 1em; background: #f9f9f9; border: 1px solid #c1c1c1; margin: 2rem auto 0 auto; max-width: 400px; padding: 1em;}}"
	  "form input {background: #fff;border: 1px solid #9c9c9c;}"
	  "form button {background: lightgrey; padding: 0.7em;width: 100%; border: 0;}"
	  "label {padding: 0.5em 0.5em 0.5em 0;}"
	  "input {padding: 0.7em;margin-bottom: 0.5rem;}"
	  "input:focus {outline: 10px solid gold;}"
	  "@media (min-width: 300px) {form {grid-template-columns: 200px 1fr; grid-gap: 16px;} label { text-align: right; grid-column: 1 / 2; } input, button { grid-column: 2 / 3; }}"
	  "</style>"
	  "</head>"
	  "<body>"
	  "<h1>QC FORM</h1>"
	  "<form class=\"form1\" id=\"loginForm1\" action=\"\">"
	  "<label for=\"URL\">URL</label>"
	  "<input id=\"url\" type=\"url\" name=\"url\">"
	  "<label for=\"port\">Port</label>"
	  "<input id=\"port\" type=\"text\" name=\"port\">"
	  "<label for=\"ocppId\">OCPP ID</label>"
	  "<input id=\"ocppId\" type=\"text\" name=\"ocppId\">"
	  "<label for=\"SSID1\">WiFi Name 1</label>"
	  "<input id=\"ssid1\" type=\"text\" name=\"ssid1\" maxlength=\"64\" minlength=\"4\">"
	  "<label for=\"Password1\">Password 1</label>"
	  "<input id=\"pwd1\" type=\"password\" name=\"pwd1\" maxlength=\"64\" minlength=\"4\">"
	  "<label for=\"SSID2\">WiFi Name 2</label>"
	  "<input id=\"ssid2\" type=\"text\" name=\"ssid2\" maxlength=\"64\" minlength=\"4\">"
	  "<label for=\"Password2\">Password 2</label>"
	  "<input id=\"pwd2\" type=\"password\" name=\"pwd2\" maxlength=\"64\" minlength=\"4\">"
	  "<button class=\"button button1\" onclick= \"window.location.href='/submitted'\">Submit</button>"
	  "</form>"
	  "<script>"
	  "document.getElementById(\"loginForm1\").addEventListener(\"submit\", (e) => {e.preventDefault(); const formData = new FormData(e.target); const data = Array.from(formData.entries()).reduce((memo, pair) => ({...memo, [pair[0]]: pair[1],  }), {}); var xhr = new XMLHttpRequest(); xhr.open(\"POST\", \"http://192.168.4.1/connection1\", true); xhr.setRequestHeader('Content-Type', 'application/json'); xhr.send(JSON.stringify(data)); document.getElementById(\"output\").innerHTML = JSON.stringify(data);});"
	  "</script>"
	  "<button class=\"button button1\" onclick= \"window.location.href='/relayon'\">Check Relay ON</button>"
	  "<button class=\"button button1\" onclick= \"window.location.href='/relayoff'\">Check Relay OFF</button>"
      "</body></html>";
   httpd_resp_send(req, html_page, strlen(html_page));
   return ESP_OK;
};

const httpd_uri_t QC = {
    .uri = "/QC",
    .method = HTTP_GET,
    .handler = servePage1_get_handler,
    .user_ctx = NULL
};

esp_err_t servePage2_get_handler(httpd_req_t *req)  {

	  const char* html_page =
	  "<!DOCTYPE html>"
	  "<head>"
	  "<style>"
	  "form {display: grid;padding: 1em; background: #f9f9f9; border: 1px solid #c1c1c1; margin: 2rem auto 0 auto; max-width: 400px; padding: 1em;}}"
	  "form input {background: #fff;border: 1px solid #9c9c9c;}"
	  "form button {background: lightgrey; padding: 0.7em;width: 100%; border: 0;}"
	  "label {padding: 0.5em 0.5em 0.5em 0;}"
	  "input {padding: 0.7em;margin-bottom: 0.5rem;}"
	  "input:focus {outline: 10px solid gold;}"
	  "@media (min-width: 300px) {form {grid-template-columns: 200px 1fr; grid-gap: 16px;} label { text-align: right; grid-column: 1 / 2; } input, button { grid-column: 2 / 3; }}"
	  "</style>"
	  "</head>"
	  "<body>"
	  "<h1>Update</h1>"
	  "<form class=\"form1\" id=\"loginForm1\" action=\"\">"
	  "<label for=\"SSID1\">WiFi Name 1</label>"
	  "<input id=\"ssid1\" type=\"text\" name=\"ssid1\" maxlength=\"64\" minlength=\"4\">"
	  "<label for=\"Password1\">Password 1</label>"
	  "<input id=\"pwd1\" type=\"password\" name=\"pwd1\" maxlength=\"64\" minlength=\"4\">"
	  "<label for=\"SSID2\">WiFi Name 2</label>"
	  "<input id=\"ssid2\" type=\"text\" name=\"ssid2\" maxlength=\"64\" minlength=\"4\">"
	  "<label for=\"Password2\">Password 2</label>"
	  "<input id=\"pwd2\" type=\"password\" name=\"pwd2\" maxlength=\"64\" minlength=\"4\">"
	  "<button class=\"button button1\" onclick= \"window.location.href='/submitted'\">Submit</button>"
	  "</form>"
	  "<script>"
	  "document.getElementById(\"loginForm1\").addEventListener(\"submit\", (e) => {e.preventDefault(); const formData = new FormData(e.target); const data = Array.from(formData.entries()).reduce((memo, pair) => ({...memo, [pair[0]]: pair[1],  }), {}); var xhr = new XMLHttpRequest(); xhr.open(\"POST\", \"http://192.168.4.1/connection2\", true); xhr.setRequestHeader('Content-Type', 'application/json'); xhr.send(JSON.stringify(data)); document.getElementById(\"output\").innerHTML = JSON.stringify(data);});"
	  "</script>"
      "</body></html>";
   httpd_resp_send(req, html_page, strlen(html_page));
   return ESP_OK;
};

const httpd_uri_t customer = {
    .uri = "/customer",
    .method = HTTP_GET,
    .handler = servePage2_get_handler,
    .user_ctx = NULL
};


 esp_err_t psw_ssid_get_handler1(httpd_req_t *req)
 {
     char buf[256];
     int ret, remaining = req->content_len;
     while (remaining > 0) {
         /* Read the data for the request */
         if ((ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)))) <= 0) {
             if (ret == 0) {
                 ESP_LOGI(TAG, "No content received please try again ...");
             }
             else if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                 /* Retry receiving if timeout occurred */
                 continue;
             }
             return ESP_FAIL;
         }

         /* Log data received */
         ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
         ESP_LOGI(TAG, "%.*s", ret, buf);
         ESP_LOGI(TAG, "====================================");

         cJSON *root = cJSON_Parse(buf);
         sprintf(SSID[0], "%s", cJSON_GetObjectItem(root, "ssid1")->valuestring);
         sprintf(PASSWORD[0], "%s", cJSON_GetObjectItem(root, "pwd1")->valuestring);
         ESP_LOGI(TAG, "ssid1: %s", SSID[0]);
         ESP_LOGI(TAG, "pwd1: %s", PASSWORD[0]);
         NVS_Write("ssid1", SSID[0]);
         NVS_Write("password1", PASSWORD[0]);

         sprintf(SSID[1], "%s", cJSON_GetObjectItem(root, "ssid2")->valuestring);
         sprintf(PASSWORD[1], "%s", cJSON_GetObjectItem(root, "pwd2")->valuestring);
         ESP_LOGI(TAG, "ssid2: %s", SSID[1]);
         ESP_LOGI(TAG, "pwd2: %s", PASSWORD[1]);
         NVS_Write("ssid2", SSID[1]);
         NVS_Write("password2", PASSWORD[1]);

         sprintf(URL, "%s", cJSON_GetObjectItem(root, "url")->valuestring);
         sprintf(PORT, "%s", cJSON_GetObjectItem(root, "port")->valuestring);
         sprintf(OCPP_ID, "%s", cJSON_GetObjectItem(root, "ocppid")->valuestring);
         ESP_LOGI(TAG, "URL: %s", URL);
         ESP_LOGI(TAG, "PORT: %s", PORT);
         ESP_LOGI(TAG, "OCPP ID: %s", OCPP_ID);
         NVS_Write("url", URL);
         NVS_Write("port", PORT);
         NVS_Write("ocppid", OCPP_ID);

         remaining -= ret;
     }

     /* End response */
     httpd_resp_send_chunk(req, NULL, 0);
     LED_COLOR(WHITE);
     vTaskDelay(1000 / portTICK_RATE_MS);
     LED_COLOR(YELLOW);
     vTaskDelay(1000 / portTICK_RATE_MS);
     LED_COLOR(WHITE);
     vTaskDelay(1000 / portTICK_RATE_MS);
     LED_COLOR(YELLOW);

     printf("Restarting device\n");
     vTaskDelay(2000 / portTICK_RATE_MS);
     //esp_restart();

     return ESP_OK;
 }

 const httpd_uri_t psw_ssid1 = {
    .uri = "/connection1",
    .method = HTTP_POST,
    .handler = psw_ssid_get_handler1,
    .user_ctx = "TEST1"
};

 esp_err_t psw_ssid_get_handler2(httpd_req_t *req)
 {
     char buf[256];
     int ret, remaining = req->content_len;
     while (remaining > 0) {
         /* Read the data for the request */
         if ((ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)))) <= 0) {
             if (ret == 0) {
                 ESP_LOGI(TAG, "No content received please try again ...");
             }
             else if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                 /* Retry receiving if timeout occurred */
                 continue;
             }
             return ESP_FAIL;
         }

         /* Log data received */
         ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
         ESP_LOGI(TAG, "%.*s", ret, buf);
         ESP_LOGI(TAG, "====================================");

         cJSON *root = cJSON_Parse(buf);
         sprintf(SSID[0], "%s", cJSON_GetObjectItem(root, "ssid1")->valuestring);
         sprintf(PASSWORD[0], "%s", cJSON_GetObjectItem(root, "pwd1")->valuestring);
         ESP_LOGI(TAG, "ssid1: %s", SSID[0]);
         ESP_LOGI(TAG, "pwd1: %s", PASSWORD[0]);
         NVS_Write("ssid1", SSID[0]);
         NVS_Write("password1", PASSWORD[0]);

         sprintf(SSID[1], "%s", cJSON_GetObjectItem(root, "ssid2")->valuestring);
         sprintf(PASSWORD[1], "%s", cJSON_GetObjectItem(root, "pwd2")->valuestring);
         ESP_LOGI(TAG, "ssid2: %s", SSID[1]);
         ESP_LOGI(TAG, "pwd2: %s", PASSWORD[1]);
         NVS_Write("ssid2", SSID[1]);
         NVS_Write("password2", PASSWORD[1]);


         remaining -= ret;
     }

     /* End response */
     httpd_resp_send_chunk(req, NULL, 0);
     //LED_COLOR(ORANGE);
     vTaskDelay(1000 / portTICK_RATE_MS);
     printf("Restarting device\n");
     vTaskDelay(2000 / portTICK_RATE_MS);
     //esp_restart();

     return ESP_OK;
 }

 const httpd_uri_t psw_ssid2 = {
    .uri = "/connection2",
    .method = HTTP_POST,
    .handler = psw_ssid_get_handler2,
    .user_ctx = "TEST2"
};


 esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)  {
     /* For any other URI send 404 and close socket */
     httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
     return ESP_FAIL;
 }

httpd_handle_t start_webserver(void)  {
	httpd_handle_t server = NULL;
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	// Start the httpd server
	ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
	if (httpd_start(&server, &config) == ESP_OK)  {
		// Set URI handlers
		ESP_LOGI(TAG, "Registering URI handlers");
		httpd_register_uri_handler(server, &root);
		httpd_register_uri_handler(server, &QC);
		httpd_register_uri_handler(server, &customer);
		httpd_register_uri_handler(server, &psw_ssid1);
		httpd_register_uri_handler(server, &psw_ssid2);
		httpd_register_uri_handler(server, &relayon);
		httpd_register_uri_handler(server, &relayoff);
		httpd_register_uri_handler(server, &submitted);
		return server;
	}
	ESP_LOGI(TAG, "Error starting server!");
	return NULL;
 }

 void stop_webserver(httpd_handle_t server)  {
     // Stop the httpd server
	 httpd_stop(server);
 }

 void connect_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data)  {
     httpd_handle_t* server = (httpd_handle_t*) arg;
     if (*server == NULL) {
         ESP_LOGI(TAG, "Starting webserver");
         *server = start_webserver();
     }
 }

 void disconnect_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data)  {
     httpd_handle_t* server = (httpd_handle_t*) arg;
     if (*server) {
         ESP_LOGI(TAG, "Stopping webserver");
         stop_webserver(*server);
         *server = NULL;
     }
 }

void wifi_event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data)  {

	if (event_id == WIFI_EVENT_AP_STACONNECTED) {
		 		wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
		 		ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
		 		MAC2STR(event->mac), event->aid);
		 		form_connected = true;
		     }
		     else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
		 		wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
		 		ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
		 		MAC2STR(event->mac), event->aid);
		 		form_disconnected = true;
		     }


 }

void wifi_init_softap(void)
{
    //ESP_ERROR_CHECK(esp_netif_init());
    //ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg1 = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg1));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config1 = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .channel = EXAMPLE_ESP_WIFI_CHANNEL,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config1.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config1));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
    EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS, EXAMPLE_ESP_WIFI_CHANNEL);
}



