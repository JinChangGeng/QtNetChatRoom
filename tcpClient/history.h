#ifndef HISTORY_H
#define HISTORY_H

#include <QWidget>
#include <QTcpSocket>
#include <QByteArray>
#include <QDebug>
namespace Ui {
class History;
}

class History : public QWidget
{
    Q_OBJECT

public:
    explicit History(QTcpSocket *socket,QWidget *parent = 0);
    ~History();
    // 添加接收历史记录的方法
    void receiveHistory(const QString &historyData);

private:
    Ui::History *ui;
    QTcpSocket *client;
private slots:
    void btnClickedSlot();
    void lineTextChangedSlot(QString);
};

#endif // HISTORY_H
