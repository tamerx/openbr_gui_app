#-------------------------------------------------
#
# Project created by QtCreator 2021-05-25T08:18:50
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = openbr_gui_app
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

CONFIG += console


INCLUDEPATH += C:\openbr\build-msvc2013\install\include \
               C:\Users\Asus\Desktop\face_detect\opencv-2.4.11\build-msvc2013\install\include

LIBS += -LC:\openbr\build-msvc2013\install\lib -lopenbr \
        -LC:\Users\Asus\Desktop\face_detect\opencv-2.4.11\build-msvc2013\lib -lopencv_core2411 \
        -LC:\Users\Asus\Desktop\face_detect\opencv-2.4.11\build-msvc2013\lib -lopencv_imgproc2411 \
        -LC:\Users\Asus\Desktop\face_detect\opencv-2.4.11\build-msvc2013\lib -lopencv_gpu2411 \
        -LC:\Users\Asus\Desktop\face_detect\opencv-2.4.11\build-msvc2013\lib -lopencv_highgui2411 \
        -LC:\Users\Asus\Desktop\face_detect\opencv-2.4.11\build-msvc2013\lib -lopencv_features2d2411 \
        -LC:\Users\Asus\Desktop\face_detect\opencv-2.4.11\build-msvc2013\lib -lopencv_calib3d2411 \
        -LC:\Users\Asus\Desktop\face_detect\opencv-2.4.11\build-msvc2013\lib -lopencv_contrib2411 \
        -LC:\Users\Asus\Desktop\face_detect\opencv-2.4.11\build-msvc2013\lib -lopencv_ml2411 \
        -LC:\Users\Asus\Desktop\face_detect\opencv-2.4.11\build-msvc2013\lib -lopencv_objdetect2411 \
        -LC:\Users\Asus\Desktop\face_detect\opencv-2.4.11\build-msvc2013\lib -lopencv_gpu2411 \
