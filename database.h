#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>

class Database;

class DatabaseDestroyer{
private:
    QSqlDatabase * p_instance;
public:
    ~DatabaseDestroyer();
    void initial(QSqlDatabase * p);
};

class Database{
private:
    static Database* p_instance;
    static DatabaseDestroyer destroyer;
    QSqlDatabase db;

protected:
    Database();
    Database(const Database&) = delete;
    Database& operator = (const Database&) = delete;
    ~Database();

    friend class DatabaseDestroyer;
    friend class mytcpserver;

public:
    static Database* getInstance();
    QString execQuery(QString query);
};
