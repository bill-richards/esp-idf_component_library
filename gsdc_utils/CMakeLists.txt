SET(app_sources
    gsdc_string_utils.c
)

idf_component_register(SRCS ${app_sources}
                       INCLUDE_DIRS "include"
)