#include "MainWindow.h"
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QTextStream>
#include <QPixmap>
#include <QPainter>
#include <QRegularExpression>
#include <QEvent>
#include <QMouseEvent>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    setWindowTitle("Регистрация пользователя");
    resize(500, 450);


    m_nameEdit = new QLineEdit(this);
    m_surnameEdit = new QLineEdit(this);
    m_nickEdit = new QLineEdit(this);
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_emailEdit = new QLineEdit(this);
    m_idEdit = new QLineEdit(this);

    m_serverCombo = new QComboBox(this);
    m_serverCombo->addItems({"Сервер 1", "Сервер 2", "Сервер 3"});

    m_loadPhotoBtn = new QPushButton("Загрузить фото", this);
    m_registerBtn = new QPushButton("Зарегистрироваться", this);
    m_photoLabel = new QLabel(this);
    m_photoLabel->setFixedSize(100, 100);
    m_photoLabel->setStyleSheet("border: 1px solid gray;");
    m_photoLabel->setAlignment(Qt::AlignCenter);
    m_photoLabel->setText("Фото");


    QString style = R"(
        QMainWindow { background-color: #2b2b2b; }
        QLabel { color: #f0f0f0; }
        QLineEdit, QComboBox { background-color: #3c3c3c; color: #ffffff; border: 1px solid #555; border-radius: 4px; padding: 4px; }
        QPushButton { background-color: #5a5a5a; color: #ffffff; border: 1px solid #777; border-radius: 4px; padding: 6px; }
        QPushButton:hover { background-color: #6a6a6a; }
        QPushButton:pressed { background-color: #4a4a4a; }
    )";
    setStyleSheet(style);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel("Имя:"), 0, 0);
    layout->addWidget(m_nameEdit, 0, 1);
    layout->addWidget(new QLabel("Фамилия:"), 1, 0);
    layout->addWidget(m_surnameEdit, 1, 1);
    layout->addWidget(new QLabel("Никнейм:"), 2, 0);
    layout->addWidget(m_nickEdit, 2, 1);
    layout->addWidget(new QLabel("Пароль:"), 3, 0);
    layout->addWidget(m_passwordEdit, 3, 1);
    layout->addWidget(new QLabel("Почта:"), 4, 0);
    layout->addWidget(m_emailEdit, 4, 1);
    layout->addWidget(new QLabel("ID-Key:"), 5, 0);
    layout->addWidget(m_idEdit, 5, 1);
    layout->addWidget(new QLabel("Сервер:"), 6, 0);
    layout->addWidget(m_serverCombo, 6, 1);
    layout->addWidget(m_loadPhotoBtn, 7, 0);
    layout->addWidget(m_photoLabel, 7, 1);
    layout->addWidget(m_registerBtn, 8, 0, 1, 2);

    QWidget *central = new QWidget(this);
    central->setLayout(layout);
    setCentralWidget(central);


    connect(m_loadPhotoBtn, &QPushButton::clicked, this, &MainWindow::onLoadPhoto);
    connect(m_registerBtn, &QPushButton::clicked, this, &MainWindow::onRegister);


    QList<QLineEdit*> edits = {m_nameEdit, m_surnameEdit, m_nickEdit,
                                m_passwordEdit, m_emailEdit, m_idEdit};
    for (auto edit : edits) {
        edit->installEventFilter(this);
    }

    QPixmap defaultPix(100, 100);
    defaultPix.fill(Qt::lightGray);
    QPainter painter(&defaultPix);
    painter.setPen(Qt::black);
    painter.drawText(defaultPix.rect(), Qt::AlignCenter, "Нет фото");
    m_photoLabel->setPixmap(defaultPix);
    m_photoPath.clear();
}

MainWindow::~MainWindow() {}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        QLineEdit *edit = qobject_cast<QLineEdit*>(obj);
        if (edit) {
            edit->clear();
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}


void MainWindow::onLoadPhoto()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Выберите фото", QDir::homePath(),
                                                    "Изображения (*.png *.jpg *.jpeg *.bmp)");
    if (fileName.isEmpty())
        return;

    QPixmap pix(fileName);
    if (pix.isNull()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось загрузить изображение.");
        return;
    }

    m_photoLabel->setPixmap(pix.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_photoPath = fileName;
}


bool MainWindow::validateFields(QString &error)
{

    QRegularExpression nameRegex("^[A-Za-zА-Яа-я]+$");
    if (!nameRegex.match(m_nameEdit->text()).hasMatch()) {
        error = "Имя должно содержать только буквы.";
        return false;
    }

    if (!nameRegex.match(m_surnameEdit->text()).hasMatch()) {
        error = "Фамилия должна содержать только буквы.";
        return false;
    }

    QRegularExpression nickRegex("^[A-Za-zА-Яа-я0-9_]{3,20}$");
    if (!nickRegex.match(m_nickEdit->text()).hasMatch()) {
        error = "Никнейм должен содержать от 3 до 20 символов (буквы, цифры, подчёркивание).";
        return false;
    }

    QRegularExpression passRegex("^(?=.*[A-Za-zА-Яа-я])(?=.*\\d).{6,}$");
    if (!passRegex.match(m_passwordEdit->text()).hasMatch()) {
        error = "Пароль должен содержать минимум 6 символов, включая букву и цифру.";
        return false;
    }

    QRegularExpression emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    if (!emailRegex.match(m_emailEdit->text()).hasMatch()) {
        error = "Введите корректный email.";
        return false;
    }

    QRegularExpression idRegex("^[A-Z]\\d{3}-\\d{2}[A-Z]-[A-Z]$");
    if (!idRegex.match(m_idEdit->text()).hasMatch()) {
        error = "ID-Key должен соответствовать формату: A123-45B-C (заглавная латиница).";
        return false;
    }
    return true;
}


bool MainWindow::isNicknameTaken(const QString &nick)
{
    QString desktop = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QDir accountsDir(desktop + "/Accounts");
    if (!accountsDir.exists())
        return false;
    return accountsDir.exists(nick);
}


QString MainWindow::encryptPassword(const QString &password)
{
    const QString key = "SecretKey123";
    QByteArray data = password.toUtf8();
    QByteArray keyData = key.toUtf8();
    for (int i = 0; i < data.size(); ++i) {
        data[i] = data[i] ^ keyData[i % keyData.size()];
    }

    return data.toBase64();
}


void MainWindow::copyDefaultPhoto(const QString &destDir)
{
    QPixmap defaultPix(200, 200);
    defaultPix.fill(Qt::lightGray);
    QPainter painter(&defaultPix);
    painter.setPen(Qt::black);
    painter.drawText(defaultPix.rect(), Qt::AlignCenter, "Default");
    defaultPix.save(destDir + "/avatar.png");
}


bool MainWindow::saveUserData(const QString &nick,
                              const QString &name,
                              const QString &surname,
                              const QString &email,
                              const QString &idKey,
                              const QString &encryptedPass,
                              const QString &photoSource)
{

    QString desktop = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString accountsDirPath = desktop + "/Accounts";
    QDir accountsDir(accountsDirPath);
    if (!accountsDir.exists()) {
        if (!accountsDir.mkpath(accountsDirPath)) {
            QMessageBox::critical(this, "Ошибка", "Не удалось создать папку Accounts.");
            return false;
        }
    }


    QString userDirPath = accountsDirPath + "/" + nick;
    QDir userDir(userDirPath);
    if (userDir.exists()) {
        // Теоретически не должно случиться, т.к. проверяли занятость, но на всякий случай
        QMessageBox::critical(this, "Ошибка", "Папка пользователя уже существует.");
        return false;
    }
    if (!userDir.mkpath(userDirPath)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать папку пользователя.");
        return false;
    }


    if (!photoSource.isEmpty() && QFile::exists(photoSource)) {
        QString ext = QFileInfo(photoSource).suffix();
        QString destPhoto = userDirPath + "/avatar." + ext;
        if (!QFile::copy(photoSource, destPhoto)) {
            // Если не скопировалось, используем стандартное
            copyDefaultPhoto(userDirPath);
        }
    } else {
        copyDefaultPhoto(userDirPath);
    }


    QJsonObject json;
    json["name"] = name;
    json["surname"] = surname;
    json["nickname"] = nick;
    json["email"] = email;
    json["id_key"] = idKey;
    // сервер? Можно добавить, но не обязательно, добавим для полноты
    json["server"] = m_serverCombo->currentText();

    QJsonDocument doc(json);
    QString jsonFilePath = userDirPath + "/data.json";
    QFile jsonFile(jsonFilePath);
    if (!jsonFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать JSON файл.");
        return false;
    }
    jsonFile.write(doc.toJson());
    jsonFile.close();


    QString txtFilePath = userDirPath + "/password.txt";
    QFile txtFile(txtFilePath);
    if (!txtFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать TXT файл.");
        return false;
    }
    QTextStream out(&txtFile);
    out << email << "/" << encryptedPass;
    txtFile.close();

    return true;
}


void MainWindow::onRegister()
{

    QString error;
    if (!validateFields(error)) {
        QMessageBox::warning(this, "Ошибка валидации", error);
        return;
    }

    QString nick = m_nickEdit->text();

    if (isNicknameTaken(nick)) {
        QMessageBox::warning(this, "Ошибка", "Никнейм уже занят. Выберите другой.");
        return;
    }

    QString encrypted = encryptPassword(m_passwordEdit->text());

    bool ok = saveUserData(nick,
                           m_nameEdit->text(),
                           m_surnameEdit->text(),
                           m_emailEdit->text(),
                           m_idEdit->text(),
                           encrypted,
                           m_photoPath);

    if (ok) {
        QMessageBox::information(this, "Успех", "Регистрация успешно завершена!");

        m_nameEdit->clear();
        m_surnameEdit->clear();
        m_nickEdit->clear();
        m_passwordEdit->clear();
        m_emailEdit->clear();
        m_idEdit->clear();

        QPixmap defaultPix(100, 100);
        defaultPix.fill(Qt::lightGray);
        QPainter painter(&defaultPix);
        painter.setPen(Qt::black);
        painter.drawText(defaultPix.rect(), Qt::AlignCenter, "Нет фото");
        m_photoLabel->setPixmap(defaultPix);
        m_photoPath.clear();
    }
}
