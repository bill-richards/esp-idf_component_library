FILE(GLOB_RECURSE SOURCES *.c)

SET(embeded_files
        include/web/index.html
        include/web/identity.html
)

SET(REQUIRED_COMPONENTS
        gsdc_utils
        configuration_file
        esp_includes
        esp_http_server
        app_update
        nvs_flash
        esp_wifi
        freertos
)

idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS include
                       REQUIRES ${REQUIRED_COMPONENTS}
                       EMBED_TXTFILES ${embeded_files}
)
