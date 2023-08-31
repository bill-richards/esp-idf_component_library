FILE(GLOB_RECURSE SOURCES *.c)

set(REQUIRED_COMPONENTS 
        driver
        esp_includes
        freertos
        newlib
)

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS include
                    REQUIRES ${REQUIRED_COMPONENTS})
