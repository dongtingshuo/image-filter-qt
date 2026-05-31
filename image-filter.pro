QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/include

SOURCES += \
    src/main.cpp \
    src/main_window.cpp

HEADERS += \
    include/main_window.h

FORMS += \
    ui/main_window.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# OpenCV配置（MinGW版，已修正格式）
win32 {
    # 1. 头文件路径（指向包含opencv2的include文件夹）
    INCLUDEPATH += D:\OpenCV4.11_mingw_launch\include

    # 2. 库文件搜索路径（必须加 -L，指定库所在目录）
    LIBS += -LD:\OpenCV4.11_mingw_launch\x64\mingw\lib
    LIBS += -LD:\OpenCV4.11_mingw_launch\x64\mingw\bin

    # 3. 链接具体库文件（用 -l 加库名，而非直接写路径）
    # 注意：libopencv_world4110.dll.a 对应的库名是 opencv_world4110（去掉前缀lib和后缀.dll.a）
    LIBS += -lopencv_world4110
}
