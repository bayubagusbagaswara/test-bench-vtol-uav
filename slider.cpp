#include "slider.h"

Slider::Slider()
{

}
void MainWindow::on_verticalSlider_thrust_valueChanged(int value)
{
    float angka = (value);
    ui->label_Thrust->setText(QString("<span style=\" font-size:10pt;\">%1</span>").arg(value));
    MainWindow::updateSliderValue(QString("t%1").arg(value));
    qDebug() << angka;

}

void MainWindow::on_verticalSlider_rpm_valueChanged(int value)
{
    float angka = (value);
    ui->label_rpm->setText(QString("<span style=\" font-size:10pt;\">%1</span>").arg(value));
    MainWindow::updateSliderValue(QString("r%1").arg(value));
    qDebug() << angka;

}

void MainWindow::updateSliderValue(QString command)
{
    if(arduino->isWritable()){
        arduino->write(command.toStdString().c_str());
    }else{
        qDebug() << "Couldn't write to serial";
    }
}


void MainWindow::on_spinBox_valueChanged(int arg1)
{

}

/*int IconSizeSpinBox::valueFromText(const QString &text) const
{
    QRegExp regExp(tr("(\\d+)(\\s*[xx]\\s*\\d+)?"));

    if (regExp.exactMatch(text))
    {
        return regExp.cap(1).toInt();
    } else{
        return 0;
    }
}
QString IconSizeSpinBox::textFromValue(int value) const
{
    return tr("%1 x %1").arg(value);
}
*/

void MainWindow::on_pushButton_clicked()
{
    QString value = ui->textEdit->toPlainText();
    float angka;
    angka =value.toFloat();
    float angka1 = angka*0.625;
    qDebug() << " angka: " << angka1;
}
