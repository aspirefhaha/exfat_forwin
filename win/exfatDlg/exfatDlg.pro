######################################################################
# Automatically generated by qmake (2.01a) ?? ?? 12 21:20:45 2020
######################################################################

TEMPLATE = app
TARGET = 
#QMAKE_LFLAGS *= /MACHINE:X64
DEFINES += "_MBCS"
DEPENDPATH += . GeneratedFiles
INCLUDEPATH += . 

#LIBS += -L../InEnvDirPlugin/debug 

!contains(QMAKE_TARGET.arch, x86_64) {
    message("x64 build")
    ## Windows x64 (64bit) specific build here       
} else {
    message("x86 build")
    ## Windows x86 (32bit) specific build here
}
# Input
HEADERS += exfatdlg.h exfatModel.h
FORMS += exfatdlg.ui \
          exnotepad.ui
SOURCES += exfatdlg.cpp \
           exfatModel.cpp \
		   exnotepad.cpp \
           main.cpp 
RESOURCES += exfatdlg.qrc
TRANSLATIONS += zh_CN.ts
