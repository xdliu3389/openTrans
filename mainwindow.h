#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QtNetwork/QUdpSocket>
#include <QThread>
#define ipLength 256
namespace Ui {
class MainWindow;
class fileTransThread;
}
enum MessageType{Message, NewParticipant, PariticipantLeft, FileName, Refuse};
extern QString filePath;
class fileThread:public QThread
{
    Q_OBJECT

public:
    virtual void run();
};
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QStandardItemModel *model;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void sendMsg(MessageType type, QString serverAdress);
    void test();

protected:
    //void newParticipant(QString localHostName, QString ipAdress);
    //void participantLeft(QString localHostName, QString ipAdress);
    void keyPressEvent(QKeyEvent *event);

    QString getIp();
    QString getMessage();

private:
    Ui::MainWindow *ui;
    QString localIp;
    QString ips[ipLength];
    QString localHostNames[ipLength];
    QUdpSocket *udpSocket;
    qint16 port;
    qint16 peopleNums;
    void scanOnlineusers();
    void setUserList();
    void UdpInit();
    void timerInit();
    void dataInit();
    void freshUserList(MessageType type, QString ip, QString hostName);

private slots:
    void processPendingDatagrams();
    void on_sendButton_clicked();
    void timeToRefreshUserList();
    void on_selectFile_clicked();
};
extern MainWindow *w;

#endif // MAINWINDOW_H
