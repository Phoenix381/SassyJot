QT += webenginewidgets webchannel widgets

HEADERS	    +=   src/include/mainwindow.h

SOURCES =       src/mainwindow.cpp \
		src/main.cpp

# unix {}
RESOURCES   +=	sassy.qrc

CONFIG += c++20