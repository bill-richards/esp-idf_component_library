FILE(GLOB_RECURSE SOURCES *.cpp)

SET(REQUIRED_COMPONENTS 
    gsdc_bme280
    gsdc_cppiic
    gsdc_utils
    esp_includes
)

idf_component_register(SRCS ${SOURCES}
    INCLUDE_DIRS include
    REQUIRES ${REQUIRED_COMPONENTS}
)
