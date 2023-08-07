#include "gsdc_ota.h"
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <freertos/task.h>
#include <esp_ota_ops.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <esp_wifi.h>
#include <esp_logging.h>

/*
 * Serve OTA update portal (index.html)
 */
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

extern const uint8_t identity_html_start[] asm("_binary_identity_html_start");
extern const uint8_t identity_html_end[] asm("_binary_identity_html_end");

static const char * OTA_TAG = "ota-subsystem";


esp_err_t identity_get_handler(httpd_req_t *req)
{
	httpd_resp_send(req, (const char *) identity_html_start, identity_html_end-identity_html_start);
	return ESP_OK;
}

esp_err_t index_get_handler(httpd_req_t *req)
{
	httpd_resp_send(req, (const char *) index_html_start, index_html_end-index_html_start);
	return ESP_OK;
}

esp_err_t identity_post_handler(httpd_req_t *req)
{
	char * buff = (char *)calloc(1000, sizeof(char));
	int remaining = req->content_len;
	ESP_LOGI(OTA_TAG, "received POST of length %u to : %s", remaining, req->uri);


	while (remaining > 0) {
		int recv_len = httpd_req_recv(req, buff, MIN(remaining, strlen(buff)));

		// Timeout Error: Just retry
		if (recv_len == HTTPD_SOCK_ERR_TIMEOUT) {
			continue;

		// Serious Error: Abort OTA
		} else if (recv_len <= 0) {
			httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Protocol Error");
			return ESP_FAIL;
		}
		remaining -= recv_len;
	}

	// Successful Upload: do something with the sent data
	free(buff);
	httpd_resp_sendstr(req, "Configuration update complete, rebooting now!\n");
	
	vTaskDelay(500 / portTICK_PERIOD_MS);
	esp_restart();

	return ESP_OK;
}

/*
 * Handle OTA file upload
 */
esp_err_t ota_post_handler(httpd_req_t *req)
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

/*
 * HTTP Server
 */
httpd_uri_t index_get = {
	.uri	  = "/",
	.method   = HTTP_GET,
	.handler  = index_get_handler,
	.user_ctx = NULL
};

httpd_uri_t identity_get = {
	.uri	  = "/identity",
	.method   = HTTP_GET,
	.handler  = identity_get_handler,
	.user_ctx = NULL
};

httpd_uri_t identity_post = {
	.uri	  = "/identity",
	.method   = HTTP_POST,
	.handler  = identity_post_handler,
	.user_ctx = NULL
};

httpd_uri_t ota_post = {
	.uri	  = "/ota",
	.method   = HTTP_POST,
	.handler  = ota_post_handler,
	.user_ctx = NULL
};

esp_err_t http_server_init(void)
{
	static httpd_handle_t http_server = NULL;

	httpd_config_t config = HTTPD_DEFAULT_CONFIG();

	if (httpd_start(&http_server, &config) == ESP_OK) {
		httpd_register_uri_handler(http_server, &index_get);
		httpd_register_uri_handler(http_server, &identity_get);
		httpd_register_uri_handler(http_server, &identity_post);
		httpd_register_uri_handler(http_server, &ota_post);
	}

	return http_server == NULL ? ESP_FAIL : ESP_OK;
}

/*
 * WiFi configuration
 */
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
