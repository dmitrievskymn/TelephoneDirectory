#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QString>
#include "Telephone_directory.h"
//подключение к базе данных
bool connectToDatabase(QSqlDatabase& db) {
    db = QSqlDatabase::addDatabase("QSQLITE");//создание объекта
    db.setDatabaseName("contacts.db");//указываю имя бд

    if (!db.open()) {
        std::cerr << "Ошибка подключения к базе данных: " << db.lastError().text().toStdString() << std::endl;
        return false;
    }
    //sql запрос
    QSqlQuery query;
    QString createTableQuery = R"(
        CREATE TABLE IF NOT EXISTS contacts (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            first_name TEXT NOT NULL,
            second_name TEXT NOT NULL,
            third_name TEXT,
            phone TEXT,
            email TEXT,
            address TEXT,
            birthday TEXT
        );
    )";
    //выполняю запрос на создание таблицы
    if (!query.exec(createTableQuery)) {
        std::cerr << "Ошибка создания таблицы: " << query.lastError().text().toStdString() << std::endl;
        return false;
    }

    return true;
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    //подключение к базе данных
    QSqlDatabase db;
    if (!connectToDatabase(db)) {
        QMessageBox::critical(nullptr, "Ошибка", "Не удалось подключиться к базе данных.");
        return -1;
    }

    Telephone_directory directory;
    //основное окно приложения
    QWidget window;
    window.setWindowTitle("Телефонный справочник");
    window.resize(400, 700);

    QVBoxLayout* layout = new QVBoxLayout;
    //поле для отображения информации
    QTextEdit* display = new QTextEdit;
    display->setReadOnly(true); //запрет на его редактирование
    layout->addWidget(display);
    //поля ввода для добавления нового контакта
    QLineEdit* firstNameInput = new QLineEdit;
    QLineEdit* middleNameInput = new QLineEdit;
    QLineEdit* lastNameInput = new QLineEdit;
    QLineEdit* phoneInput = new QLineEdit;
    QLineEdit* emailInput = new QLineEdit;
    QLineEdit* addressInput = new QLineEdit;
    QLineEdit* birthdayInput = new QLineEdit;
    QLineEdit* searchInput = new QLineEdit;
    //текст подсказка
    firstNameInput->setPlaceholderText("Имя");
    middleNameInput->setPlaceholderText("Отчество");
    lastNameInput->setPlaceholderText("Фамилия");
    phoneInput->setPlaceholderText("Телефон");
    emailInput->setPlaceholderText("Email");
    addressInput->setPlaceholderText("Адрес");
    birthdayInput->setPlaceholderText("День рождения (например, 01.01.2000)");
    searchInput->setPlaceholderText("Введите фамилию для поиска");
    // поля для вывода того что ввели
    layout->addWidget(new QLabel("Добавление контакта:"));
    layout->addWidget(firstNameInput);
    layout->addWidget(middleNameInput);
    layout->addWidget(lastNameInput);
    layout->addWidget(phoneInput);
    layout->addWidget(emailInput);
    layout->addWidget(addressInput);
    layout->addWidget(birthdayInput);
    //кнопки для управления контактами
    QPushButton* addButton = new QPushButton("Добавить контакт");
    QPushButton* deleteButton = new QPushButton("Удалить контакт");
    QPushButton* showButton = new QPushButton("Показать все контакты");
    QPushButton* clearButton = new QPushButton("Очистить базу данных");
    QPushButton* searchButton = new QPushButton("Найти контакт");
    //добавляю эти кнопки
    layout->addWidget(addButton);
    layout->addWidget(deleteButton);
    layout->addWidget(showButton);
    layout->addWidget(clearButton);
    layout->addWidget(new QLabel("Поиск контакта:"));
    layout->addWidget(searchInput);
    layout->addWidget(searchButton);
    //обработчик для кнопки добавления контакта
    QObject::connect(addButton, &QPushButton::clicked, [&]() {
        QString firstName = firstNameInput->text().trimmed();
        QString middleName = middleNameInput->text().trimmed();
        QString lastName = lastNameInput->text().trimmed();
        QString phone = phoneInput->text().trimmed();
        QString email = emailInput->text().trimmed();
        QString address = addressInput->text().trimmed();
        QString birthday = birthdayInput->text().trimmed();
        //проверка введенных полей
        if (firstName.isEmpty() || lastName.isEmpty()) {
            display->setText("Ошибка: Имя и фамилия обязательны для заполнения.");
            return;
        }

        if (!directory.isValidPhone(phone)) {
            display->setText("Ошибка: Некорректный формат телефона. Введите телефон в формате +7XXXXXXXXXX или 8XXXXXXXXXX.");
            return;
        }

        if (!directory.isValidEmail(email)) {
            display->setText("Ошибка: Некорректный формат email. Допустимы только адреса @gmail.com и @mail.ru.");
            return;
        }

        if (!directory.isValidDate(birthday)) {
            display->setText("Ошибка: Некорректный формат даты. Используйте формат DD.MM.YYYY.");
            return;
        }

        directory.Add(firstName.toStdString(), lastName.toStdString(), middleName.toStdString(),
                      phone.toStdString(), address.toStdString(), birthday.toStdString(), email.toStdString());
        //обновление отображения и очистка полей для ввода
        display->setText("Контакт успешно добавлен.");
        firstNameInput->clear();
        middleNameInput->clear();
        lastNameInput->clear();
        phoneInput->clear();
        emailInput->clear();
        addressInput->clear();
        birthdayInput->clear();
    });
    //обработчики для кнопок
    QObject::connect(showButton, &QPushButton::clicked, [&]() {
        QString result = QString::fromStdString(directory.getAllContacts());
        display->setText(result.isEmpty() ? "Контакты не найдены." : result);
    });

    QObject::connect(deleteButton, &QPushButton::clicked, [&]() {
        QString lastName = lastNameInput->text().trimmed();

        if (lastName.isEmpty()) {
            display->setText("Ошибка: Для удаления введите фамилию.");
            return;
        }

        directory.Delete(lastName.toStdString());
        display->setText("Контакт успешно удален.");
        lastNameInput->clear();
    });
    //кнопки для сохранения и загрузки из файла
    QPushButton* saveButton = new QPushButton("Сохранить в файл");
    QPushButton* loadButton = new QPushButton("Загрузить из файла");

    layout->addWidget(saveButton);
    layout->addWidget(loadButton);

    QObject::connect(saveButton, &QPushButton::clicked, [&]() {
        directory.saveToFile("contacts.txt");
        display->setText("Контакты сохранены в файл.");
    });

    QObject::connect(loadButton, &QPushButton::clicked, [&]() {
        directory.loadFromFile("contacts.txt");
        display->setText("Контакты загружены из файла.");
    });

    QObject::connect(clearButton, &QPushButton::clicked, [&]() {
        directory.Clear();
        display->setText("База данных очищена.");
    });

    QObject::connect(searchButton, &QPushButton::clicked, [&]() {
        QString searchName = searchInput->text().trimmed();

        if (searchName.isEmpty()) {
            display->setText("Ошибка: Введите фамилию для поиска.");
            return;
        }

        QString result = QString::fromStdString(directory.findContact(searchName.toStdString()));
        display->setText(result.isEmpty() ? "Контакт не найден." : result);
    });
    //отображаю окно
    window.setLayout(layout);
    window.show();
    //запуск цикла обработки событий приложения
    return app.exec();
}
