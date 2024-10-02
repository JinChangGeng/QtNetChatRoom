#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QByteArray>
#include <QSqlError>
#include <QMessageBox>
#include <QList>
#include <QDateTime>
#include <QTextStream>
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private:
    Ui::Dialog *ui;
    QTcpServer *server;
    QTcpSocket* socket = NULL;
    QList<QTcpSocket *>socketList;
    QSqlDatabase db;
    void sqlConnect();//连接数据库
    void sqlCreatTable();//创建表
    QByteArray printMsg(QByteArray);
    void insertHistorySql(QString,QString,QString);
    void selectHistory(QString);
    void selectReg(QString,QString);

private slots:
    void newConnectedSlot();//新连接
    void disConnectedSlot();//断开连接
    void sendMessageSlot();//发送给所有客户端消息
};

#endif // DIALOG_H
