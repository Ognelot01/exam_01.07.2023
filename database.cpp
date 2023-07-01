#include "database.h"

DatabaseDestroyer::~DatabaseDestroyer(){
    delete p_instance;
}

void DatabaseDestroyer::initial(QSqlDatabase* p){
    p_instance = p;
}

Database::Database(){
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:/221-353_Ararat_Tunyan/myShop/data.sqlite"); //указать полный путь
    if (!db.open()) {
        qDebug() << "Failed to open database connection.";
    }
}

Database::~Database(){
    db.close();
}

Database* Database::getInstance(){
    if (!p_instance) {
        p_instance = new Database();
        destroyer.initial(&p_instance ->db);
    }
    return p_instance;
}

QString Database::execQuery(QString query)
{
        QSqlQuery sqlQuery(db);
        if (!sqlQuery.exec(query)) {
            qDebug() << "Failed to execute query: " << sqlQuery.lastError().text();
            return "";
        }
        QString result = "";
        while (sqlQuery.next()) {
            result = sqlQuery.value(0).toString();
        }
        return result;
}

Database * Database::p_instance;
DatabaseDestroyer Database::destroyer;
