FILE(GLOB_RECURSE SOURCES *.c)

SET(REQUIRED_COMPONENTS
        spiffs
        esp_includes
        esp_common
)


idf_component_register(SRCS ${SOURCES}
                        INCLUDE_DIRS include
                        REQUIRES ${REQUIRED_COMPONENTS})
