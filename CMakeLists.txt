cmake_minimum_required(VERSION 3.20)
project(SDRunoPlugin_Cosmo VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)

# Set up include directories
set(SDRUNO_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include" CACHE PATH "SDRuno include directory")
set(NANA_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/nana/include" CACHE PATH "Nana GUI include directory")

# Plugin source files
set(PLUGIN_SOURCES
    SDRunoPlugin_Cosmo.cpp
    SDRunoPlugin_CosmoUi.cpp
)

set(PLUGIN_HEADERS
    SDRunoPlugin_Cosmo.h
    SDRunoPlugin_CosmoUi.h
)

# Create the plugin DLL
add_library(SDRunoPlugin_Cosmo SHARED
    ${PLUGIN_SOURCES}
    ${PLUGIN_HEADERS}
    SDRunoPlugin_Cosmo.rc
)

# Set target properties
set_target_properties(SDRunoPlugin_Cosmo PROPERTIES
    OUTPUT_NAME "SDRunoPlugin_Cosmo"
    SUFFIX ".dll"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Include directories
target_include_directories(SDRunoPlugin_Cosmo PRIVATE
    ${SDRUNO_INCLUDE_DIR}
    ${NANA_INCLUDE_DIR}
)

# Compiler definitions
target_compile_definitions(SDRunoPlugin_Cosmo PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    SDRUNOPLUGINCOSMO_EXPORTS
    _WINDOWS
    _USRDLL
    UNICODE
    _UNICODE
)

# Link libraries
target_link_libraries(SDRunoPlugin_Cosmo PRIVATE
    Threads::Threads
)

# Platform-specific settings
if(WIN32)
    target_link_libraries(SDRunoPlugin_Cosmo PRIVATE
        user32
        gdi32
        shell32
        ole32
        oleaut32
        uuid
        comdlg32
        advapi32
    )
    
    # Use the DEF file for exports
    set_target_properties(SDRunoPlugin_Cosmo PROPERTIES
        LINK_FLAGS "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/SDRunoPlugin_Cosmo.def"
    )
endif()

# Installation
install(TARGETS SDRunoPlugin_Cosmo
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)