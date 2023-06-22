#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>
#include <QString>
#include <QStringList>

// Create a TCP socket
QTcpSocket tcpSocket;

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

QString IDTren;


void conectareServer()
{
    // Connect to the server
    QString serverAddress = "192.168.1.5"; // Server IP address
    quint16 serverPort = 8888; // Server port number

    tcpSocket.connectToHost(QHostAddress(serverAddress), serverPort);

    // Wait until the connection is established
    if (!tcpSocket.waitForConnected())
    {
        qDebug() << "Failed to connect to the server.";
    }
}
void trimiteLaServer(QString dataToSend)
{
    // Send data to the server
    tcpSocket.write(dataToSend.toUtf8());
    // Wait until the data is written
    if (!tcpSocket.waitForBytesWritten())
    {
        qDebug() << "Failed to send data to the server.";
    }

}
QString raspunsServer()
{
    // Read data from the server
    if (!tcpSocket.waitForReadyRead())
    {
        qDebug() << "Failed to receive data from the server.";
        //return a.exec();
    }

    QByteArray responseData = tcpSocket.readAll();
    QString receivedData = QString::fromUtf8(responseData);
    qDebug() << "Received data from server:" << receivedData;
    return receivedData;
}
void inchideConexiunea()
{
    // Close the connection
    tcpSocket.disconnectFromHost();
}

void MainWindow::on_pushButton_clicked()
{
    conectareServer();
    QString dataToSend =IDTren + "," + ui->lineEdit->text() + "," + ui->lineEdit_2->text() + "," + ui->lineEdit_3->text() + "," + ui->clasaBiletBox->currentText();
    trimiteLaServer(dataToSend);

    QString datePrimite = raspunsServer();
    int poz=0;
    QStringList tokens = datePrimite.split("_");
    QString nrvagon;
    QString tipvagon;
    QString clasavagon;
    QString compartiment;
    QString loc;
    QString afis;
    for (const QString& token : tokens)
    {
        poz++;
        if(token!="_")
        {
            if(poz==1)
                nrvagon=token;
            else if(poz==2)
                tipvagon=token;
            else if(poz==3)
                clasavagon=token;
            else if(poz==4)
                compartiment=token;
            else if(poz==5)
                loc=token;

        }



    }
     afis=afis+"Clasa:"+clasavagon+" Vagon:"+nrvagon+" Loc:"+compartiment+loc+"\0";
    ui->raspunsServerLabel->setText(afis);




    inchideConexiunea();
}
void MainWindow::populateComboBox()
{
    ui->listaRute->clear();
    conectareServer();
    QString dataToSend = "cereRute";
    trimiteLaServer(dataToSend);

    QString datePrimite = raspunsServer();
    QStringList tokens = datePrimite.split("\n");
      for (const QString& token : tokens)
    {
        if(token!="\n")
        ui->listaRute->addItem(token);
    }
      inchideConexiunea();
}
void MainWindow::on_listaRute_activated(int index)
{
    int verif = ui->listaRute->findText("RUTE");

    if (verif != -1) {
        populateComboBox();
    } else {
        QString rutaAleasa = ui->listaRute->currentText();
        IDTren = rutaAleasa.mid(0,4);
        QString dataToSend="detalii" + IDTren;
        conectareServer();
        trimiteLaServer(dataToSend);
        QString datePrimite = raspunsServer();
        QString oraplecare = datePrimite.mid(0,5);
        QString oradestinatie = datePrimite.mid(5,5);
        QString nrvagoane = datePrimite.at(datePrimite.length()-4);
        QString vagoanecl1 = datePrimite.at(datePrimite.length()-3);
        QString vagoanecl2 = datePrimite.at(datePrimite.length()-2);
        QString pretbilet = datePrimite.mid(10,datePrimite.length()-14);
        ui->detalii_cursa->setText("Detalii cursa aleasa:\nOra plecare: " + oraplecare + "\nOra destinatie: " + oradestinatie + "\nPret bilet intreg:" + pretbilet + "\nNumar vagoane: " + nrvagoane + "\nNumar vagoane clasa 1: " + vagoanecl1 + "\nNumar vagoane clasa 2: " + vagoanecl2);
        inchideConexiunea();

    }

}

