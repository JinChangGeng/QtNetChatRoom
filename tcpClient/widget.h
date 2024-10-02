#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDebug>
#include <QTcpSocket>
#include <QButtonGroup>
#include "chatroom.h"
#include "reg.h"
#include <QByteArray>
#include <QMessageBox>
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;
    QButtonGroup *group;
    ChatRoom *chatRoom;
    QTcpSocket *client;
private slots:
    void btnsClickedSlot(int);
    void readServerResponse();
    void connectedSlot();//连接成功的槽函数
    void disconnectedSlot();//连接失败和断开连接的槽函数

};

#endif // WIDGET_H
