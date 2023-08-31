SET(SOURCES gsdc_cppiic.cpp )


set(components driver)

idf_component_register(SRCS ${SOURCES}
    INCLUDE_DIRS include
    REQUIRES ${components}
)
