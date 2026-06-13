cmake_minimum_required(VERSION 3.16)
project(AITxtToPicture LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets WebEngineWidgets Network)

add_executable(ai_txt_to_picture
    main.cpp
    config.hpp config.cpp
    ai_client.hpp ai_client.cpp
    preview_view.hpp preview_view.cpp
    code_editor.hpp code_editor.cpp
    mainwindow.hpp mainwindow.cpp
    settingsdialog.hpp settingsdialog.cpp
)

target_link_libraries(ai_txt_to_picture PRIVATE
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::Network
)

add_executable(test_export
    test_export.cpp
    preview_view.hpp preview_view.cpp
)

target_link_libraries(test_export PRIVATE
    Qt6::Widgets
    Qt6::WebEngineWidgets
)
