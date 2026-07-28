set(CMAKE_AUTOMOC ON)

add_executable(tests
    test_main.cpp
    test_scanfiles.cpp
    test_scanfiles.h
    ../startupdialog.cpp
    ../startupdialog.h
)

target_link_libraries(tests PRIVATE Qt6::Test Qt6::Widgets)
target_include_directories(tests PRIVATE ..)

add_test(NAME tests COMMAND tests)
