#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <QAbstractItemView>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkInterface>
#include <QMessageBox>
#include <QKeyEvent>
#include <iostream>
#include <QTimer>
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    UdpInit();
    setUserList();
    dataInit();
    timerInit();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerInit() {
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeToSendParticipant()));
    timer->start(5000);
}
void MainWindow::dataInit() {
    peopleNums = 0;
}

void MainWindow::UdpInit() {
    udpSocket = new QUdpSocket(this);
    port = 33888;
    localIp = getIp();
    udpSocket->bind(port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
    sendMsg(NewParticipant, "");
}

void MainWindow::sendMsg(MessageType type, QString serverAdress) {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    QString localHostName = QHostInfo::localHostName();
    QString address = getIp();
    out << type << localHostName;

    switch(type) {
    case Message:
        if(ui->msgSend->toPlainText() == "") {
            QMessageBox::warning(0, tr("warning"), tr("The empty contents can't be send"), QMessageBox::Ok);
            ui->msgSend->setFocus();
            return ;
        }
        out << address << getMessage();
        break;
    case NewParticipant:
        out << address;
        break;
    default:
        break;
    }
    udpSocket->writeDatagram(data, data.length(), QHostAddress::Broadcast, port);
}

void MainWindow::processPendingDatagrams() {
    while(udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());
        QDataStream in(&datagram, QIODevice::ReadOnly);
        int messageType;
        in >> messageType;
        QString localHostName,ipAddress,message;
        switch(messageType)
        {
        case Message:
            in >>localHostName >> ipAddress >> message;
            if(ipAddress == localIp) {
                ui->msgDisplay->setTextColor(Qt::blue);
                ui->msgDisplay->setCurrentFont(QFont("Times New Roman", 8));
            } else {
                ui->msgDisplay->setTextColor(Qt::green);
                ui->msgDisplay->setCurrentFont(QFont("Times New Roman", 8));
            }
            ui->msgDisplay->append(ipAddress+" "+localHostName);
            ui->msgDisplay->setTextColor(Qt::black);
            ui->msgDisplay->setCurrentFont(QFont("Times New Roman", 12));
            ui->msgDisplay->append(message);
            ui->msgDisplay->append(" ");
            break;
        case NewParticipant:
            in >> localHostName >> ipAddress;
            model->setItem(peopleNums, 0, new QStandardItem(ipAddress));
            model->setItem(peopleNums, 1, new QStandardItem(localHostName));
            peopleNums++;
            break;
        default:
            break;
        }
    }
}

void MainWindow::freshUserList(MessageType type, QString ip, QString hostName) {
    int flag = 1;
    switch(type) {
    case NewParticipant:
        for(int i=0; i<peopleNums; i++) {
            if(ips[i].toStdString() == ip.toStdString()) {
                flag = 0;
                break;
            }
        }
        if(flag)
            ips[peopleNums++] = ip;
        break;
    case PariticipantLeft:
        for(int i=0; i<peopleNums; i++) {
            if(ips[i].toStdString() == ip.toStdString())
            {
                ips[i]= "";
                localHostNames[i] = "";
            }
        }
        peopleNums--;
        break;
    default:
        break;
    }
}

QString MainWindow::getIp()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list) {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
            return address.toString();
    }
    return 0;
}

QString MainWindow::getMessage() {
    QString msg = ui->msgSend->toPlainText();
    ui->msgSend->clear();
    ui->msgSend->setFocus();
    return msg;
}

void MainWindow::setUserList() {
    model = new QStandardItemModel();
    model->setColumnCount(2);
    model->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("ip"));
    model->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("localhost"));
    ui->userList->setModel(model);
    ui->userList->setColumnWidth(0,120);
    ui->userList->setColumnWidth(1,120);
    ui->userList->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainWindow::scanOnlineusers()
{
}

void MainWindow::timeToRefreshUserList()
{
    sendMsg(NewParticipant, "");
    for(int i=0; i<peopleNums; i++) {
        model->setItem(i, 0, new QStandardItem(ips[i]));
        model->setItem(i, 1, new QStandardItem(localHostNames[i]));
        i++;
    }
}
void MainWindow::on_sendButton_clicked()
{
    sendMsg(Message, "");
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if(event->modifiers() == Qt::ControlModifier && (event->key() == Qt::Key_Return)) {
        sendMsg(Message, "");
    }
}
