# ************************************************
# @file             : CMakeLists.txt
# @file-Location    : <project-root-folder>
# @author           : Jabed-Akhtar (github)
# @date-of-creation : 2025.04.25
# ************************************************

# ================================================
# 1. Project setup
# ================================================
# Set the min. CMake version needed
cmake_minimum_required(VERSION 3.22)

# !!! Must come before project()
set(CMAKE_SYSTEM_NAME Generic)  # Critical for cross-compilation
set(CMAKE_SYSTEM_PROCESSOR arm) # Target processor architecture
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)  # Skip host system checks

set(TOOLCHAIN_PREFIX arm-none-eabi-)

# Toolchain configuration
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}gcc)

# ================================================
# 2. Project definition
# ================================================
set(PROJECT_NAME "DCMotor_PositionNSpeedCtrl")

# Define project name, version and languages
project(${PROJECT_NAME}
    VERSION 1.0.0
    LANGUAGES C ASM
)

# Diagnostic messages
message(STATUS "-------------------------------------------")
message(STATUS "----->>> Toolchain verification ---------------")
message(STATUS "C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "ASM compiler: ${CMAKE_ASM_COMPILER}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "-------------------------------------------")

# ================================================
# 3. Build system configuration
# ================================================
set(EXECUTABLE ${CMAKE_PROJECT_NAME})

# C language standard configuration
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)   # Strict compliance
set(CMAKE_C_EXTENSIONS ON)          # Allow compiler extensions

# ================================================
# 4. Headers
# ================================================
set(CUBEDIR_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F0xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F0xx_HAL_Driver/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F0xx_HAL_Driver/Inc/Legacy
)

# ================================================
# 5. Source files
# ================================================
file(GLOB_RECURSE CUBEDIR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F0xx_HAL_Driver/Src/*.c
)

# ================================================
# 6. Project configuration
# ================================================
set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# MCU-specific configuration
set(MCU_FAMILY STM32F0xx)
set(MCU_MODEL STM32F051x8)

# CPU architecture flags
set(CPU_PARAMETERS
    -mcpu=cortex-m0     # Cortex-M0 core
    -mthumb             # Thumb instruction set
    -mfloat-abi=soft    # Hardware floating-point ABI
)

# Critical files
set(STARTUP_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/Core/Startup/startup_stm32f051r8tx.s)
set(MCU_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32F051R8TX_FLASH.ld)

# ================================================
# 7. Executable target
# ================================================
add_executable(${EXECUTABLE}
    ${CUBEDIR_SOURCES}
    ${STARTUP_SCRIPT}   # Critical: Startup code must be included
)

# Preprocessor definitions | Embedded macros(defines)
target_compile_definitions(${EXECUTABLE} PRIVATE
    ${MCU_MODEL}
    USE_HAL_DRIVER      # Enable HAL library
)

# Include directories (!!! AFTER add_executable)
target_include_directories(${EXECUTABLE} PRIVATE
    ${CUBEDIR_INCLUDE_DIRECTORIES}
)

# ================================================
# 8. Compiler options
# ================================================
target_compile_options(${EXECUTABLE} PRIVATE
    ${CPU_PARAMETERS}       # MCU-specific flags
    -Wall                   # Enable all warnings
    -Wextra                 # Extra warnings
    -Wpedantic              # Strict ISO C compliance
    -Wno-unused-parameter   # Disable specific warning

    # Debug/Release configurations
    $<$<CONFIG:Debug>:-Og -g3 -ggdb>    # Debug symbols
    $<$<CONFIG:Release>:-Og -g0>        # Release optimizations
)

# ================================================
# 9. Linker configuration
# ================================================
target_link_libraries(${EXECUTABLE} PRIVATE
    -lnosys             # Minimal system library
)

target_link_options(${EXECUTABLE} PRIVATE
    -T${MCU_LINKER_SCRIPT}              # Linker script
    ${CPU_PARAMETERS}                   # MCU flags
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map  # Generate memory map
    --specs=nosys.specs                 # No system calls

    # Linker optimizations
    -Wl,--gc-sections                   # Remove unused sections, gc: garbage collector
    -Wl,--print-memory-usage            # Memory usage report
    -Wl,--cref                          # Add cross-reference table

    # Standard libraries
    -Wl,--start-group
    -lc                                 # C library
    -lm                                 # Math library
    -lstdc++                            # Kept for C standard library compatibility
    -Wl,--end-group
)

# ================================================
# 10. Post-build actions
# ================================================
add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.bin
    COMMENT "Generating HEX and BIN firmware images"
)

message(STATUS "Project ${PROJECT_NAME} configured successfully.")

# **************** END OF FILE *******************
