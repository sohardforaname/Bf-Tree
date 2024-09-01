add_executable(TestBFTree
        miniPageTest.cc
)

set_target_properties(TestBFTree PROPERTIES
        COMPILE_FLAGS "-Werror"
)
