#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QDebug>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Player = new QMediaPlayer();

    ui->playPauseBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->stopBtn->setIcon(style()->standardIcon(QStyle::SP_MediaStop));

    connect(Player, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    connect(Player, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loadVideoBtn_clicked()
{
    // Formatos suportados
    QString filter = "Video (*.mp4)";
    QString fileName = QFileDialog::getOpenFileName(this, "Selecione um video", QDir::homePath(), filter);
    if (!fileName.isEmpty()) {
        ui->loadVideoLabel->setText(fileName);
        cv::VideoCapture cap(fileName.toStdString());
        if (!cap.isOpened()) {
            qDebug() << "Erro ao abrir o video: " << fileName;
        }
        cv::Mat frame;
        double width, height;
        width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
        height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
        qDebug() << "Dimensoes do frame: " << width << " x " << height;
        cap.release();
    }

    Video = new QVideoWidget();
    Video->setGeometry(5, 5, ui->videoGroupBox->width(), ui->videoGroupBox->height());
    Video->setParent(ui->videoGroupBox);
    Player->setVideoOutput(Video);
    Player->setSource(QUrl(fileName));
    Video->setVisible(true);
    Video->show();
}


void MainWindow::on_durationHorizontalSlider_valueChanged(int value)
{
    Player->setPosition(value*1000);
}


void MainWindow::on_stopBtn_clicked()
{
    Player->stop();
}


void MainWindow::on_playPauseBtn_clicked()
{
    if (IS_PAUSED == true) {
        IS_PAUSED = false;
        Player->play();
        ui->playPauseBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    } else {
        IS_PAUSED = true;
        ui->playPauseBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        Player->pause();
    }
}

void MainWindow::updateDuration(qint64 duration)
{
    if (duration || mDuration) {
        QTime currentTime((duration / 3600) % 60, (duration / 60) % 60,
                          duration % 60, (duration * 1000) % 1000);
        QTime totalTime((mDuration / 3600) % 60, (mDuration / 60) % 60,
                        mDuration % 60, (mDuration * 1000) % 1000);
        QString format = "mm:ss";
        ui->currentTimeLabel->setText(currentTime.toString(format));
        ui->totalTimeLabel->setText(totalTime.toString(format));
    }
}

void MainWindow::positionChanged(qint64 duration)
{
    if (!ui->durationHorizontalSlider->isSliderDown())
    {
        ui->durationHorizontalSlider->setValue(duration / 1000);
    }
    this->updateDuration(duration / 1000);
}

void MainWindow::durationChanged(qint64 duration)
{
    mDuration = duration / 1000;
    ui->durationHorizontalSlider->setMaximum(mDuration);
}
