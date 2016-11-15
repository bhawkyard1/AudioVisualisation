TARGET = PoetryGenerator

OBJECTS_DIR = obj

QT += opengl core

isEqual(QT_MAJOR_VERSION, 5) {
        cache()
        #DEFINES  += QT5BUILD
}

INCLUDEPATH += $$PWD/include

MOC_DIR = moc

SOURCES += $$PWD/main.cpp \
                      $$PWD/src/*.cpp

HEADERS += $$PWD/include/*.hpp

OTHER_FILES += readme.md

CONFIG += console

unix {
    QMAKE_CXXFLAGS += $$system(sdl2-config --cflags)

    LIBS += $$system(sdl2-config --libs)
    LIBS += -lSDL2_image -lSDL2_mixer -lSDL2_ttf
}
