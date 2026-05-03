cmake_minimum_required(VERSION 3.16)

project(PolriWindowsClient VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tambahkan Multimedia & Dependencies
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Widgets WebSockets Multimedia Positioning Location Concurrent)

# Konfigurasi Opus dari Source Lokal
set(OPUS_BUILD_TESTING OFF CACHE BOOL "build tests" FORCE)
set(OPUS_BUILD_PROGRAMS OFF CACHE BOOL "build programs" FORCE)
add_subdirectory(opus/opus-1.5.2)

qt_standard_project_setup()

# Tambahkan WIN32 agar tidak muncul jendela console saat dijalankan
# Tambahkan executable
qt_add_executable(PolriWindowsClient WIN32
    src/main.cpp
    src/WebSocketManager.cpp
    src/WebSocketManager.h
    src/AudioHandler.cpp
    src/AudioHandler.h
    src/SettingsManager.cpp
    src/SettingsManager.h
    src/VideoRenderer.cpp
    src/VideoRenderer.h
    src/LocationManager.cpp
    src/LocationManager.h
    src/SoundManager.cpp
    src/SoundManager.h
)

qt_add_qml_module(PolriWindowsClient
    URI "PolriApp"
    VERSION 1.0
    QML_FILES
        res/qml/main.qml
        res/qml/Style.qml
        res/qml/components/Sidebar.qml
        res/qml/components/SidebarItem.qml
        res/qml/components/InfoCard.qml
        res/qml/components/PttButton.qml
        res/qml/components/MarqueeText.qml
        res/qml/screens/Dashboard.qml
        res/qml/screens/GroupScreen.qml
        res/qml/screens/UserScreen.qml
        res/qml/screens/MapsScreen.qml
        res/qml/screens/VideoScreen.qml
        res/qml/screens/SettingsScreen.qml
        res/qml/screens/AboutScreen.qml
        res/qml/screens/LoginScreen.qml
    RESOURCES
        assets/images/logo.png
        assets/sounds/on.wav
        assets/sounds/off.wav
        assets/sounds/end.wav
        assets/sounds/rx.wav
        assets/sounds/sos.wav
        assets/sounds/push.wav
)

target_link_libraries(PolriWindowsClient PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::Widgets
    Qt6::WebSockets
    Qt6::Multimedia
    Qt6::Positioning
    Qt6::Location
    Qt6::Concurrent
    opus
)

target_include_directories(PolriWindowsClient PRIVATE
    opus/opus-1.5.2/include
)

# --- KONFIGURASI UNTUK MEMBUAT SETUP.EXE (CPACK) ---

include(GNUInstallDirs)

# 1. Tentukan apa yang akan di-install
install(TARGETS PolriWindowsClient
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 2. Jalankan windeployqt secara otomatis (Hanya di Windows)
if(WIN32)
    find_package(Qt6 REQUIRED COMPONENTS Core)
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET PolriWindowsClient POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                    --qmldir "${CMAKE_CURRENT_SOURCE_DIR}/res/qml"
                    --no-translations
                    --compiler-runtime
                    "$<TARGET_FILE:PolriWindowsClient>"
            COMMENT "Menjalankan windeployqt untuk mengumpulkan dependensi..."
        )
    endif()
endif()

# 3. Konfigurasi CPack untuk membuat Installer (NSIS)
set(CPACK_PACKAGE_NAME "PolriWindowsClient")
set(CPACK_PACKAGE_VENDOR "Polri")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Polri PTT Native Windows Client")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Polri PTT Client")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/opus/opus-1.5.2/COPYING")
set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/assets/images/logo.png")

# Generator untuk Windows (Memerlukan NSIS terinstal di sistem)
set(CPACK_GENERATOR "NSIS")
set(CPACK_NSIS_DISPLAY_NAME "Polri PTT Client")
set(CPACK_NSIS_PACKAGE_NAME "Polri PTT Client")
set(CPACK_NSIS_HELP_LINK "https://polri.go.id")
set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/assets/images/logo.png") # Catatan: Idealnya .ico
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/assets/images/logo.png")

# Buat Shortcut di Desktop dan Start Menu
set(CPACK_NSIS_CREATE_ICONS_EXTRA "
  CreateShortCut '$DESKTOP\\Polri PTT Client.lnk' '$INSTDIR\\bin\\PolriWindowsClient.exe'
")
set(CPACK_NSIS_DELETE_ICONS_EXTRA "
  Delete '$DESKTOP\\Polri PTT Client.lnk'
")

include(CPack)
