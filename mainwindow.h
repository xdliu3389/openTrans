#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QtNetwork/QUdpSocket>
namespace Ui {
class MainWindow;
}

enum MessageType{Message, NewParticipant, PariticipantLeft, FileName, Refuse};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QStandardItemModel *model;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    //void newParticipant(QString localHostName, QString ipAdress);
    //void participantLeft(QString localHostName, QString ipAdress);
    void sendMsg(MessageType type, QString serverAdress);
    void keyPressEvent(QKeyEvent *event);

    QString getIp();
    QString getMessage();

private:
    Ui::MainWindow *ui;
    QString localIp;
    QUdpSocket *udpSocket;
    qint16 port;
    qint16 peopleNums;
    void scanOnlineusers();
    void setUserList();
    void UdpInit();

private slots:
    void processPendingDatagrams();
    void on_sendButton_clicked();

};

#endif // MAINWINDOW_H
