QMAKE_CXXFLAGS += -O3 -Wall -static -std=c++11 -fopenmp

OBJECTS_DIR = objects
TARGET      = cad2017

INCLUDEPATH += \ # start from the binary file
    $$PWD/includes

LIBS += \
    -L$$PWD/lib/lefdef \ # start from the binary file
    -llef -ldef -fopenmp

SOURCES += \ # start from the .pro file
    src/common/arghandler.cpp \
    src/common/paramhandler.cpp \
    src/common/util.cpp \
    src/common/GnuplotPlotter.cpp \
    src/circuit/circuit.cpp \
    src/parser/defrw.cpp \
    src/parser/lefrw.cpp \
    src/parser/parser.cpp \
    src/placer/placer.cpp \
    src/placer/node.cpp \
    src/main.cpp

HEADERS += \
    src/common/arghandler.h \
    src/common/paramhandler.h \
    src/common/util.h \
    src/common/GnuplotPlotter.h \
    src/circuit/circuit.h \
    src/circuit/module.h \
    src/circuit/net.h \
    src/circuit/pin.h \
    src/circuit/row.h \
    src/circuit/layer.h \
    src/circuit/fregion.h \
    src/parser/parser.h \
    src/placer/placer.h \
    src/placer/node.h \

