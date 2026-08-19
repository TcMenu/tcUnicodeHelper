
cmake_minimum_required(VERSION 3.13)

add_library(tcUnicodeHelper 
        ../src/tcUnicodeHelper.cpp
        ../src/Utf8TextProcessor.cpp
)

if (BUILD_FOR_STM32CUBE)
    target_compile_definitions(tcUnicodeHelper
            PUBLIC BUILD_FOR_STM32CUBE_CMAKE=1 IO_LOGGING_DEBUG=1
    )
else ()
    target_compile_definitions(tcUnicodeHelper
            PUBLIC BUILD_FOR_PICO_CMAKE=1 BUILD_PICO_FORCE_UART=1 IO_LOGGING_DEBUG=1
    )
endif ()

target_include_directories(tcUnicodeHelper PUBLIC
        ../src
)

target_link_libraries(tcUnicodeHelper PUBLIC IoAbstraction TcMenuLog AdafruitGFXNativePort)


# only bring in pico libs when compiling for PicoSDK
if(TARGET pico_stdlib AND TARGET pico_sync)
    message("Build is pico, adding libs")
    target_link_libraries(tcUnicodeHelper PUBLIC pico_stdlib pico_sync)
endif()
