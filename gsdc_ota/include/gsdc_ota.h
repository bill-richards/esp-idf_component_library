#ifndef __GSDC_OTA_H__
#define __GSDC_OTA_H__

#include <esp_err.h>
#include <configuration_file.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef gsdc_configuration_file_descriptor_t gsdc_ota_configuration_file_descriptor_t;
typedef gsdc_configuration_file_t gsdc_ota_configuration_file_t;

/**
 * @brief Internal use. Starts teh STA mode wi-fi which is used to enable OTA
 * 
 * @param ssid (char *) the SSID to assign this wi-fi network
 */
esp_err_t gsdc_ota_configure_wifi(char * ssid);

// @brief Internal use. Initializes the http server
esp_err_t gsdc_ota_http_server_init(gsdc_ota_configuration_file_t * configurationFile);

/**
 * @brief Internal use. Saves the binary image to the next OTA partition
 * 
 * @param image (char *) the new firmware image
 * @param length (size_t) the size of the new image
 */
esp_err_t gsdc_ota_save_firmware_image(const char * image, size_t length);

#ifdef __cplusplus
}
#endif

#endif // __GSDC_OTA_H__