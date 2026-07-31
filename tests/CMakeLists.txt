set(CMAKE_AUTOMOC ON)

add_executable(tests
    test_main.cpp
    test_scanfiles.cpp
    test_scanfiles.h
    test_formatsize.cpp
    test_formatsize.h
    test_xorkey.cpp
    test_xorkey.h
    test_processor.cpp
    test_processor.h
    ../startupdialog.cpp
    ../startupdialog.h
    ../settingsdialog.cpp
    ../settingsdialog.h
    ../fileprocessor.cpp
    ../fileprocessor.h
    ../mainwindow.cpp
    ../mainwindow.h
    ../mainwindow.ui
)

target_link_libraries(tests PRIVATE Qt6::Test Qt6::Widgets)
target_include_directories(tests PRIVATE ..)

add_test(NAME tests COMMAND tests)
