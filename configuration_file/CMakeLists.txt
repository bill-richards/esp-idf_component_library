FILE(GLOB_RECURSE SOURCES *.c)

SET(REQUIRED_COMPONENTS 
    spiffs_file
    esp_includes
)

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS include 
                    REQUIRES ${REQUIRED_COMPONENTS})
