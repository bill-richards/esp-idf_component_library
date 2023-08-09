// //////////////////////////////////////////// //
//                DEPENDENCIES                  //
// //////////////////////////////////////////// //
#include "gsdc_ota.h"							//
#include <string.h>								//
#include <ctype.h>								//
//                                              //
#include <freertos/FreeRTOS.h>					//
#include <esp_http_server.h>					//
#include <freertos/task.h>						//
#include <esp_ota_ops.h>						//
#include <esp_system.h>							//
#include <nvs_flash.h>							//
#include <sys/param.h>							//
#include <esp_wifi.h>							//
#include <esp_logging.h>						//
#include <configuration_file.h>					//
#include <gsdc_string_utils.h>					//
// //////////////////////////////////////////// //

// //////////////////////////////////////////////////////////////////////// //
//                       FORWARD DECLARATIONS                               //
// //////////////////////////////////////////////////////////////////////// //
//                                                                          //
esp_err_t internal_configuration_get_handler(httpd_req_t *req);				//
esp_err_t internal_index_get_handler(httpd_req_t *req);						//
esp_err_t internal_configuration_post_handler(httpd_req_t *req);			//
esp_err_t internal_ota_post_handler(httpd_req_t *req);						//
//                                                                          //
// //////////////////////////////////////////////////////////////////////// //

// //////////////////////////////////////////////////////////////////////////// //
//                      LOCAL VARIABLES                             			//
// //////////////////////////////////////////////////////////////////////////// //
//																				//
static const char * OTA_TAG = "ota-subsystem";									//
// Serve OTA update portal (index.html)											//
extern const uint8_t index_html_start[] asm("_binary_index_html_start");		//
extern const uint8_t index_html_end[] asm("_binary_index_html_end");			//
// Serve Configuration portal (identity.html)									//
extern const uint8_t identity_html_start[] asm("_binary_identity_html_start");	//
extern const uint8_t identity_html_end[] asm("_binary_identity_html_end");		//
//																				//
gsdc_ota_configuration_file_t * Configuration_File;								//
//																				//
httpd_uri_t internal_configuration_get = {										//
	.uri	  = "/identity",													//
	.method   = HTTP_GET,														//
	.handler  = internal_configuration_get_handler,								//
	.user_ctx = NULL															//
};																				//
httpd_uri_t internal_index_get = {												//
	.uri	  = "/",															//
	.method   = HTTP_GET,														//
	.handler  = internal_index_get_handler,										//
	.user_ctx = NULL															//
};																				//
httpd_uri_t internal_configuration_post = {										//
	.uri	  = "/identity",													//
	.method   = HTTP_POST,														//
	.handler  = internal_configuration_post_handler,							//
	.user_ctx = NULL															//
};																				//
httpd_uri_t internal_ota_post = {												//
	.uri	  = "/ota",															//
	.method   = HTTP_POST,														//
	.handler  = internal_ota_post_handler,										//
	.user_ctx = NULL															//
};																				//
//																				//
// //////////////////////////////////////////////////////////////////////////// //

esp_err_t internal_configuration_get_handler(httpd_req_t *req)
{
	size_t html_length = identity_html_end-identity_html_start;
	char * html = calloc(html_length, sizeof(char));
	memcpy(html, identity_html_start, html_length);

	config_key_value_pair_t pair;
	Configuration_File->get_configuration_item("IIC", &pair, Configuration_File);
	char * modified_html = gsdc_string_utils_replace_substring(html, "CONFIGIICADDRESS", pair.Value);
	free(html);

	if(Configuration_File->get_configuration_item("CLIENTS", &pair, Configuration_File)) {
		modified_html = gsdc_string_utils_replace_substring(modified_html, "CONFIGCLIENTLIST", pair.Value);
		modified_html = gsdc_string_utils_replace_substring(modified_html, "CONFIGISMASTER", "checked");
	} else {
		modified_html = gsdc_string_utils_replace_substring(modified_html, "CONFIGISMASTER", " ");
	}

	Configuration_File->get_configuration_item("SSID", &pair, Configuration_File);
	modified_html = gsdc_string_utils_replace_substring(modified_html, "CONFIGSSID", pair.Value);

	httpd_resp_send(req, modified_html, strlen(modified_html));
	free(modified_html);
	return ESP_OK;
}

esp_err_t internal_index_get_handler(httpd_req_t *req)
{
	httpd_resp_send(req, (const char *) index_html_start, index_html_end-index_html_start);
	return ESP_OK;
}

esp_err_t internal_configuration_post_handler(httpd_req_t *req)
{
	ESP_LOGI(OTA_TAG, "received POST of length %u to : %s", req->content_len, req->uri);

    char * tail;
    char * inner_tail;
	char * key = calloc(CONFIGURATION_FILE_MAXIMUM_KEY_LENGTH, sizeof(char));
	char * value = calloc(CONFIGURATION_FILE_MAXIMUM_VALUE_LENGTH, sizeof(char)); 

	char * head = strtok_r((char *)req->uri, "?", &tail);
	char * pair = strtok_r(tail, "&", &tail);

	while(pair != NULL)
	{
		head = strtok_r(pair, "=", &inner_tail);
		memcpy(key, head, strlen(head));

		// We have to store the pointer so that we can delete it immediately to avoid the possibillity of a stack overflow
		char * decoded = gsdc_string_utils_url_decode(inner_tail);
		memcpy(value, decoded, strlen(inner_tail));
		free(decoded);

		Configuration_File->set_configuration_item(key, value, Configuration_File);
		memset(key, '\0', CONFIGURATION_FILE_MAXIMUM_KEY_LENGTH);
		memset(value, '\0', CONFIGURATION_FILE_MAXIMUM_VALUE_LENGTH);

		pair = strtok_r(tail, "&", &tail);
	}
	free(key);
	free(value);

	Configuration_File->save_configuration(Configuration_File);
	httpd_resp_sendstr(req, "Configuration update complete, rebooting now!\n");
		
	vTaskDelay(500 / portTICK_PERIOD_MS);
	esp_restart();

	return ESP_OK;
}

esp_err_t internal_ota_post_handler(httpd_req_t *req)
{
	char buf[1000];
	esp_ota_handle_t ota_handle;
	int remaining = req->content_len;

	const esp_partition_t *ota_partition = esp_ota_get_next_update_partition(NULL);
	ESP_ERROR_CHECK(esp_ota_begin(ota_partition, OTA_SIZE_UNKNOWN, &ota_handle));

	while (remaining > 0) {
		int recv_len = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)));

		// Timeout Error: Just retry
		if (recv_len == HTTPD_SOCK_ERR_TIMEOUT) {
			continue;

		// Serious Error: Abort OTA
		} else if (recv_len <= 0) {
			httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Protocol Error");
			return ESP_FAIL;
		}

		// Successful Upload: Flash firmware chunk
		if (esp_ota_write(ota_handle, (const void *)buf, recv_len) != ESP_OK) {
			httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Flash Error");
			return ESP_FAIL;
		}

		remaining -= recv_len;
	}

	// Validate and switch to new OTA image and reboot
	if (esp_ota_end(ota_handle) != ESP_OK || esp_ota_set_boot_partition(ota_partition) != ESP_OK) {
			httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Validation / Activation Error");
			return ESP_FAIL;
	}

	httpd_resp_sendstr(req, "Firmware update complete, rebooting now!\n");

	vTaskDelay(500 / portTICK_PERIOD_MS);
	esp_restart();

	return ESP_OK;
}


// //////////////////////////////////////////////////////////////////////// //
//                       		PUBLIC API                               	//
// //////////////////////////////////////////////////////////////////////// //

esp_err_t gsdc_ota_configure_wifi(char * ssid)
{
	esp_err_t res = ESP_OK;

	res |= esp_netif_init();
	res |= esp_event_loop_create_default();
	esp_netif_create_default_wifi_ap();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	res |= esp_wifi_init(&cfg);

	wifi_config_t wifi_config = {
		.ap = {
			.ssid_len = strlen(ssid),
			.channel = 6,
			.authmode = WIFI_AUTH_OPEN,
			.max_connection = 3,
		},
	};
	memcpy(&wifi_config.ap.ssid, ssid, sizeof(char[32]));

	res |= esp_wifi_set_mode(WIFI_MODE_AP);
	res |= esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);
	res |= esp_wifi_start();

	return res;
}

esp_err_t gsdc_ota_http_server_init(gsdc_ota_configuration_file_t * configurationFile)
{
	Configuration_File = configurationFile;
	static httpd_handle_t http_server = NULL;

	httpd_config_t config = HTTPD_DEFAULT_CONFIG();

	if (httpd_start(&http_server, &config) == ESP_OK) {
		httpd_register_uri_handler(http_server, &internal_index_get);
		httpd_register_uri_handler(http_server, &internal_configuration_get);
		httpd_register_uri_handler(http_server, &internal_configuration_post);
		httpd_register_uri_handler(http_server, &internal_ota_post);
	}

	return http_server == NULL ? ESP_FAIL : ESP_OK;
}

esp_err_t gsdc_ota_save_firmware_image(const char * image, size_t length)
{
	esp_err_t result = ESP_OK;
	esp_ota_handle_t ota_handle;

	const esp_partition_t *ota_partition = esp_ota_get_next_update_partition(NULL);
	ESP_ERROR_CHECK(esp_ota_begin(ota_partition, OTA_SIZE_UNKNOWN, &ota_handle));

	result = esp_ota_write(ota_handle, (const void *)image, length);
	if (result != ESP_OK) {
		return result;
	}
	
	if (esp_ota_end(ota_handle) != ESP_OK || esp_ota_set_boot_partition(ota_partition) != ESP_OK) {
			return ESP_FAIL;
	}

	vTaskDelay(500 / portTICK_PERIOD_MS);
	esp_restart();
	return ESP_OK;
}