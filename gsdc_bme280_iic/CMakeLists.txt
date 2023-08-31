SET(SOURCES gsdc_bme280_iic.cpp )

SET(REQUIRED_COMPONENTS 
    gsdc_bme280
    gsdc_cppiic
    esp_includes
)

idf_component_register(SRCS ${SOURCES}
    INCLUDE_DIRS include
    REQUIRES ${REQUIRED_COMPONENTS}
)
