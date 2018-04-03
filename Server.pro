QT -= gui

QT +=  core network sql testlib
CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BypassHandler/bypasshandler.cpp \
    ClientHandler/clienthandler.cpp \
    Config/serverconfig.cpp \
    FireWall/accessclientfirewall.cpp \
    JsonSocketLib/qt_json_socket_lib.cpp \
    JsonSocketLib/qtjsonsocketlib_v3.cpp \
    Protocol/Template/protocoltemplate.cpp \
    ServerList/serverlist.cpp \
    main.cpp \
    server.cpp \
    ClientHandler/Spread/spreadhandler.cpp \
    DbAddressPath/dbaddresspathbyfile.cpp

SUBDIRS += \
    Server.pro

HEADERS += \
    Block/AddressBlock/addressblock.h \
    Block/FileBlock/blockstruct.h \
    BypassHandler/bypasshandler.h \
    ClientHandler/clienthandler.h \
    Config/serverconfig.h \
    FireWall/accessclientfirewall.h \
    JsonSocketLib/qt_json_socket_lib.h \
    JsonSocketLib/qtjsonsocketlib_v3.h \
    Log/errorlog.h \
    Protocol/Template/protocoltemplate.h \
    Protocol/Type/protocoltype.h \
    Protocol/protocol.h \
    ServerList/serverlist.h \
    Type/ServerError/errorblock.h \
    servertest.h \
    clientmoc.h \
    server.h \
    Log/debuglog.h \
    ClientHandler/Spread/spreadhandler.h \
    DbAddressPath/dbaddresspathbyfile.h

