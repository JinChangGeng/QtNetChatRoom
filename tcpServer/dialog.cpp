#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    //连接数据库
    sqlConnect();
    //创建数据库表单
    sqlCreatTable();

    //创建服务器对象
    server = new QTcpServer(this);
    //开启监听
    bool result=server->listen(QHostAddress::Any,6699);
    if(!result)
    {
        ui->textBrowser->append("监听失败");
    }
    ui->textBrowser->append("监听成功，端口为：6699");

    //有客户端连接时发送信号，执行槽函数
    connect(server,SIGNAL(newConnection()),
            this,SLOT(newConnectedSlot()));
}
//析构函数
Dialog::~Dialog()
{
    if(server->isListening())
        server->close();
    delete ui;
    delete server;
}
//连接数据库
void Dialog::sqlConnect()
{
    //创建数据库对象(句柄)
    db=QSqlDatabase::addDatabase("QSQLITE");
    //设置数据库名称
    db.setDatabaseName("User.db");
    //打开数据库连接
    if(db.open())
    {
        qDebug()<<"连接成功";
    }
    else
    {
        //返回错误信息
        QSqlError errorInfo=db.lastError();
        //提取错误文本
        QString errorText=errorInfo.text();
        // 展示错误
        QMessageBox::critical(this,"错误",errorText);
    }
}
//创建数据库表单,自动去连接的数据库内创建表单
void Dialog::sqlCreatTable()
{
    //存储sql语句
    QString creatTable="CREATE TABLE user(id Text PRIMARY KEY,password TEXT)";
    QString creatTable2="CREATE TABLE history(id Text,message TEXT,time TEXT)";
    qDebug()<<creatTable;
    //创建QSqlQuery操作类
    QSqlQuery sqlExec;
    if(sqlExec.exec(creatTable))
    {
        qDebug()<<"创建成功";
    }
    else
    {
        // 获得错误信息的封装类
        QSqlError info = sqlExec.lastError();
        // 提取错误信息
        QString text = info.text();
        // 展示错误
        qDebug() << "建表失败" << text ;
    }
    if(sqlExec.exec(creatTable2))
    {
        qDebug()<<"创建成功";
    }
    else
    {
        // 获得错误信息的封装类
        QSqlError info = sqlExec.lastError();
        // 提取错误信息
        QString text = info.text();
        // 展示错误
        qDebug() << "建表失败" << text ;
    }
}
//新客户端连接
void Dialog::newConnectedSlot()
{
    //得到服务器创建的socket套接字并存储
    socket = server->nextPendingConnection();
    //存储容器
    socketList.append(socket);
    //人员数+1;
    //接收客户端掉线信息
    connect(socket,SIGNAL(disconnected()),
            this,SLOT(disConnectedSlot()));

    //遍历发送客户端消息、接收客户端消息,数据库操作
    connect(socket,SIGNAL(readyRead()),
            this,SLOT(sendMessageSlot()));

    //获取客户端ip地址
    QString ip=socket->peerAddress().toString();
    qint64 port=socket->peerPort();
    //输出提示
    ip.prepend("客户端已连接").append(":").append(QString::number(port));
    ui->textBrowser->append(ip);
}
//客户端断连
void Dialog::disConnectedSlot()
{
    //拿到发射者的socket
    socket = (QTcpSocket*)sender();
    //遍历判断是第几个socket文件描述符掉线
    for(int i=0;i<socketList.size();i++)
    {
        if(socket==socketList.at(i))
        {
            // 获取对方IP
            QString ip = socketList.at(i)->peerAddress().toString();
            // 获取对方端口号
            quint16 port = socketList.at(i)->peerPort();
            ip.prepend("客户端已断开！").append(":").append(QString::number(port));
            ui->textBrowser->append(ip);
            //移除当前客户端的描述符，其余的自动向前移动？
            socketList.removeAt(i);
        }
    }
}
//遍历发送给所有客户端消息
void Dialog::sendMessageSlot()
{
    //读取消息
    socket = (QTcpSocket*)sender();
    QByteArray readBuffer = socket->readAll();
    QString textTemp(readBuffer);
    if (textTemp.isEmpty()) {
        return;
    }
    //注册 ToDo查询数据库比对
    if(textTemp.at(0)=='R')
    {
        QString text=textTemp.mid(1);//去掉R
        ui->textBrowser->append(text);
        int i,j;
        //拆分id和password
        for(i=0;i<text.size();i++)
        {
            if(text.at(i)==':')
            {
                j=i;
            }
        }
        QString id=text.mid(0,j);
        QString password=text.mid(j+1);
        qDebug()<<"11111";
        selectReg(id,password);
        qDebug()<<"22222";
    }
    //登录
    if(textTemp.at(0)=='L')
    {
        QString text=textTemp.mid(1);//去掉L
        ui->textBrowser->append(text);
        int i,j;
        //拆分id和password
        for(i=0;i<text.size();i++)
        {
            if(text.at(i)==':')
            {
                j=i;
            }
        }
        QString id=text.mid(0,j);
        QString password=text.mid(j+1);
        //查询数据库中是否有，如果有则登录成功，如果没有登录失败
        //预处理的SQL语句
        QString insertSql = "SELECT * FROM user WHERE id=:id AND password=:password;";
        //绑定数据
        QSqlQuery sqlInsert;
        //预处理SQL语句,把接收到的参数替换占位符
        sqlInsert.prepare(insertSql);
        sqlInsert.bindValue(":id",id);
        sqlInsert.bindValue(":password",password);
        if(sqlInsert.exec())
        {
            if(sqlInsert.next())//循环取出
            {
                QString sqlID=sqlInsert.value("id").toString();
                QString sqlPassword=sqlInsert.value("password").toString();
                if(id==sqlID&&password==sqlPassword)
                {
                    ui->textBrowser->append("用户登录成功");
                    QString tips="用户登录成功";
                    //发送给客户端登录成功的标志
                    QByteArray sendBuffer = tips.toUtf8();
                    sendBuffer.prepend("T");
                    qDebug()<<sendBuffer;
                    socket->write(sendBuffer);
                }
            }
            else
            {
                ui->textBrowser->append("用户登录失败");
                QString tips="用户登录失败";
                //发送给客户端登录失败的标志
                QByteArray sendBuffer = tips.toUtf8();
                sendBuffer.prepend("T");
                socket->write(sendBuffer);
            }
        }
        else
        {
            ui->textBrowser->append("语句执行失败");
        }
    }
    //遍历所有客户端发送消息
    if(textTemp.at(0)=='C')
    {
        QString text=textTemp.mid(1);//去掉C
        QByteArray sendBuffer = text.toUtf8();
        QByteArray Message=printMsg(sendBuffer);
        // 遍历发送给所有客户端，除自己外
        for (int i = 0; i < socketList.size(); ++i)
        {
            socketList.at(i)->write(Message);
        }
    }
    //查询历史记录
    if(textTemp.at(0)=='H')
    {
        QList<QByteArray> textList =readBuffer.split('|');
        QByteArray flag=textList[0];
        QByteArray text1=textList[1];
        QString textMessage(text1);
        //查询历史记录函数
        selectHistory(textMessage);
        //发送给查询的客户端
    }
}
//发送消息的格式
QByteArray Dialog::printMsg(QByteArray msg)
{

    QList<QByteArray> textList =msg.split('/');

    QByteArray nameTemp=textList[0];
    QString name(nameTemp);
    QByteArray text=textList[1];
    QString textMessage(text);
    // 获得当前时间
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //插入到消息记录
    insertHistorySql(name,textMessage,time);
    time.prepend(name+" ").append("\n"+textMessage);
    QByteArray timetime=time.toUtf8();
    return timetime;
}
//添加消息到历史记录
void Dialog::insertHistorySql(QString name,QString text,QString time)
{
    //预处理的SQL语句
    QString insertSql = "INSERT INTO history VALUES(?,?,?);";
    //绑定数据
    QSqlQuery sqlInsert; //这个类才能执行sql语句，必须要有
    //预处理SQL语句,把接收到的参数替换占位符
    sqlInsert.prepare(insertSql);
    sqlInsert.addBindValue(name);
    sqlInsert.addBindValue(text);
    sqlInsert.addBindValue(time);
    //执行预处理绑定后的SQL语句
    sqlInsert.exec();
}
//查找历史记录
void Dialog::selectHistory(QString message)
{
    //ODBC预处理SQL语句
    QString selectLike = "SELECT * FROM history WHERE message LIKE ?;";
    //获得用户输入
    if(message == "")
    {
        return;
    }
    QSqlQuery sqlSelectLike;
    //预处理
    sqlSelectLike.prepare(selectLike);
    sqlSelectLike.addBindValue(message.append("%").prepend("%"));
    QByteArray sendBuffer;
    if(sqlSelectLike.exec())
    {

        while(sqlSelectLike.next())// 循环取出
        {
            QString name=sqlSelectLike.value(0).toString();
            QString message=sqlSelectLike.value(1).toString();
            QString time=sqlSelectLike.value(2).toString();
            QString text=name+" "+time+"\n "+message+'\n';
            //发送
            sendBuffer.append(text.toUtf8()); // 累加到 sendBuffer
        }
    }
    socket->write("H|"+sendBuffer);
}
//查询注册是否有相同用户名
void Dialog::selectReg(QString id, QString password)
{
    // ODBC预处理SQL语句
    QString selectLike = "SELECT * FROM user WHERE id=:id;";
    QSqlQuery sqlSelectLike;

    // 预处理
    sqlSelectLike.prepare(selectLike);
    sqlSelectLike.bindValue(":id", id);
    if (sqlSelectLike.exec())
    {

        // 检查是否存在相同的用户名
        if (sqlSelectLike.next())
        {
            // 用户名已存在，注册失败
            QString text = "R|注册失败";
            QByteArray sendBuffer = text.toUtf8();
            socket->write(sendBuffer);
            qDebug() << sendBuffer;

        }
        else
        {
            // 用户名不存在，可以注册
            QString insertSql = "INSERT INTO user VALUES(?,?);";
            QSqlQuery sqlInsert;
            sqlInsert.prepare(insertSql);
            sqlInsert.addBindValue(id);
            sqlInsert.addBindValue(password);

            if (sqlInsert.exec())
            {
                // 注册成功
                QString text = "R|注册成功";
                QByteArray sendBuffer = text.toUtf8();
                socket->write(sendBuffer);
            }
            else
            {
                // 插入失败
                QString text = "R|注册失败";
                QByteArray sendBuffer = text.toUtf8();
                socket->write(sendBuffer);
            }
        }
    }
    else
    {
        // 查询执行失败
        QString text = "R|查询失败";
        QByteArray sendBuffer = text.toUtf8();
        socket->write(sendBuffer);
    }
}
