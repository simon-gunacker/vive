# MeVisLab-Module for Virtual Reality
This is a MeVisLab-Module for visualizing medical images in Virtual Reality.

# How to compile the project?
This project has been built for MeVisLab2.8.1VC14-64 on Windows 8.1 using Visual Studio 2015 (v140). It has not been tested on other platforms. All relevant libraries are included in the project. To compile the project, the following steps have to be done:

1. `INCLUDEPATH` and `LIBPATH` (can be found in `General\Sources\ML\_MLHTCVive\_MLHTCVive.pro`) have to be adapted

1. in case the project has already been built before, `make.clean.bat` has to be invoked to clean the project again

1. to build the project, `make.build.bat` has to be invoked

1. `LocalDebuggerCommand` and `LocalDebuggerCommandArguments` have to be adaped in `General\Sources\ML\_MLHTCVive\_MLHTCVive.vcxproj.user.bak`. After doing so, `make.fix.bat` ca be invoked.

1. `make.run.bat` can be invoked. It opens the project in VisualStudio where it can be compiled

> **Warning**: the root folder of the project has to be named *VR*, otherwise `General/Configuration/VR_General.pri` cannot be found by MeVisLab any more.

# How to run the project
After starting the project in VisualStudio, make sure to load a STL-file in MeVisLab before VR is started - otherwise the application will throw an exception.
