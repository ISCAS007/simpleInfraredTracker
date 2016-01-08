#-------------------------------------------------
#
# Project created by QtCreator 2016-01-08T08:56:42
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = infrared
CONFIG   += console
CONFIG +=c++11
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += src/main.cpp\
src/pl/BackgroundSubtractorLBSP.cpp\
      src/pl/LBSP.cpp\
src/pl/BackgroundSubtractorLOBSTER.cpp\
   src/pl/LOBSTER.cpp\
src/pl/BackgroundSubtractorSuBSENSE.cpp\
  src/pl/SuBSENSE.cpp

INCLUDEPATH +=src/GNN/AuctionAlgorithm.h\
src/GNN/helper.h\
src/GNN/KalmanFilter.h\
src/GNN/Tracker.h\
src/pl/BackgroundSubtractorLBSP.h\
src/pl/BackgroundSubtractorLOBSTER.h\
src/pl/BackgroundSubtractorSuBSENSE.h\
src/pl/DistanceUtils.h\
src/pl/IBGS.h\
src/pl/LBSP.h\
src/pl/LOBSTER.h\
src/pl/RandUtils.h\
src/pl/SuBSENSE.h\
/usr/include/opencv

LIBS += /usr/local/lib/libopencv_calib3d.so\
/usr/local/lib/libopencv_nonfree.so\
/usr/local/lib/libopencv_contrib.so\
/usr/local/lib/libopencv_objdetect.so\
/usr/local/lib/libopencv_core.so\
/usr/local/lib/libopencv_ocl.so\
/usr/local/lib/libopencv_features2d.so\
/usr/local/lib/libopencv_photo.so\
/usr/local/lib/libopencv_flann.so\
#/usr/local/lib/libopencv_shape.so\
/usr/local/lib/libopencv_gpu.so\
/usr/local/lib/libopencv_stitching.so\
/usr/local/lib/libopencv_highgui.so\
/usr/local/lib/libopencv_superres.so\
#/usr/local/lib/libopencv_imgcodecs.so\
#/usr/local/lib/libopencv_videoio.so\
/usr/local/lib/libopencv_imgproc.so\
/usr/local/lib/libopencv_video.so\
/usr/local/lib/libopencv_legacy.so\
/usr/local/lib/libopencv_videostab.so\
/usr/local/lib/libopencv_ml.so\

HEADERS += \
    src/trackstatus.h \
    src/GNN/AuctionAlgorithm.h \
    src/GNN/helper.h \
    src/GNN/KalmanFilter.h \
    src/GNN/Tracker.h \
    src/pl/BackgroundSubtractorLBSP.h \
    src/pl/BackgroundSubtractorLOBSTER.h \
    src/pl/BackgroundSubtractorSuBSENSE.h \
    src/pl/DistanceUtils.h \
    src/pl/IBGS.h \
    src/pl/LBSP.h \
    src/pl/LOBSTER.h \
    src/pl/RandUtils.h \
    src/pl/SuBSENSE.h
