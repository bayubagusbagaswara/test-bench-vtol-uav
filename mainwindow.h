#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QTimer>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
using namespace std;
using namespace  cv;


namespace Ui {

class MainWindow;

}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    int a=1;

public slots:

private slots:
    void readData();

    void updateLCD2(const QString reading_Thrust);
    void updateLCD1(const QString reading_RPM);

    void updateControlValue(QString);

    void on_verticalSlider_ESC_valueChanged(int value);
    void on_verticalSlider_Servo_valueChanged(int value);

    void on_pushButton_ESC_clicked();
    void on_pushButton_Servo_clicked();

    void on_pushButton_StartLog_clicked();
    void on_pushButton_StopLog_clicked();

    //void on_pushButton_openCamera_clicked();
    //void on_pushButton_closeCamera_clicked();

    //void update_window();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QFile *fileku;
    QTimer *timer;

    static const quint16 arduino_mega_vendor_id = 6790;
    static const quint16 arduino_mega_product_id = 29987;
    QString arduino_port_name;
    bool arduino_is_available;

    bool ButtonStartLog;
    bool ButtonStopLog;

    QByteArray serialData;
    QString serialBuffer;
    QString rpm;
    QString thrust;
    QStringList bufferSplit;

    cv::VideoCapture cap;

    cv::Mat frame;
    QImage qt_image;

};

#endif // MAINWINDOW_H


