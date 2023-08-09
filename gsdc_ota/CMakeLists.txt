SET(app_sources
        gsdc_ota.c
        gsdc_ota_subsystem.c
)

SET(embeded_files
        include/web/index.html
        include/web/identity.html
)

SET(components
        gsdc_utils
        configuration_file
        esp_includes
        esp_http_server
        app_update
        nvs_flash
        esp_wifi
        freertos
)

#set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)

idf_component_register(SRCS ${app_sources}
                       INCLUDE_DIRS "include"
                       REQUIRES ${components}
                       EMBED_TXTFILES ${embeded_files}
)
