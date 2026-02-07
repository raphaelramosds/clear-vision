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
}

