#ifndef REG_H
#define REG_H

#include <QWidget>
#include <QTcpSocket>
#include <QDebug>
#include <QMessageBox>
namespace Ui {
class Reg;
}

class Reg : public QWidget
{
    Q_OBJECT

public:
    explicit Reg(QTcpSocket *socket,QWidget *parent = 0);
    ~Reg();

private:
    Ui::Reg *ui;
    QTcpSocket *client;
private slots:
    void sendRegDataSlot();
    void readSlot();
};

#endif // REG_H
