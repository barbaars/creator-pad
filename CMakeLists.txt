cmake_minimum_required(VERSION 3.15)

set(CMAKE_TOOLCHAIN_FILE toolchain-arm-none-eabi.cmake)

project(remote_pad VERSION 1 LANGUAGES C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

set(SRC_DIR "src")

file(WRITE ${SRC_DIR}/version.c
    "const char version[]=\"V${CMAKE_PROJECT_VERSION}\";\n"
)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/${SRC_DIR}/library/Device/Nuvoton/NUC123/Source/gcc/gcc_arm.ld)
set(CMAKE_EXE_LINKER_FLAGS "-T${LINKER_SCRIPT} -Xlinker --gc-sections -Wl,-Map,${PROJECT_NAME}.map --specs=nano.specs" CACHE INTERNAL "")

file(GLOB SRC_STD_DRIVER "${SRC_DIR}/library/StdDriver/src/*.c")


add_executable(${PROJECT_NAME}
    ${SRC_DIR}/library/Device/Nuvoton/NUC123/Source/system_NUC123.c
    ${SRC_DIR}/library/Device/Nuvoton/NUC123/Source/gcc/startup_NUC123.S
    ${SRC_DIR}/library/Device/Nuvoton/NUC123/Source/gcc/_syscalls.c
    ${SRC_STD_DRIVER}
    ${SRC_DIR}/version.c
    ${SRC_DIR}/main.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${SRC_DIR}/library/CMSIS/Include
    ${SRC_DIR}/library/Device/Nuvoton/NUC123/Include
    ${SRC_DIR}/library/StdDriver/inc
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ARGS -O binary -S ${PROJECT_NAME} ${PROJECT_NAME}.bin
)
