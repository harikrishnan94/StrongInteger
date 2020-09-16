option(ENABLE_ASAN "Enable Address Sanitizer." OFF)
option(ENABLE_TSAN "Enable Thread Sanitizer." OFF)
option(ENABLE_UBSAN "Enable Undefined Behaviour Sanitizer." OFF)

if(CMAKE_SYSTEM_NAME STREQUAL Linux AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    option(ENABLE_MSAN "Enable Memory Sanitizer." OFF)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_ASAN_FLAGS "-fsanitize=address")
    set(CMAKE_TSAN_FLAGS "-fsanitize=thread")
    set(CMAKE_UBSAN_FLAGS
        "-fsanitize=undefined" "-fsanitize=bounds" "-fsanitize=float-divide-by-zero"
        "-fsanitize=integer" "-fsanitize=nullability")
    set(CMAKE_MSAN_FLAGS "-fsanitize=memory")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_ASAN_FLAGS "-fsanitize=address")
    set(CMAKE_TSAN_FLAGS "-fsanitize=thread")
    set(CMAKE_UBSAN_FLAGS "-fsanitize=undefined" "-fsanitize-address-use-after-scope")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    message(FATAL "Unsupported compiler")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_ASAN_FLAGS "/fsanitize=address")
    set(CMAKE_TSAN_FLAGS "/fsanitize=thread")
    set(CMAKE_UBSAN_FLAGS "/fsanitize=undefined")
endif()

if(ENABLE_UBSAN OR ENABLE_ASAN)
    set(SAN_ENABLED On)
endif()

if(ENABLE_TSAN)
    if(SAN_ENABLED)
        message(FATAL "Cannot enable Thread Sanitizer when Address or Undefined Sanitizer Enabled")
    endif()
    set(SAN_ENABLED On)
endif()

if(ENABLE_MSAN)
    if(SAN_ENABLED)
        message(
            FATAL
            "Cannot enable Memory Sanitizer when Address or Undefined or Thread Sanitizer Enabled")
    endif()
endif()

function(add_sanitizer_flags TARGET)
    if(ENABLE_ASAN)
        target_compile_options(${TARGET} PRIVATE ${CMAKE_ASAN_FLAGS})
        target_link_options(${TARGET} PRIVATE ${CMAKE_ASAN_FLAGS})
    endif()
    if(ENABLE_TSAN)
        target_compile_options(${TARGET} PRIVATE ${CMAKE_TSAN_FLAGS})
        target_link_options(${TARGET} PRIVATE ${CMAKE_TSAN_FLAGS})
    endif()
    if(ENABLE_UBSAN)
        target_compile_options(${TARGET} PRIVATE ${CMAKE_UBSAN_FLAGS})
        target_link_options(${TARGET} PRIVATE ${CMAKE_UBSAN_FLAGS})
    endif()
    if(ENABLE_MBSAN)
        target_compile_options(${TARGET} PRIVATE ${CMAKE_MSAN_FLAGS})
        target_link_options(${TARGET} PRIVATE ${CMAKE_MSAN_FLAGS})
    endif()
endfunction()
