FILE(GLOB_RECURSE SOURCES *.c)

set(REQUIRED_COMPONENTS 
        log
        driver
        configuration_file
        esp_includes
        led_controller
        freertos
)

idf_component_register(SRCS ${SOURCES}
                        INCLUDE_DIRS include
                        REQUIRES ${REQUIRED_COMPONENTS})
