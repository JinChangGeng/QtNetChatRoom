#ifndef CHATROOM_H
#define CHATROOM_H

#include <QWidget>
#include <QDebug>
#include <QTcpSocket>
#include <QDateTime>
#include <QCloseEvent>
#include "history.h"
#include <QTextStream>
#include <QTimer>
namespace Ui {
class ChatRoom;
}

class ChatRoom : public QWidget
{
    Q_OBJECT

public:
    explicit ChatRoom(QTcpSocket *socket,QWidget *parent = 0);
    ~ChatRoom();

private:
    Ui::ChatRoom *ui;
    QTcpSocket *client;
    History *history;
    QTimer *timer;
    void printMsg(QString);
private slots:
    void sendBtnClickedSlot();
    void readMessageSlot();
    void historyClickedSlot();
    void closeClickedSlot();
    void clearClickedSlot();
    void timeoutSlot();


protected:
    void closeEvent(QCloseEvent * event);

};

#endif // CHATROOM_H
