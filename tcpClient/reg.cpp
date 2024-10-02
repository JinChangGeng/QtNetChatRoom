#include "reg.h"
#include "ui_reg.h"

Reg::Reg(QTcpSocket *socket,QWidget *parent) :
    QWidget(parent),client(socket),
    ui(new Ui::Reg)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window);//设置为独立窗口

    connect(ui->pushButtonReg,SIGNAL(clicked())
            ,this,SLOT(sendRegDataSlot()));

    connect(client,SIGNAL(readyRead())
            ,this,SLOT(readSlot()));
}

Reg::~Reg()
{
    delete ui;
    delete client;
}

void Reg::sendRegDataSlot()
{
    //获取要发送的内容
    QString id = ui->lineEditId->text();
    QString password = ui->lineEditPassword->text();
    QByteArray sendBuffer = id.toUtf8();
    sendBuffer.prepend("R").append(":").append(password.toUtf8());
    client->write(sendBuffer);
    qDebug()<<sendBuffer;
}
//ToDo接收服务器发送的消息“用户已注册”弹出弹窗，注册成功，点击确定关闭注册窗口。
void Reg::readSlot()
{
    //接受内容比对
    //获取接收的内容
    //读取消息
    QByteArray readBuffer = client->readAll();
    QString textTemp(readBuffer);
    qDebug()<<textTemp;
    if(textTemp=="R|注册成功")
    {
        QMessageBox::information(this,"提示","注册成功");
        qDebug()<<"注册成功";
        disconnect(client,SIGNAL(readyRead())
                ,this,SLOT(readSlot()));
    }
    else if(textTemp=="R|注册失败")
    {
        QMessageBox::information(this,"提示","用户已存在");
        qDebug()<<"注册失败";
    }
}
