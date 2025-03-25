#include "Telephone_directory.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QDebug>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>

Telephone_directory::Telephone_directory() {
    db = QSqlDatabase::addDatabase("QSQLITE"); //подключение к QSQLITE
    db.setDatabaseName("contacts.db"); //устанавливаю имя базы данных

    if (!db.open()) {
        qDebug() << "Ошибка подключения к базе данных:" << db.lastError().text();
    }
}

Telephone_directory::~Telephone_directory() {
    if (db.isOpen()) {
        db.close();//закрытиие соединения с базой данных при уничтожении объекта
    }
}

void Telephone_directory::Add(const std::string& first_name, const std::string& second_name, const std::string& third_name,
                              const std::string& number, const std::string& address, const std::string& birthday,
                              const std::string& email) {
    QSqlQuery query;
    //подготовка для добавления данных в таблицу
    query.prepare("INSERT INTO contacts (first_name, second_name, third_name, phone, email, address, birthday) "
                  "VALUES (:first_name, :second_name, :third_name, :phone, :email, :address, :birthday)");
    //привязка значений
    query.bindValue(":first_name", QString::fromStdString(first_name));
    query.bindValue(":second_name", QString::fromStdString(second_name));
    query.bindValue(":third_name", QString::fromStdString(third_name));
    query.bindValue(":phone", QString::fromStdString(number));
    query.bindValue(":email", QString::fromStdString(email));
    query.bindValue(":address", QString::fromStdString(address));
    query.bindValue(":birthday", QString::fromStdString(birthday));
    //выполнение sql запроса
    if (!query.exec()) {
        qDebug() << "Ошибка добавления контакта:" << query.lastError().text();
    }
}

void Telephone_directory::Delete(const std::string& second_name) {
    QSqlQuery query;
    query.prepare("DELETE FROM contacts WHERE second_name = :second_name");
    query.bindValue(":second_name", QString::fromStdString(second_name));

    if (!query.exec()) {
        qDebug() << "Ошибка удаления контакта:" << query.lastError().text();
    }
}

void Telephone_directory::Clear() {
    QSqlQuery query;
    if (!query.exec("DELETE FROM contacts")) {
        qDebug() << "Ошибка очистки базы данных:" << query.lastError().text();
    }
}

std::string Telephone_directory::getAllContacts() {
    QSqlQuery query("SELECT * FROM contacts");
    std::string result;

    while (query.next()) {
        result += "Имя: " + query.value("first_name").toString().toStdString() + "\n";
        result += "Фамилия: " + query.value("second_name").toString().toStdString() + "\n";
        result += "Отчество: " + query.value("third_name").toString().toStdString() + "\n";
        result += "Телефон: " + query.value("phone").toString().toStdString() + "\n";
        result += "Email: " + query.value("email").toString().toStdString() + "\n";
        result += "Адрес: " + query.value("address").toString().toStdString() + "\n";
        result += "День рождения: " + query.value("birthday").toString().toStdString() + "\n";
        result += "------------------------\n";
    }

    return result;
}

std::string Telephone_directory::findContact(const std::string& second_name) {
    QSqlQuery query;
    query.prepare("SELECT * FROM contacts WHERE second_name = :second_name");
    query.bindValue(":second_name", QString::fromStdString(second_name));

    if (query.exec() && query.next()) {
        std::string result;
        result += "Имя: " + query.value("first_name").toString().toStdString() + "\n";
        result += "Фамилия: " + query.value("second_name").toString().toStdString() + "\n";
        result += "Отчество: " + query.value("third_name").toString().toStdString() + "\n";
        result += "Телефон: " + query.value("phone").toString().toStdString() + "\n";
        result += "Email: " + query.value("email").toString().toStdString() + "\n";
        result += "Адрес: " + query.value("address").toString().toStdString() + "\n";
        result += "День рождения: " + query.value("birthday").toString().toStdString() + "\n";
        return result;
    }

    return "";
}bool Telephone_directory::isValidEmail(const QString& email) {
    QRegularExpression regex(R"(^[a-zA-Z0-9._%+-]+@(gmail\.com|mail\.ru|mail\.com)$)");
    return regex.match(email).hasMatch();
}

bool Telephone_directory::isValidPhone(const QString& phone) {
    QRegularExpression regex(R"(^(\+7|8)\d{10}$)");
    return regex.match(phone).hasMatch();
}

bool Telephone_directory::isValidDate(const QString& date) {
    QRegularExpression regex(R"(^\d{2}\.\d{2}\.\d{4}$)");
    if (!regex.match(date).hasMatch()) {
        return false;
    }

    QDate parsedDate = QDate::fromString(date, "dd.MM.yyyy");
    return parsedDate.isValid();
}

void Telephone_directory::saveToFile(const QString& fileName) {
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Ошибка открытия файла для записи:" << file.errorString();
        return;
    }

    QTextStream out(&file);
    QSqlQuery query("SELECT * FROM contacts");

    while (query.next()) {
        out << query.value("first_name").toString() << ","
            << query.value("second_name").toString() << ","
            << query.value("third_name").toString() << ","
            << query.value("phone").toString() << ","
            << query.value("email").toString() << ","
            << query.value("address").toString() << ","
            << query.value("birthday").toString() << "\n";
    }

    file.close();
    qDebug() << "Контакты успешно сохранены в файл:" << fileName;
}

void Telephone_directory::loadFromFile(const QString& fileName) {
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Ошибка открытия файла для чтения:" << file.errorString();
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(",");

        if (fields.size() != 7) {
            qDebug() << "Ошибка в формате строки:" << line;
            continue;
        }

        Add(fields[0].toStdString(), fields[1].toStdString(), fields[2].toStdString(),
            fields[3].toStdString(), fields[5].toStdString(), fields[6].toStdString(), fields[4].toStdString());
    }

    file.close();
    qDebug() << "Контакты успешно загружены из файла:" << fileName;
}
