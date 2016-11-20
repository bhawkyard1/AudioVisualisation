TARGET = AudioVisualisation

OBJECTS_DIR = obj

QT += opengl core

isEqual(QT_MAJOR_VERSION, 5) {
        cache()
        #DEFINES  += QT5BUILD
}

INCLUDEPATH += $$PWD/include

MOC_DIR = moc

SOURCES += $$PWD/main.cpp \
           $$PWD/src/*.cpp \
           $$PWD/src/*.c

HEADERS += $$PWD/include/*.hpp \
           $$PWD/include/*.h

OTHER_FILES += readme.md

CONFIG += console

win32 {
    INCLUDEPATH += C:\SDL2\i686-w64-mingw32\include\SDL2
    INCLUDEPATH += C:\SDL2\i686-w64-mingw32\include
    LIBS += -L"C:/SDL2/lib/x64/" -lSDL2 -lSDL2main -lSDL2_image -lSDL2_ttf -lSDL2_mixer
}

unix {
    QMAKE_CXXFLAGS += $$system(sdl2-config --cflags)

    LIBS += $$system(sdl2-config --libs)
    LIBS += -lSDL2_image -lSDL2_mixer -lSDL2_ttf
}
