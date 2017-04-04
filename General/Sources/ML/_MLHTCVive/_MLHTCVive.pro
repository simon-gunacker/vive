# -----------------------------------------------------------------------------
# _MLHTCVive project profile
#
# \author  Simon Gunacker
# \date    2017-03-31
# -----------------------------------------------------------------------------

TEMPLATE   = lib
TARGET     = _MLHTCVive

DESTDIR    = $$(MLAB_CURRENT_PACKAGE_DIR)/lib
DLLDESTDIR = $$(MLAB_CURRENT_PACKAGE_DIR)/lib

# Set high warn level (warn 4 on MSVC)
WARN = HIGH

# Add used projects here (see included pri files below for available projects)
CONFIG += dll ML inventor MLBase MLTools MLWEM MLBackgroundTasks

MLAB_PACKAGES += VR_General
MLAB_PACKAGES += MeVisLab_Standard

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += _MLHTCVIVE_EXPORTS

# Enable ML deprecated API warnings. To completely disable the deprecated API, change WARN to DISABLE.
DEFINES += ML_WARN_DEPRECATED

# Include OpenVR files
INCLUDEPATH += C:\\Users\\Vivemachine\\Desktop\\VR\\General\\lib\\glm
INCLUDEPATH += C:\\Users\\Vivemachine\\Desktop\\VR\\General\\lib\\glfw-3.2.1.bin.WIN64\\include
INCLUDEPATH += C:\\Users\\Vivemachine\\Desktop\\VR\\General\\lib\\glew-2.0.0\\include
INCLUDEPATH += C:\\Users\\Vivemachine\\Desktop\\VR\\General\\lib\\openvr-master\\headers

QMAKE_LIBDIR += C:\\Users\\Vivemachine\\Desktop\\VR\\General\\lib\\glfw-3.2.1.bin.WIN64\\lib-vc2015
QMAKE_LIBDIR += C:\\Users\\Vivemachine\\Desktop\\VR\\General\\lib\\glew-2.0.0\\lib\\Release\\x64
QMAKE_LIBDIR += C:\\Users\\Vivemachine\\Desktop\\VR\\General\\lib\\openvr-master\\lib\\win64
QMAKE_LIBDIR += C:\\Users\\Vivemachine\\Desktop\\VR\\General\\lib

LIBS += glfw3.lib
LIBS += glew32.lib
LIBS += openvr_api.lib
LIBS += opengl32.lib
LIBS += glu32.lib
LIBS += kernel32.lib
LIBS += user32.lib
LIBS += gdi32.lib
LIBS += winspool.lib
LIBS += comdlg32.lib
LIBS += advapi32.lib
LIBS += shell32.lib
LIBS += ole32.lib
LIBS += oleaut32.lib
LIBS += uuid.lib
LIBS += odbc32.lib
LIBS += odbccp32.lib

HEADERS += _MLHTCViveInit.h
HEADERS += _MLHTCViveSystem.h
HEADERS += mlHTCVive.h
HEADERS += Vive/Model.h
HEADERS += Vive/Vive.h

SOURCES += _MLHTCViveInit.cpp
SOURCES += mlHTCVive.cpp
SOURCES += Vive/Model.cpp
SOURCES += Vive/Vive.cpp
