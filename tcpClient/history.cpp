#include "history.h"
#include "ui_history.h"

History::History(QTcpSocket *socket,QWidget *parent) :
    QWidget(parent),client(socket),
    ui(new Ui::History)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window);

    connect(ui->pushButton,SIGNAL(clicked()),
            this,SLOT(btnClickedSlot()));

    connect(ui->lineEdit,SIGNAL(textChanged(QString)),
            this,SLOT(lineTextChangedSlot(QString)));
}

History::~History()
{
    delete ui;
}

void History::btnClickedSlot()
{
    //发送一下查询标志
    //获取查询的记录
    QString text=ui->lineEdit->text();
    // 添加标志位 'H:'
    text.prepend("H|");
    QByteArray buffer = text.toUtf8();
    client->write(buffer);
    qDebug()<<"查询历史记录："<<buffer;
}
////模糊查询
void History::lineTextChangedSlot(QString historyData)
{
    // 当输入内容变化时，发送模糊查询请求
        historyData.prepend("H|");
        QByteArray buffer = historyData.toUtf8();
        client->write(buffer);
        qDebug() << "模糊查询：" << buffer;
}
void History::receiveHistory(const QString &historyData)
{
    ui->textBrowserHistory->clear(); // 清空旧记录
    ui->textBrowserHistory->append(historyData);

}
