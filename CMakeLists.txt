
cmake_minimum_required(VERSION 3.0)

project(vcash)

set(WX_INCLUDES ./include /usr/local/include/wx-3.0 /usr/local/lib/wx/include/gtk3-unicode-static-3.0)

include_directories(${WX_INCLUDES} .)

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -D_UNICODE -D_FILE_OFFSET_BITS=64 -D__WXGTK__")

set(SOURCE_FILES AccountPage.cpp AddressesPage.cpp ConsolePage.cpp Controller.cpp EntryDialog.cpp
        HistoryPage.cpp MainFrame.cpp MainPanel.cpp MiningPage.cpp Resources.cpp
        StatisticsPage.cpp StatusBarImage.cpp StatusBarWallet.cpp OnPairsEvent.cpp StatusBar.cpp
        ToolsFrame.cpp ToolsPanel.cpp TransferPage.cpp VcashApp.cpp View.cpp WalletActions.cpp)

add_executable(vcash ${SOURCE_FILES})

LINK_DIRECTORIES(/usr/local/lib /usr/lib/x86_64-linux-gnu ${CMAKE_SOURCE_DIR}/lib)

set(vcashLib ${CMAKE_SOURCE_DIR}/lib)

TARGET_LINK_LIBRARIES(vcash
        ${vcashLib}/libcoin.a
        ${vcashLib}/libboost_system.a
        ${vcashLib}/libssl.a
        ${vcashLib}/libcrypto.a
        ${vcashLib}/libdb.a
        ${vcashLib}/libdb_cxx.a
        ${vcashLib}/libdatabase.a

        wx_gtk3u_richtext-3.0
        wx_gtk3u_adv-3.0
        wx_baseu_xml-3.0
        wx_gtk3u_html-3.0


        wx_gtk3u_core-3.0 wx_baseu-3.0 wx_gtk3u_adv-3.0 wxtiff-3.0 wxjpeg-3.0
        pthread
        gtk-3 gdk-3 gdk_pixbuf-2.0
        X11
        SM
        cairo pango-1.0 pangocairo-1.0
        gobject-2.0 glib-2.0 png z dl m
        expat)