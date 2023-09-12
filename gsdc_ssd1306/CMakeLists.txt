FILE(GLOB_RECURSE SOURCES *.c *.cpp)

set(REQUIRED_COMPONENTS 
        driver
        freertos
)

idf_component_register(SRCS ${SOURCES}
                       PRIV_REQUIRES ${REQUIRED_COMPONENTS}
                       INCLUDE_DIRS include
)
