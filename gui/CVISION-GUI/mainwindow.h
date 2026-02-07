#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMultimedia>
#include <QtMultimediaWidgets>
#include <QtCore>
#include <QtWidgets>
#include <QtGui>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void durationChanged(qint64 duration);

    void positionChanged(qint64 duration);

    void on_loadVideoBtn_clicked();

    void on_durationHorizontalSlider_valueChanged(int value);

    void on_stopBtn_clicked();

    void on_playPauseBtn_clicked();

private:
    Ui::MainWindow *ui;

    QMediaPlayer *Player;

    QVideoWidget *Video;

    qint64 mDuration;

    bool IS_PAUSED = true;

    void updateDuration(qint64 duration);

};
#endif // MAINWINDOW_H
