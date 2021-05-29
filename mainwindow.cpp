#include "mainwindow.h"
#include "ui_mainwindow.h"

Q_DECLARE_METATYPE(QCameraInfo)

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      camera(0),
      imageCapture(0),
      mediaRecorder(0),
      isCapturingImage(false),
      applicationExiting(false) {

              ui->setupUi(this);


              this->setFixedSize(QSize(1041, 397));



              // Camera devices:
              QActionGroup *videoDevicesGroup = new QActionGroup(this);
              videoDevicesGroup->setExclusive(true);
              foreach (const QCameraInfo &cameraInfo, QCameraInfo::availableCameras()) {

                QAction *videoDeviceAction = new QAction(cameraInfo.description(), videoDevicesGroup);
                videoDeviceAction->setCheckable(true);
                videoDeviceAction->setData(QVariant::fromValue(cameraInfo));
                if (cameraInfo == QCameraInfo::defaultCamera()) {
                  videoDeviceAction->setChecked(true);
                }

                ui->menuDevices->addAction(videoDeviceAction);

              }

              defaultImagePathForSave = "C:\\Users\\Asus\\Desktop\\openbr_train_save\\";
              defaultImagePathForSearch = "C:\\Users\\Asus\\Desktop\\openbr_train_search\\";
              setCamera(QCameraInfo::defaultCamera());

}

MainWindow::~MainWindow() {
  delete mediaRecorder;
  delete imageCapture;
  delete camera;
}

void MainWindow::setCamera(const QCameraInfo &cameraInfo) {

      delete imageCapture;
      delete mediaRecorder;
      delete camera;

      camera = new QCamera(cameraInfo);

      connect(camera, SIGNAL(stateChanged(QCamera::State)), this,SLOT(updateCameraState(QCamera::State)));
      connect(camera, SIGNAL(error(QCamera::Error)), this,SLOT(displayCameraError()));

      mediaRecorder = new QMediaRecorder(camera);
      imageCapture = new QCameraImageCapture(camera);

      connect(mediaRecorder, SIGNAL(durationChanged(qint64)), this,SLOT(updateRecordTime()));
      connect(mediaRecorder, SIGNAL(error(QMediaRecorder::Error)), this, SLOT(displayRecorderError()));

      mediaRecorder->setMetaData(QMediaMetaData::Title,
                                 QVariant(QLatin1String("Test Title")));

      camera->setViewfinder(ui->viewfinder);

      updateCameraState(camera->state());
      updateLockStatus(camera->lockStatus(), QCamera::UserRequest);
      connect(imageCapture, SIGNAL(readyForCaptureChanged(bool)), this,SLOT(readyForCapture(bool)));
      connect(imageCapture, SIGNAL(imageCaptured(int, QImage)), this,SLOT(processCapturedImage(int, QImage)));
      connect(imageCapture, SIGNAL(imageSaved(int, QString)), this,SLOT(imageSaved(int, QString)));
      connect(imageCapture, SIGNAL(error(int, QCameraImageCapture::Error, QString)),this,SLOT(displayCaptureError(int, QCameraImageCapture::Error, QString)));
      connect(camera,SIGNAL(lockStatusChanged(QCamera::LockStatus, QCamera::LockChangeReason)),this,SLOT(updateLockStatus(QCamera::LockStatus, QCamera::LockChangeReason)));

      camera->start();
}

void MainWindow::updateRecordTime() {

    QString str = QString("Recorded %1 sec").arg(mediaRecorder->duration() / 1000);
    ui->statusBar->showMessage(str);

}

void MainWindow::processCapturedImage(int requestId, const QImage &img) {

  Q_UNUSED(requestId);
  QImage scaledImage = img.scaled(ui->viewfinder->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
  ui->lastImagePreviewLabel->setPixmap(QPixmap::fromImage(scaledImage));
  cv::Mat opencvImage = convertQImageToMat(scaledImage);
  opencvFaceDetectProcess(opencvImage);
}

cv::Mat MainWindow::convertQImageToMat(QImage image) {
  cv::Mat mat;
  switch (image.format()) {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
      mat = cv::Mat(image.height(), image.width(), CV_8UC4,
                    (void *)image.constBits(), image.bytesPerLine());
      break;
    case QImage::Format_RGB888:
      mat = cv::Mat(image.height(), image.width(), CV_8UC3,
                    (void *)image.constBits(), image.bytesPerLine());
      cv::cvtColor(mat, mat, CV_BGR2RGB);
      break;
    case QImage::Format_Grayscale8:
      mat = cv::Mat(image.height(), image.width(), CV_8UC1,
                    (void *)image.constBits(), image.bytesPerLine());
      break;
  }

  return mat;
}

void MainWindow::opencvFaceDetectProcess(cv::Mat &image) {

                  double scale = 1.0;
                  cv::CascadeClassifier face_cascade;
                  face_cascade.load("C:\\Users\\Asus\\Documents\\openbr_gui_app\\haarcascade_frontalface_alt.xml");

                  if (face_cascade.empty()) {
                    qDebug() << "Error Loading XML file";
                    close();
                  }

                  // Detect faces
                  std::vector<cv::Rect> faces;
                  face_cascade.detectMultiScale(image, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE,cv::Size(30, 30));

                    for (cv::Rect area : faces) {

                            cv::Scalar drawColor = cv::Scalar(20, 255, 160);
                            cv::rectangle(image,
                            cv::Point(cvRound(area.x * scale), cvRound(area.y * scale)),
                            cv::Point(cvRound((area.x + area.width - 1) * scale),cvRound((area.y + area.height - 1) * scale)),drawColor);

                    }

}

void MainWindow::startCamera() {
    camera->start();
}

void MainWindow::stopCamera() {
    camera->stop();
}

void MainWindow::toggleLock() {
  switch (camera->lockStatus()) {
    case QCamera::Searching:
    case QCamera::Locked:
      camera->unlock();
      break;
    case QCamera::Unlocked:
      camera->searchAndLock();
  }
}

void MainWindow::updateLockStatus(QCamera::LockStatus status,QCamera::LockChangeReason reason) {

    QColor indicationColor = Qt::black;

  switch (status) {
    case QCamera::Searching:
      indicationColor = Qt::yellow;
      ui->statusBar->showMessage(tr("Focusing..."));

      break;
    case QCamera::Locked:
      indicationColor = Qt::darkGreen;
      ui->statusBar->showMessage(tr("Focused"), 2000);
      break;
    case QCamera::Unlocked:
      indicationColor = reason == QCamera::LockFailed ? Qt::red : Qt::black;

      if (reason == QCamera::LockFailed)
        ui->statusBar->showMessage(tr("Focus Failed"), 2000);
  }

}

void MainWindow::takeImage() {
  // QMessageBox::warning(this, tr("Image Capture Error"),"myErrorString");

  isCapturingImage = true;
  imageCapture->capture(defaultImagePathForSave);
  isCapturingImage=false;

}


void MainWindow::searchImage()
{

    isCapturingImage = true;
    isSearch=true;
    imageCapture->capture(defaultImagePathForSearch);

}


void MainWindow::displayCaptureError(int id, const QCameraImageCapture::Error error,const QString &errorString) {

  Q_UNUSED(id);
  Q_UNUSED(error);
  QMessageBox::warning(this, tr("Image Capture Error"), errorString);
  isCapturingImage = false;

}

void MainWindow::updateCameraState(QCamera::State state) {
  switch (state) {
    case QCamera::ActiveState:
      ui->actionStartCamera->setEnabled(false);
      ui->actionStopCamera->setEnabled(true);
      break;

    case QCamera::UnloadedState:
    case QCamera::LoadedState:
      ui->actionStartCamera->setEnabled(true);
      ui->actionStopCamera->setEnabled(false);
      break;
  }
}

void MainWindow::setExposureCompensation(int index) {

  camera->exposure()->setExposureCompensation(index * 0.5);

}

void MainWindow::displayRecorderError() {

  QMessageBox::warning(this, tr("Capture error"), mediaRecorder->errorString());

}

void MainWindow::displayCameraError() {

  QMessageBox::warning(this, tr("Camera error"), camera->errorString());

}

void MainWindow::readyForCapture(bool ready) {
  ui->captureImage->setEnabled(ready);
}

void MainWindow::imageSaved(int id, const QString &fileName) {


    if(isSearch) // Yüz eşleştirme arama yapılıyor ise  openbr kütüphanesi kodları çağırılır
    {
        Q_UNUSED(id);
        Q_UNUSED(fileName);
        qDebug() << fileName;




        // Retrieve classes for enrolling and comparing templates using the
        // FaceRecognition algorithm
        QSharedPointer<br::Transform> transform =br::Transform::fromAlgorithm("FaceRecognition");
        QSharedPointer<br::Distance> distance =br::Distance::fromAlgorithm("FaceRecognition");

        // Initialize templates

        br::TemplateList target =br::TemplateList::fromGallery("C:\\Users\\Asus\\Desktop\\openbr_train_save\\");
        br::Template query(fileName);

        // Enroll templates
        br::Globals->enrollAll = true;  // Enroll 0 or more faces per image
        target >> *transform;
        br::Globals->enrollAll = true;  // Enroll exactly one face per image
        query >> *transform;

        // Compare templates
        QList<float> scores = distance->compare(target, query);

        // Print an example score



        qDebug() << "Target File Name::" << target[3].file.name;
        qDebug() << "Query File Name" << query.file.name;


        FileList fileList= target.files();

        for(File file:fileList)
        {
           qDebug() << "file::" << file.name;
        }

        for(float score:scores)
        {
           qDebug() << "score::" << score;
        }

        isCapturingImage = false;

          if (applicationExiting) {
              close();
          }

    }

}

void MainWindow::closeEvent(QCloseEvent *event) {

  if (isCapturingImage) {
    setEnabled(false);
    applicationExiting = true;
    event->ignore();
  }
  else {
    event->accept();
  }

  qDebug() << "Closing App";
  br::Context::finalize();

}
