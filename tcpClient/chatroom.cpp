#include "chatroom.h"
#include "ui_chatroom.h"

ChatRoom::ChatRoom(QTcpSocket *socket,QWidget *parent) :
    QWidget(parent),client(socket),
    ui(new Ui::ChatRoom)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window
                         |Qt::WindowStaysOnTopHint|Qt::CustomizeWindowHint);//设置为独立窗口
    // 手动刷新一次时间
    timeoutSlot();

    // 创建定时器对象
    timer = new QTimer(this);
    // 设置为周期性
    timer->setSingleShot(false);
    // 设置间隔时间
    timer->setInterval(1000);
    // 建立信号槽连接
    connect(timer,SIGNAL(timeout()),
            this,SLOT(timeoutSlot()));
    // 启动
    timer->start();

    //连接按钮槽函数
    connect(ui->pushButtonSend,SIGNAL(clicked()),
            this,SLOT(sendBtnClickedSlot()));

    //接收信息槽函数
    connect(client,SIGNAL(readyRead()),
            this,SLOT(readMessageSlot()));

    //历史记录
    connect(ui->pushButtonHistory,SIGNAL(clicked()),
            this,SLOT(historyClickedSlot()));

    //关闭按钮
    connect(ui->pushButtonClose,SIGNAL(clicked()),
            this,SLOT(closeClickedSlot()));

    //清屏按钮
    connect(ui->pushButtonClear,SIGNAL(clicked()),
            this,SLOT(clearClickedSlot()));
}

ChatRoom::~ChatRoom()
{
    delete ui;
    delete client;
}
void ChatRoom::sendBtnClickedSlot()
{
    QString name = ui->lineEditName->text();
    //获取输入的字符
    QString sendText=ui->textEditSend->toPlainText();
    sendText.prepend(name+"/");
    QByteArray sendBuffer = sendText.toUtf8();
    sendBuffer.prepend("C");
    //发送给服务器
    client->write(sendBuffer);
    //清空发送消息框
    ui->textEditSend->clear();
}
//获取服务器发送的消息
void ChatRoom::readMessageSlot()
{
    //读取服务器发送的消息
    QByteArray readBuffer = client->readAll();
    QString text(readBuffer);
    qDebug()<<"text"<<text;
    // 判断消息类型
    if (text.at(0)=='H')
    {
        QList<QByteArray> textList =readBuffer.split('|');
        QByteArray text1=textList[1];
        QString textMessage(text1);
        // 如果是历史记录，传递给 History 窗口处理
        if (history)
        {
            history->receiveHistory(textMessage);
        }
    }
    else
    {
        //显示到消息对话框
        ui->textEditAccept->append(text);
    }
}
//新建历史记录窗口
void ChatRoom::historyClickedSlot()
{
    history = new History(client,this);
    history->show();
}
//关闭聊天
void ChatRoom::closeClickedSlot()
{
    this->close();
}
//清屏
void ChatRoom::clearClickedSlot()
{
    ui->textEditAccept->clear();
}
//关闭事件
void ChatRoom::closeEvent(QCloseEvent *event)
{
    QWidget *parentwidget = parentWidget();
    while(parentwidget!=NULL)
    {
        parentwidget->close();
        parentwidget=parentwidget->parentWidget();
    }
    event->accept();
}
void ChatRoom::timeoutSlot()
{
    // 获得当前时间
    QString time = QDateTime::currentDateTime().toString("hh:mm:ss");
    // 展示
    ui->lcdNumber->display(time);
}
