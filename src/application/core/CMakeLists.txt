set(DIR_SRC "src")
set(DIR_INC "inc")
set(DIR_SRC_LIB "../../library")

add_library(core STATIC
    ${DIR_SRC}/scheduler.c
    ${DIR_SRC}/cyclic_tasks.c
    ${DIR_SRC}/programs.c
    ${DIR_SRC}/ttime.c
    ${DIR_SRC}/rbuff.c
    ${DIR_SRC}/shell.c
    ${DIR_SRC}/trace.c
    ${DIR_SRC}/pads.c
    ${DIR_SRC}/led_toggle.c
)

target_include_directories(core
    PUBLIC
    ${DIR_INC}
    PRIVATE
    "../drivers/inc"
)

add_subdirectory(${DIR_SRC_LIB}/lwshell/lwshell build)
target_link_libraries(core lwshell)
