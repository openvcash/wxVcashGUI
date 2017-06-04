cmake_minimum_required(VERSION 3.0)

project(vcash)

# directory for wxVcashGUI sources
set(WXVCASHGUI_SRC_PATH ${CMAKE_SOURCE_DIR})

# directory for vcash sources
set(VCASH_SRC_PATH ${WXVCASHGUI_SRC_PATH}/..)

# subpath for libaries compiled by bjam in build-linux.sh
set(VCASH_LIB_SUBPATH bin/gcc-${CMAKE_C_COMPILER_VERSION}/release/link-static/threading-multi)

# directory for wxWidgets includes
set(WX_INCLUDES /usr/local/include/wx-3.1 /usr/local/lib/wx/include/gtk3-unicode-static-3.1)


include_directories(
        ${WXVCASHGUI_SRC_PATH}/include
        ${WXVCASHGUI_SRC_PATH}/deps/libqrencode

        ${VCASH_SRC_PATH}/coin/include
        ${VCASH_SRC_PATH}/deps/boost
        ${VCASH_SRC_PATH}/deps/openssl/include
        ${VCASH_SRC_PATH}/deps/db/include

        ${WX_INCLUDES})

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -D_UNICODE -D_FILE_OFFSET_BITS=64 -D__WXGTK__")

set(SOURCE_FILES
        AccountPage.cpp
        AddressesPage.cpp
        BlockExplorer.cpp
        ConsolePage.cpp
        ContextMenu.cpp
        Controller.cpp
        EntryDialog.cpp
        HistoryPage.cpp
        MainFrame.cpp
        MainPanel.cpp
        MiningPage.cpp
        QRDialog.cpp
        Resources.cpp
        ShowInfoDialog.cpp
        StatisticsPage.cpp
        StatusBarImage.cpp
        StatusBarWallet.cpp
        OnPairsEvent.cpp
        StatusBar.cpp
        TaskBarIcon.cpp
        ToolsFrame.cpp
        ToolsPanel.cpp
        TransferPage.cpp
        Utils.cpp
        VcashApp.cpp
        View.cpp
        WalletActions.cpp
        wxStack.cpp)

add_executable(vcash ${SOURCE_FILES})

LINK_DIRECTORIES(/usr/local/lib /usr/lib/x86_64-linux-gnu)

TARGET_LINK_LIBRARIES(vcash
        ${VCASH_SRC_PATH}/coin/${VCASH_LIB_SUBPATH}/libcoin.a
        ${VCASH_SRC_PATH}/database/${VCASH_LIB_SUBPATH}/libdatabase.a

        ${VCASH_SRC_PATH}/deps/boost/stage/lib/libboost_system.a

        ${VCASH_SRC_PATH}/deps/openssl/lib/libssl.a
        ${VCASH_SRC_PATH}/deps/openssl/lib/libcrypto.a

        ${VCASH_SRC_PATH}/deps/db/lib/libdb.a
        ${VCASH_SRC_PATH}/deps/db/lib/libdb_cxx.a

        ${WXVCASHGUI_SRC_PATH}/deps/libqrencode/libqrencode.a
        
        Xxf86vm
        wx_gtk3u_richtext-3.1
        wx_gtk3u_adv-3.1
        wx_baseu_xml-3.1
        wx_gtk3u_html-3.1
        wx_baseu_net-3.1

        wx_gtk3u_core-3.1 wx_baseu-3.1 wx_gtk3u_adv-3.1 wxtiff-3.1 wxjpeg-3.1
        pthread
        gtk-3 gdk-3 gdk_pixbuf-2.0
        X11
        SM
        cairo pango-1.0 pangocairo-1.0
        gobject-2.0 glib-2.0 png z dl m
        expat)
