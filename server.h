#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QStringList>

struct Product {
    QString name;
    QString type;
    float price;
};

class Server : public QObject {
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);
    void start();

private slots:
    void newConnection();
    void socketReadyRead();
    void handleMinPrice(QTcpSocket *socket);
    void handleExpensiveInType(QTcpSocket *socket, const QString &type);
    void handleMinPriceInType(QTcpSocket *socket, const QString &type);
    void handleExpensive(QTcpSocket *socket);

private:
    QTcpServer *server;
    QList<QTcpSocket *> clients;
    QList<Product> products;

    void sendResponse(QTcpSocket *socket, const QString &response);
};

#endif // SERVER_H
