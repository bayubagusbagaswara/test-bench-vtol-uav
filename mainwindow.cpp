#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <stdio.h>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <string>
#include <QDebug>
#include <QMessageBox>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QString>
#include <QPixmap>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow (parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPixmap pix(":/resources/img/bhimasena.jpg");
    int w = ui->label_image->width();
    int h = ui->label_image->height();
    ui->label_image->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));

    ui->label_start_stop->setText("Information: \nMake sure the electrical connection. \nKeep careful when making measurements.");

    timer = new QTimer(this);
    serial = new QSerialPort(this);
    serialBuffer = "";
    arduino_is_available = false;
    arduino_port_name = "";

    //creation checkbox
    ui->checkBox_ESC->setChecked(false);
    ui->checkBox_Servo->setChecked(false);

    //creation and attribute of the slider
    ui->verticalSlider_ESC->setRange(0,100);
    ui->verticalSlider_Servo->setRange(0,100);

    //creation spinbox range
    ui->spinBox_ESC->setRange(0,8);
    ui->spinBox_Servo->setRange(0,8);

    //create display lcd number
    ui->lcdNumber_Thrust->display("----");
    ui->lcdNumber_RPM->display("----");

    //creation button log
    ui->pushButton_StartLog->setEnabled(true);
    ui->pushButton_StopLog->setEnabled(false);
    ButtonStartLog = false;
    ButtonStopLog = true;

    //creation button camera
    //ui->pushButton_openCamera->setEnabled(true);
   //ui->pushButton_closeCamera->setEnabled(false);


    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()){
            if(serialPortInfo.vendorIdentifier() == arduino_mega_vendor_id){
                if(serialPortInfo.productIdentifier() == arduino_mega_product_id){
                    arduino_port_name = serialPortInfo.portName();
                    arduino_is_available = true;
                }
            }
        }
    }
    if (arduino_is_available){
        //open and configure the serialport
        qDebug() << "Found the arduino port...\n";
        serial->setPortName(arduino_port_name);
        serial->open(QIODevice::ReadWrite);
        serial->setBaudRate(QSerialPort::Baud9600);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);
        QObject::connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
        QMessageBox::information(0, "Port Available","Arduino is connected");
    }
    else{
        //give error
        qDebug() << "Couldn't find the correct port for the arduino.\n";
        QMessageBox::warning(this, "Port error", "Couldn't find the Arduino!"); //jika arduino tidak tersambung
    }
}

MainWindow::~MainWindow()
{
    if(serial->isOpen()){
        serial->close();   //close the serial port if it's open.
    }
    delete ui;
}

void MainWindow::on_verticalSlider_ESC_valueChanged(int value)
{
    if(ui->checkBox_ESC->isChecked()){
        float angka = (value);
        ui->label_ESC->setText(QString("<span style=\" font-size:10pt;\">%1</span>").arg(value));
        MainWindow::updateControlValue(QString("e%1").arg(value));
        qDebug() << angka;
    }
    else
    {
        //do not excecute.
    }
}

void MainWindow::on_verticalSlider_Servo_valueChanged(int value)
{
    if(ui->checkBox_Servo->isChecked()){
        float angka = (value);
        ui->label_Servo->setText(QString("<span style=\" font-size:10pt;\">%1</span>").arg(value));
        MainWindow::updateControlValue(QString("s%1").arg(value));
        qDebug() << angka;
    }
    else
    {
        //do not excecute.
    }
}
void MainWindow::on_pushButton_ESC_clicked()
{
    if(ui->checkBox_ESC->isChecked()){
    QString spin1 = ui->spinBox_ESC->text();
    int spin_i = spin1.toInt();                     //pilih float atau integer
    spin_i = spin_i;
    MainWindow::updateControlValue(QString("c%1").arg(spin_i));
    qDebug() << spin_i;
    }
    else
    {
        //not execution
    }
}

void MainWindow::on_pushButton_Servo_clicked()
{
    if(ui->checkBox_Servo->isChecked()){
    QString spin2 = ui->spinBox_Servo->text();
    int spin_f = spin2.toInt();                     //data float or integer
    spin_f = spin_f;
    MainWindow::updateControlValue(QString("o%1").arg(spin_f));
    qDebug() << spin_f;
    }
    else
    {
        //not execution
    }

}

void MainWindow::updateControlValue(QString command) //Write function to update control value
{
    if(serial->isWritable()){
        serial->write(command.toStdString().c_str());
    }
    else {
        qDebug() << "Couldn't write to serial";
    }
}

void MainWindow::readData()
{
    bufferSplit = serialBuffer.split(" ; ");
    if(bufferSplit.length() < 3){
        serialData = serial->readAll();
        serialBuffer += QString::fromStdString(serialData.toStdString());
        serialData.clear();
    }
    else {
        serialBuffer = "";
        qDebug() << bufferSplit;
        rpm = bufferSplit[0];
        thrust = bufferSplit[1];
        MainWindow::updateLCD1(thrust);
        MainWindow::updateLCD2(rpm);
    }
    if(ButtonStartLog == true && ButtonStopLog == false){
        QFile LogFile("D:/data.csv");
        if(!LogFile.open(QFile::WriteOnly | QIODevice::Append)){
            //can't open log file
        }
        if (serial->bytesAvailable()){
            if(LogFile.isOpen()){
                QTextStream in(&LogFile);
                if(a==1){
                    in <<"\n RPM "<<" ; "<<" Thrust ";
                    in << rpm <<" ; "<< thrust;
                }
                else{
                    in << rpm <<" ; "<< thrust;
                }
                //in << rpm <<" ; "<< thrust;
                a++;
            }
            LogFile.close();
        }
    else
    {
        //fail to receive data
    }
    LogFile.close();
}
}

void MainWindow::updateLCD1(const QString reading_RPM)
{
    ui->lcdNumber_RPM->display(reading_RPM);
}
void MainWindow::updateLCD2(const QString reading_Thrust)
{
    ui->lcdNumber_Thrust->display(reading_Thrust);
}

void MainWindow::on_pushButton_StartLog_clicked()
{
    ButtonStartLog = true;
    ButtonStopLog = false;
    ui->pushButton_StartLog->setEnabled(false);
    ui->pushButton_StopLog->setEnabled(true);
    QMessageBox::information(0, "Logging Data", "Proses penyimpanan data sedang berjalan.");
}

void MainWindow::on_pushButton_StopLog_clicked()
{
    ButtonStartLog = false;
    ButtonStopLog = true;
    ui->pushButton_StartLog->setEnabled(true);
    ui->pushButton_StopLog->setEnabled(false);
    QMessageBox::information(0, "Logging Data", "Data telah berhasil disimpan.");
}

/*void MainWindow::on_pushButton_openCamera_clicked()
{
    ui->pushButton_openCamera->setEnabled(false);
    ui->pushButton_closeCamera->setEnabled(true);
    ui->label_camera_2->setText("Camera streaming is active.");

    cap.open("http://192.168.43.1:8080/video?x.mjpeg");        //IP camera streaming wifi
    //cap.open(0);

    qDebug() << "width: " << cap.get(CV_CAP_PROP_FRAME_WIDTH) << "height: " << cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 4672);     //lebar 640
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 3104);    //panjang 480
    qDebug() << "width_N: " << cap.get(CV_CAP_PROP_FRAME_WIDTH) << "height_N: " << cap.get(CV_CAP_PROP_FRAME_HEIGHT);

    if(!cap.isOpened()){
        cout << "camera is not open" << endl;
    }
    else {
        //cout << "camera is open" << endl;
        connect(timer, SIGNAL(timeout()), this, SLOT(update_window()));
        timer->start(20);
    }
}

void MainWindow::on_pushButton_closeCamera_clicked()
{
    ui->pushButton_closeCamera->setEnabled(false);
    ui->pushButton_openCamera->setEnabled(true);
    ui->label_camera_2->setText("Camera streaming is non active.");

    disconnect(timer, SIGNAL(timeout()), this, SLOT(update_window()));
    cap.release();
    cv::Mat image = cv::Mat::zeros(frame.size(),CV_8UC3);
    qt_image = QImage((const unsigned char*) (image.data), image.cols, image.rows, QImage::Format_RGB888);
    ui->label_camera->setPixmap(QPixmap::fromImage(qt_image));
    ui->label_camera->resize(ui->label_camera->pixmap()->size());

    //cout << "camera is closed" << endl;
}

void MainWindow::update_window()
{
    cap >> frame;
    cvtColor(frame, frame, CV_BGR2RGB);
    qt_image = QImage((const unsigned char*) (frame.data), frame.cols, frame.rows, QImage::Format_RGB888);
    ui->label_camera->setPixmap(QPixmap::fromImage(qt_image));
    ui->label_camera->resize(ui->label_camera->pixmap()->size());
}*/

