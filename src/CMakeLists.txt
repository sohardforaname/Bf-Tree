add_library(BFTree STATIC
        miniPage.cc
)

set_target_properties(BFTree PROPERTIES
        COMPILE_FLAGS "-Wall"
)