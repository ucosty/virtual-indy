TEMPLATE = app
TARGET = VirtualIndy

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

unix:LIBS += -lncurses -pthread


SOURCES += \
    main.cpp \
    debuggerform.cpp \
    eprom.cpp \
    error.cpp \
    exceptions.cpp \
    graphics_lg1.cpp \
    hpc3.cpp \
    mc.cpp \
    memory_bus.cpp \
    memory.cpp \
    processor_COP0.cpp \
    processor_COP1.cpp \
    processor_disassembler.cpp \
    processor_i_type.cpp \
    processor_j_type.cpp \
    processor_r_type.cpp \
    processor_regimm.cpp \
    processor_special2.cpp \
    processor_special3.cpp \
    processor_utils.cpp \
    processor.cpp \
    rom.cpp \
    seeq_8003_8020.cpp \
    utils.cpp \
    z85c30.cpp \
    indyemulator.cpp \
    cpuregistersdisplaymodel.cpp

FORMS += \
    debuggerForm.ui

HEADERS += \
    debuggerform.h \
    debug.h \
    eprom.h \
    error.h \
    exceptions.h \
    graphics_lg1.h \
    hpc3.h \
    macros.h \
    main.h \
    mc.h \
    memory_bus.h \
    memory.h \
    optimize.h \
    processor_utils.h \
    processor.h \
    rom.h \
    seeq_8003_8020.h \
    utils.h \
    z85c30.h \
    indyemulator.h \
    cpuregistersdisplaymodel.h

DISTFILES += \
    VirtualIndy.pro.user
