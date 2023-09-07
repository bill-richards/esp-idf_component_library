FILE(GLOB_RECURSE SOURCES *.c)

idf_component_register(SRCS ${SOURCES}
                       PRIV_REQUIRES driver
                       INCLUDE_DIRS include
)
