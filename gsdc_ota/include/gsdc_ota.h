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
esp_err_t http_server_init(gsdc_ota_configuration_file_t * configurationFile);


#ifdef __cplusplus
}
#endif

#endif // __GSDC_OTA_H__