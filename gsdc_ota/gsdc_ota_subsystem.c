// //////////////////////////////////////////// //
//                DEPENDENCIES                  //
// //////////////////////////////////////////// //
#include <freertos/FreeRTOS.h>                  //
#include <esp_ota_ops.h>                        //
#include <esp_logging.h>                        //
#include <nvs_flash.h>                          //
#include "gsdc_ota.h"							//
#include "gsdc_ota_subsystem.h"					//
// //////////////////////////////////////////// //

static const char * OTA_SUBSYSTEM_TAG = "ota-subsystem";

void gsdc_ota_subsystem_initialize(uint8_t iicAddress)
{
    char ssid[32];
    sprintf(ssid, "%s-%x", OTA_SUBSYSTEM_TAG, iicAddress);

	esp_err_t ret = nvs_flash_init();

	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}

	ESP_ERROR_CHECK(ret);
	ESP_ERROR_CHECK(gsdc_ota_configure_wifi(ssid));
	ESP_ERROR_CHECK(http_server_init());
	ESP_LOGW(OTA_SUBSYSTEM_TAG, "Wi-Fi SSID: %s", ssid);

	/* Mark current app as valid */
	const esp_partition_t *partition = esp_ota_get_running_partition();
	ESP_LOGW(OTA_SUBSYSTEM_TAG, "Currently running partition: %s\r\n", partition->label);

	esp_ota_img_states_t ota_state;
	if (esp_ota_get_state_partition(partition, &ota_state) == ESP_OK) {
		if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
			esp_ota_mark_app_valid_cancel_rollback();
		}
	}
}
