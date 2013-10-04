#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Qt LED Controller");

    mUdpSocket = new QUdpSocket(this);
    connect(mUdpSocket, SIGNAL(readyRead()),
            this, SLOT(readPendingDatagrams()));

    on_listenButton_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readPendingDatagrams()
{
    while (mUdpSocket->hasPendingDatagrams()) {
        QByteArray data;
        data.resize(mUdpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        mUdpSocket->readDatagram(data.data(), data.size(),
                                &sender, &senderPort);

        ui->terminalBrowser->append(data);
    }
}

void MainWindow::on_listenButton_clicked()
{
    mUdpSocket->bind(QHostAddress::Any, ui->listenPortBox->value());
}

void MainWindow::on_sendButton_clicked()
{
    QByteArray sendData;
    sendData.append(ui->sendEdit->text());

    QHostAddress address(ui->addressEdit->text());
    mUdpSocket->writeDatagram(sendData, address, ui->sendPortBox->value());

    ui->sendEdit->clear();
}

void MainWindow::on_autoButton_clicked()
{
    QByteArray sendData;
    sendData.append((char)LED_STATE_AUTO);
    appendGuiParameters(sendData);

    QHostAddress address(ui->addressEdit->text());
    mUdpSocket->writeDatagram(sendData, address, ui->sendPortBox->value());
}

void MainWindow::on_allOnButton_clicked()
{
    QByteArray sendData;
    sendData.append((char)LED_STATE_ALL_ON);
    appendGuiParameters(sendData);

    QHostAddress address(ui->addressEdit->text());
    mUdpSocket->writeDatagram(sendData, address, ui->sendPortBox->value());
}

void MainWindow::on_allOffButton_clicked()
{
    QByteArray sendData;
    sendData.append((char)LED_STATE_ALL_OFF);
    appendGuiParameters(sendData);

    QHostAddress address(ui->addressEdit->text());
    mUdpSocket->writeDatagram(sendData, address, ui->sendPortBox->value());
}

void MainWindow::on_resendButton_clicked()
{
    QByteArray sendData;
    sendData.append((char)LED_STATE_NONE);
    appendGuiParameters(sendData);

    QHostAddress address(ui->addressEdit->text());
    mUdpSocket->writeDatagram(sendData, address, ui->sendPortBox->value());
}

void MainWindow::appendGuiParameters(QByteArray &array)
{
    array.append((char)ui->redSlider->value());
    array.append((char)ui->greenSlider->value());
    array.append((char)ui->blueSlider->value());
    array.append((char)ui->intensitySlider->value());
    array.append((char)ui->posSlider->value());
}

void MainWindow::on_posSlider_valueChanged(int value)
{
    Q_UNUSED(value);

    QByteArray sendData;
    sendData.append((char)LED_STATE_SINGLE);
    appendGuiParameters(sendData);

    QHostAddress address(ui->addressEdit->text());
    mUdpSocket->writeDatagram(sendData, address, ui->sendPortBox->value());
}
