cmake_minimum_required(VERSION 3.15)

set(CMAKE_TOOLCHAIN_FILE toolchain-arm-none-eabi.cmake)

cmake_policy(SET CMP0096 NEW) # Preserve leading zeros in version number
project(remote_pad VERSION 01.02 LANGUAGES C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

set(DIR_SRC_ROOT "src")
set(DIR_SRC_LIB "${DIR_SRC_ROOT}/library")

set(DIR_SRC_BSP "${DIR_SRC_LIB}/NUC123BSP")

set(DIR_INC_CMSIS "${DIR_SRC_BSP}/Library/CMSIS/Include")

set(DIR_DEVICE "${DIR_SRC_BSP}/Library/Device/Nuvoton/NUC123")
set(DIR_SRC_DEVICE "${DIR_DEVICE}/Source")
set(DIR_INC_DEVICE "${DIR_DEVICE}/Include")

set(DIR_STD_DRIVER "${DIR_SRC_BSP}/Library/StdDriver")
set(DIR_SRC_STD_DRIVER "${DIR_STD_DRIVER}/src")
set(DIR_INC_STD_DRIVER "${DIR_STD_DRIVER}/inc")
file(GLOB SRC_STD_DRIVER "${DIR_SRC_STD_DRIVER}/*.c")

set(DIR_CONFIGURATION "${DIR_SRC_ROOT}/configuration/configA")
set(DIR_SRC_CONFIGURATION "${DIR_CONFIGURATION}/src")
set(DIR_INC_CONFIGURATION "${DIR_CONFIGURATION}/inc")
file(GLOB SRC_CONFIGURATION "${DIR_SRC_CONFIGURATION}/*.c")

set(DIR_APPLICATION "${DIR_SRC_ROOT}/application")
set(APPLICATION_NAME "${PROJECT_NAME}")
file(WRITE ${DIR_APPLICATION}/version.c
    "const char app_name[16] __attribute__ ((section (\".version\"))) = \"${APPLICATION_NAME}\";\n"
    "const char app_version[16] __attribute__ ((section (\".version\"))) = \"V${PROJECT_VERSION_MAJOR}R${PROJECT_VERSION_MINOR}\";\n"
)

set(DIR_APPLICATION_CORE "${DIR_APPLICATION}/core")
set(DIR_SRC_APPLICATION_CORE "${DIR_APPLICATION_CORE}/src")
set(DIR_INC_APPLICATION_CORE "${DIR_APPLICATION_CORE}/inc")

set(DIR_APPLICATION_DRIVERS "${DIR_APPLICATION}/drivers")
set(DIR_SRC_APPLICATION_DRIVERS "${DIR_APPLICATION_DRIVERS}/src")
set(DIR_INC_APPLICATION_DRIVERS "${DIR_APPLICATION_DRIVERS}/inc")


add_executable(${APPLICATION_NAME}
    ${DIR_SRC_DEVICE}/gcc/startup_NUC123.S
    ${DIR_SRC_DEVICE}/system_NUC123.c
    ${DIR_SRC_DEVICE}/gcc/_syscalls.c
    ${SRC_STD_DRIVER}
    ${SRC_CONFIGURATION}
    ${DIR_APPLICATION}/version.c
    ${DIR_APPLICATION}/main.c
    ${DIR_SRC_APPLICATION_DRIVERS}/powerdown.c
    ${DIR_SRC_APPLICATION_DRIVERS}/systick.c
    ${DIR_SRC_APPLICATION_DRIVERS}/usb.c
    ${DIR_SRC_APPLICATION_DRIVERS}/descriptors.c
    ${DIR_SRC_APPLICATION_DRIVERS}/cdc_serial.c
    ${DIR_SRC_APPLICATION_DRIVERS}/usb_out.c
    ${DIR_SRC_APPLICATION_DRIVERS}/bootloader_run.c
    ${DIR_SRC_APPLICATION_DRIVERS}/keys.c
    ${DIR_SRC_APPLICATION_DRIVERS}/analog_inputs.c
    ${DIR_SRC_APPLICATION_DRIVERS}/leds.c
    ${DIR_SRC_APPLICATION_DRIVERS}/critical_section.c
)

target_include_directories(${APPLICATION_NAME} PRIVATE
    ${DIR_INC_CMSIS}
    ${DIR_INC_DEVICE}
    ${DIR_INC_STD_DRIVER}
    ${DIR_INC_CONFIGURATION}
    ${DIR_INC_APPLICATION_DRIVERS}
)
target_compile_definitions(${APPLICATION_NAME} PRIVATE STDDRIVER_RETARGET_USER_OVERRIDE)

add_subdirectory(${DIR_APPLICATION_CORE})

set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/${DIR_SRC_DEVICE}/gcc/gcc_arm.ld")
set_target_properties(${APPLICATION_NAME} PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

target_link_options(${APPLICATION_NAME} BEFORE PRIVATE -Wl,-Map,${APPLICATION_NAME}.map -T${LINKER_SCRIPT})
target_link_libraries(${APPLICATION_NAME} core)

# Convert to binary file
add_custom_command(
    TARGET ${APPLICATION_NAME}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ARGS -O binary -S ${APPLICATION_NAME} ${APPLICATION_NAME}.bin
)

# Show size
add_custom_command(
    TARGET ${APPLICATION_NAME}
    POST_BUILD
    COMMAND ${CMAKE_SIZE} ARGS ${APPLICATION_NAME}
)


# DFU loader
set(DIR_LOADER "${DIR_SRC_ROOT}/loader")
set(LOADER_NAME "dfu")
file(WRITE ${DIR_LOADER}/version.c
    "const char bootloader_name[16] __attribute__ ((section (\".version\"))) = \"${LOADER_NAME}\";\n"
    "const char bootloader_version[16] __attribute__ ((section (\".version\"))) = \"V${PROJECT_VERSION_MAJOR}R${PROJECT_VERSION_MINOR}\";\n"
)

add_executable(${LOADER_NAME}
    ${DIR_SRC_DEVICE}/system_NUC123.c
    ${DIR_LOADER}/version.c
    ${DIR_LOADER}/startup_NUC123_user.S
    ${DIR_LOADER}/main.c
    ${DIR_LOADER}/descriptors.c
    ${DIR_LOADER}/dfu_transfer.c
    ${DIR_LOADER}/fmc_user.c
    ${DIR_LOADER}/usbd_user.c
)

target_include_directories(${LOADER_NAME} PRIVATE
    ${DIR_INC_CMSIS}
    ${DIR_INC_DEVICE}
    ${DIR_INC_STD_DRIVER}
    ${DIR_INC_CONFIGURATION}
)

# Dont use stdlib in loader
target_compile_options(${LOADER_NAME} PRIVATE -nostdlib)
target_compile_definitions(${LOADER_NAME} PRIVATE __STARTUP_CLEAR_BSS __START=main)

set_target_properties(${LOADER_NAME} PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

target_link_options(${LOADER_NAME} BEFORE PRIVATE -Wl,-Map,${LOADER_NAME}.map -T${LINKER_SCRIPT})

add_custom_command(
    TARGET ${LOADER_NAME}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ARGS -O binary -S ${LOADER_NAME} ${LOADER_NAME}.bin
)

add_custom_command(
    TARGET ${LOADER_NAME}
    POST_BUILD
    COMMAND ${CMAKE_SIZE} ARGS ${LOADER_NAME}
)
