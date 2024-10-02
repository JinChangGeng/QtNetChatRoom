#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    //置顶、关闭缺省标题栏
    this->setWindowFlags(Qt::WindowStaysOnTopHint|Qt::CustomizeWindowHint);
    group = new QButtonGroup(this);
    group->addButton(ui->pushButtonClose,1);//关闭按钮
    group->addButton(ui->pushButtonConnect,2);//连接按钮
    group->addButton(ui->pushButtonLogin,3);//登录按钮
    group->addButton(ui->pushButtonReg,4);//注册按钮
    group->addButton(ui->radioButton,5);
    ui->pushButtonLogin->setEnabled(false);
    ui->pushButtonReg->setEnabled(false);

    connect(group,SIGNAL(buttonClicked(int)),this,SLOT(btnsClickedSlot(int)));


    //创建客户端对象
    client =new QTcpSocket(this);

    //使用套接字判断是否连接成功
    connect(client,SIGNAL(connected()),
            this,SLOT(connectedSlot()));
    connect(client,SIGNAL(disconnected()),
            this,SLOT(disconnectedSlot()));
}

Widget::~Widget()
{
    if(client->isOpen())
        client->close();
    delete ui;
    delete client;
}

void Widget::btnsClickedSlot(int id)
{
    if(id==1)
    {
        this->close();
    }
    else if(id==2)
    {

        //获取ip和port
        QString ip=ui->lineEditIp->text();
        QString portString=ui->lineEditPort->text();
        //QString→int
        bool result=false;
        int port=portString.toInt(&result,10);
        if(result)
            qDebug()<<"转换成功"<<port;
        else
            qDebug()<<"转换失败";
        //连接到服务器
        client->connectToHost(ip,port);
        if(ui->pushButtonConnect->text()=="断开连接")
        {
            client->close();
        }

    }
    else if(id==3)
    {
        //发送id和password
        //获取要发送的内容
        QString id = ui->lineEditId->text();
        QString password = ui->lineEditPassword->text();
        QByteArray sendBuffer = id.toUtf8();
        sendBuffer.prepend("L").append(":").append(password.toUtf8());
        client->write(sendBuffer);

        // 等待readyRead信号时才读取数据
        connect(client, SIGNAL(readyRead()), this, SLOT(readServerResponse()));
    }
    else if(id==4)
    {
        Reg *reg = new Reg(client,this);
        reg->show();

    }
    else if(id==5)
    {
        ui->pushButtonLogin->setEnabled(true);
    }
}
void Widget::readServerResponse()
{
    //获取接收的内容
    //读取消息
    QByteArray readBuffer = client->readAll();
    QString textTemp(readBuffer);
    if(textTemp=="T用户登录成功")
    {
        chatRoom = new ChatRoom(client,this);
        chatRoom->show();
        this->hide();
    }
    else if(textTemp=="T用户登录失败")
    {
        QMessageBox::information(this,"提示","登陆失败");
        qDebug()<<"登陆失败";
    }
    disconnect(client, SIGNAL(readyRead()), this, SLOT(readServerResponse()));
}

void Widget::connectedSlot()
{
    ui->pushButtonConnect->setText("断开连接");
    ui->pushButtonLogin->setEnabled(true);
    ui->pushButtonReg->setEnabled(true);

}

void Widget::disconnectedSlot()
{
    ui->pushButtonConnect->setText("连接");
    client->close();
}
