SET(SOURCES gsdc_bme280.cpp)

SET(REQUIRED_COMPONENTS 
    gsdc_telemetry
)

idf_component_register(SRCS "gsdc_bme280.cpp"
                        INCLUDE_DIRS include
                        REQUIRES ${REQUIRED_COMPONENTS}
)
