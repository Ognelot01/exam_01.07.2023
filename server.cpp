#include "server.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent)
{
    mTcpServer = new QTcpServer(this);
    connect(mTcpServer, &QTcpServer::newConnection, this, &MyTcpServer::slotNewConnection);

    if (!mTcpServer->listen(QHostAddress::Any, 33333)) {
        qDebug() << "Server is not started";
    } else {
        qDebug() << "Server is started";
    }

    connectToDatabase();
}

MyTcpServer::~MyTcpServer()
{
    for (QTcpSocket *socket : mTcpSockets) {
        socket->close();
        socket->deleteLater();
    }

    mTcpServer->close();
}

void MyTcpServer::slotNewConnection()
{
    QTcpSocket *socket = mTcpServer->nextPendingConnection();
    mTcpSockets.append(socket);

    connect(socket, &QTcpSocket::readyRead, this, &MyTcpServer::slotServerRead);
    connect(socket, &QTcpSocket::disconnected, this, &MyTcpServer::slotClientDisconnected);
}

void MyTcpServer::slotClientDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        mTcpSockets.removeOne(socket);
        socket->close();
        socket->deleteLater();
    }
}

void MyTcpServer::slotServerRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        while (socket->bytesAvailable() > 0) {
            QByteArray requestData = socket->readAll();
            QString request = QString::fromUtf8(requestData).trimmed();

            processRequest(socket, request);
        }
    }
}

void MyTcpServer::connectToDatabase()
{
    mDatabase = QSqlDatabase::addDatabase("QSQLITE");
    mDatabase.setDatabaseName("products.db");

    if (!mDatabase.open()) {
        qDebug() << "Database connection error:" << mDatabase.lastError().text();
    } else {
        qDebug() << "Database connected";
    }
}

void MyTcpServer::processRequest(QTcpSocket *socket, const QString &request)
{
    QString response;
    QStringList tokens = request.split(QRegExp("\\s+"), Qt::SkipEmptyParts);

    if (tokens.isEmpty()) {
        response = "Invalid command";
    } else {
        QString command = tokens[0].toLower();
        QSqlQuery query(mDatabase);

        if (command == "min_price") {
            query.prepare("SELECT * FROM products ORDER BY price ASC LIMIT 1");
            if (query.exec() && query.next()) {
                response = query.record().value("name").toString();
            }
        } else if (command == "expensive_in_type") {
            if (tokens.size() == 2) {
                QString type = tokens[1];
                query.prepare("SELECT * FROM products WHERE type = :type AND price > (SELECT AVG(price) FROM products WHERE type = :type)");
                query.bindValue(":type", type);

                if (query.exec()) {
                    while (query.next()) {
                        response += query.record().value("name").toString() + "\n";
                    }
                }
            } else {
                response = "Invalid command";
            }
        } else if (command == "min_price_in_type") {
            if (tokens.size() == 2) {
                QString type = tokens[1];
                query.prepare("SELECT * FROM products WHERE type = :type ORDER BY price ASC LIMIT 1");
                query.bindValue(":type", type);

                if (query.exec() && query.next()) {
                    response = query.record().value("name").toString();
                }
            } else {
                response = "Invalid command";
            }
        } else if (command == "expensive") {
            query.prepare("SELECT * FROM products WHERE price > (SELECT AVG(price) FROM products)");
            if (query.exec()) {
                while (query.next()) {
                    response += query.record().value("name").toString() + "\n";
                }
            }
        } else {
            response = "Unknown command";
        }
    }

    sendResponse(socket, response);
}

void MyTcpServer::sendResponse(QTcpSocket *socket, const QString &response)
{
    QByteArray responseData = response.toUtf8();
    socket->write(responseData);
    socket->flush();
}
