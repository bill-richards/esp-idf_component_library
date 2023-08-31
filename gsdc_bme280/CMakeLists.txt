FILE(GLOB_RECURSE SOURCES *.cpp)

SET(REQUIRED_COMPONENTS 
    gsdc_telemetry
)

idf_component_register(SRCS ${SOURCES}
                        INCLUDE_DIRS include
                        REQUIRES ${REQUIRED_COMPONENTS}
)
