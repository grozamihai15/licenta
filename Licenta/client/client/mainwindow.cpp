#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    raspunsServerLabel = new QLabel(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    // Create a TCP socket
    QTcpSocket tcpSocket;

    // Connect to the server
    QString serverAddress = "192.168.1.5"; // Server IP address
    quint16 serverPort = 8888; // Server port number

    tcpSocket.connectToHost(QHostAddress(serverAddress), serverPort);

    // Wait until the connection is established
    if (!tcpSocket.waitForConnected())
    {
        qDebug() << "Failed to connect to the server.";
    }

    QString dataToSend =ui->comboBox->currentText() + "," + ui->lineEdit->text() + "," + ui->lineEdit_2->text() + "," + ui->lineEdit_3->text(); // Obține valorile din QLineEdit-uri și concatenează-le
    // Send data to the server
    tcpSocket.write(dataToSend.toUtf8());

    // Wait until the data is written
    if (!tcpSocket.waitForBytesWritten())
    {
        qDebug() << "Failed to send data to the server.";
       // return a.exec();
    }

    // Read data from the server
    if (!tcpSocket.waitForReadyRead())
    {
        qDebug() << "Failed to receive data from the server.";
        //return a.exec();
    }

    QByteArray responseData = tcpSocket.readAll();
    QString receivedData = QString::fromUtf8(responseData);
    ui->raspunsServerLabel->setText(receivedData);

    qDebug() << "Received data from server:" << receivedData;

    // Close the connection
    tcpSocket.disconnectFromHost();


}

