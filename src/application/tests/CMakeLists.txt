cmake_minimum_required(VERSION 3.15)

project(sim)


add_subdirectory("../core" core)

set(TESTS OFF CACHE BOOL "Switch off CppUTest Test build")
add_subdirectory("../../library/cpputest" cpputest)

add_executable(sim 
    src/sim.c
    stubs/stub_analog_inputs.c
    fakes/fake_bootloader_run.c
    stubs/stub_keys.c
    stubs/stub_leds.c
    fakes/fake_systick.c
    stubs/stub_cdc_serial.c
    stubs/stub_critical_section.c
)

target_include_directories(sim PRIVATE
    "header_overrides"
    "../drivers/inc"
)

target_link_libraries(sim PRIVATE core)


add_executable(RunAllTests
    src/RunAllTests.cpp
    src/test_rbuff.cpp
    src/test_trace.cpp
    src/test_scheduler.cpp
    stubs/stub_systick.c
    stubs/stub_critical_section.c
)

target_include_directories(RunAllTests PRIVATE
    "header_overrides"
    "../drivers/inc"
)

target_link_libraries(RunAllTests PRIVATE core CppUTest CppUTestExt)

add_custom_command(
    TARGET RunAllTests
    POST_BUILD
    COMMAND RunAllTests
)
