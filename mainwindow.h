#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCamera>
#include <QCameraImageCapture>
#include <QMediaRecorder>
#include <QCoreApplication>
#include <QMainWindow>
#include <QMediaService>
#include <QMediaRecorder>
#include <QCameraViewfinder>
#include <QCameraInfo>
#include <QMediaMetaData>
#include <QDebug>
#include <QMessageBox>
#include <QPalette>
#include <QtWidgets>
#include <QFileDialog>
#include <iostream>
#include <algorithm>
#include <map>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <openbr/openbr_plugin.h>

using namespace std;
using namespace cv;
using namespace br;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget* parent = 0);
  ~MainWindow();

 private slots:

  void setCamera(const QCameraInfo& cameraInfo);
  void startCamera();
  void stopCamera();
  void toggleLock();
  void takeImage();
  void searchImage();
  void displayCaptureError(int, QCameraImageCapture::Error,
                           const QString& errorString);
  void displayRecorderError();
  void displayCameraError();
  void updateCameraState(QCamera::State);
  void setExposureCompensation(int index);
  void updateRecordTime();
  void processCapturedImage(int requestId, const QImage& img);
  void updateLockStatus(QCamera::LockStatus, QCamera::LockChangeReason);
  void readyForCapture(bool ready);
  void imageSaved(int id, const QString& fileName);
  void opencvFaceDetectProcess(cv::Mat& image);
  void uploadImage();
  void findAgeAndGender(QFileInfo& fileInfo);

 protected:
  void closeEvent(QCloseEvent* event);

 private:
  Ui::MainWindow* ui;
  QCamera* camera;
  QCameraImageCapture* imageCapture;
  QMediaRecorder* mediaRecorder;
  QString videoContainerFormat;
  bool isCapturingImage;
  bool applicationExiting;
  QString defaultImagePathForSave;
  QString defaultImagePathForSearch;
  cv::Mat convertQImageToMat(QImage image);

  QString destinationFile;
  QFileInfo globalFileInfo;
  QFileInfo globalLastFileInfo;


  QImage globalScaledImage;

  bool isSearch = false;
  bool isCaptured = false;
  bool isUploadedImage = false;
};

#endif  // MAINWINDOW_H
