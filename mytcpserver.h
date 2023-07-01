#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSqlDatabase>
#include "database.h"

class MyTcpServer : public QObject
{
    Q_OBJECT
public:
    explicit MyTcpServer(QObject *parent = nullptr);
    ~MyTcpServer();

public slots:
    void slotNewConnection();
    void slotClientDisconnected();
    void slotServerRead();

private:
    QTcpServer *mTcpServer;
    QList<QTcpSocket *> mTcpSockets;
    QSqlDatabase mDatabase;
    int count_client = 0;

    void connectToDatabase();
    void processRequest(QTcpSocket *socket, const QString &request);
    void sendResponse(QTcpSocket *socket, const QString &response);
};

#endif // MYTCPSERVER_H
