QT += webenginewidgets widgets

HEADERS	    +=   src/include/mainwindow.h

SOURCES	    +=   $$files("src/*.cpp", true)
# src/mainwindow.cpp
# $$files("src/*.cpp", true)

# unix {}
RESOURCES   +=	sassy.qrc
