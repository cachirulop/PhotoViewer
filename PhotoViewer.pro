#-------------------------------------------------
#
# Project created by QtCreator 2014-08-23T08:44:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PhotoViewer
TEMPLATE = app


SOURCES += main.cpp\
        photoviewer.cpp \
    pictureview.cpp

HEADERS  += photoviewer.h \
    pictureview.h

FORMS    += photoviewer.ui

OTHER_FILES += \
    images/folder-open.png \
    images/media-playback-start.png

RESOURCES += \
    PhotoViewer.qrc

win32|win32-msvc* {
INCLUDEPATH  += $$quote(C:/Users/dmagro/git/exiv2-0.24/msvc2012/include)
LIBS         += $$quote(C:/Users/dmagro/git/exiv2-0.24/msvc2012/exiv2lib/Win32/Debug/exiv2sd.lib)
LIBS         += $$quote(C:/Users/dmagro/git/exiv2-0.24/msvc2012/expat/Win32/Debug/libexpat.lib)
LIBS         += $$quote(C:/Users/dmagro/git/exiv2-0.24/msvc2012/zlib/Win32/Debug/zlib1d.lib)
LIBS         += $$quote(C:/Users/dmagro/git/exiv2-0.24/msvc2012/xmpsdk/Win32/Debug/xmpsdk.lib)
}

unix {
INCLUDEPATH  += /usr/local/include
LIBS         += -L/usr/local/lib -lexiv2
}
