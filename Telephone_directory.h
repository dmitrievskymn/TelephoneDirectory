#pragma once
#include <iostream>
#include <string>
#include <QSqlDatabase>

class Telephone_directory {
public:
    Telephone_directory(); // конструктор
    ~Telephone_directory(); //деструктор
    //добавление контакта
    void Add(const std::string& first_name, const std::string& second_name, const std::string& third_name = "",
             const std::string& number = "", const std::string& address = "", const std::string& birthday = "",
             const std::string& email = "");
    //удаление контакта
    void Delete(const std::string& second_name);
    //очисте всех контактов
    void Clear();
    //вывод всех контактов
    std::string getAllContacts();
    //поиск контакта
    std::string findContact(const std::string& second_name);
    //сохранить в файл
    void saveToFile(const QString& fileName);
    //загрузить из файла
    void loadFromFile(const QString& fileName);
    //проверка верно ли введен Email
    bool isValidEmail(const QString& email);
    //проверка верно ли введен телефон
    bool isValidPhone(const QString& phone);
    //проверка верно ли введена дата
    bool isValidDate(const QString& date);


private:
    QSqlDatabase db; // объект базы данных
};
