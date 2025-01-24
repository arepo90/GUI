cmake_minimum_required(VERSION 3.19)
project(bruh LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS
    Widgets
    3DCore
    3DRender
    3DInput
    3DLogic
    3DExtras
    3DAnimation
    Multimedia
    MultimediaWidgets
)
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

qt_standard_project_setup()

qt_add_executable(bruh
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

target_link_libraries(bruh PRIVATE
    Qt6::Widgets
    Qt6::3DCore
    Qt6::3DRender
    Qt6::3DLogic
    Qt6::3DExtras
    Qt6::3DAnimation
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    ws2_32
    ${OpenCV_LIBS}
)

include(GNUInstallDirs)

install(TARGETS bruh
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET bruh
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
