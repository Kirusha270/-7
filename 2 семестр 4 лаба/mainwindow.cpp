#include "mainwindow.h"
#include "car.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(550, 600); // увеличенное окно
    setWindowTitle("Автомобиль");
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    // Поля ввода
    m_brandEdit = new QLineEdit;
    m_modelEdit = new QLineEdit;
    m_yearEdit = new QLineEdit;
    m_plateEdit = new QLineEdit;

    // Двигатель (радио) – изначально ни один не выбран
    m_petrolRadio = new QRadioButton("Бензин");
    m_dieselRadio = new QRadioButton("Дизель");
    m_petrolRadio->setAutoExclusive(false);
    m_dieselRadio->setAutoExclusive(false);
    m_petrolRadio->setChecked(false);
    m_dieselRadio->setChecked(false);
    m_petrolRadio->setAutoExclusive(true);
    m_dieselRadio->setAutoExclusive(true);

    QHBoxLayout *engineLayout = new QHBoxLayout;
    engineLayout->addWidget(m_petrolRadio);
    engineLayout->addWidget(m_dieselRadio);
    engineLayout->addStretch();

    // Опции
    m_heatedSeatsCheck = new QCheckBox("Подогрев сидений");
    m_parkingSensorsCheck = new QCheckBox("Парктроники");
    m_matsCheck = new QCheckBox("Коврики");

    QVBoxLayout *optionsLayout = new QVBoxLayout;
    optionsLayout->addWidget(m_heatedSeatsCheck);
    optionsLayout->addWidget(m_parkingSensorsCheck);
    optionsLayout->addWidget(m_matsCheck);

    QGroupBox *optionsGroup = new QGroupBox("Опции");
    optionsGroup->setLayout(optionsLayout);

    // Кнопки
    m_resetBtn = new QPushButton("Сброс");
    m_saveBtn = new QPushButton("Сохранить");

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_resetBtn);
    buttonLayout->addWidget(m_saveBtn);

    // Основная компоновка
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Марка:", m_brandEdit);
    formLayout->addRow("Модель:", m_modelEdit);
    formLayout->addRow("Год выпуска:", m_yearEdit);
    formLayout->addRow("Гос. номер:", m_plateEdit);
    formLayout->addRow("Двигатель:", engineLayout);
    formLayout->addRow(optionsGroup);
    formLayout->addRow(buttonLayout);

    central->setLayout(formLayout);

    connect(m_resetBtn, &QPushButton::clicked, this, &MainWindow::onReset);
    connect(m_saveBtn, &QPushButton::clicked, this, &MainWindow::onSave);
}

MainWindow::~MainWindow() = default;

void MainWindow::onReset()
{
    m_brandEdit->clear();
    m_modelEdit->clear();
    m_yearEdit->clear();
    m_plateEdit->clear();

    m_petrolRadio->setAutoExclusive(false);
    m_dieselRadio->setAutoExclusive(false);
    m_petrolRadio->setChecked(false);
    m_dieselRadio->setChecked(false);
    m_petrolRadio->setAutoExclusive(true);
    m_dieselRadio->setAutoExclusive(true);

    m_heatedSeatsCheck->setChecked(false);
    m_parkingSensorsCheck->setChecked(false);
    m_matsCheck->setChecked(false);
}

void MainWindow::onSave()
{
    QString brand = m_brandEdit->text().trimmed();
    QString model = m_modelEdit->text().trimmed();
    QString year = m_yearEdit->text().trimmed();
    QString plate = m_plateEdit->text().trimmed().toUpper();

    bool petrol = m_petrolRadio->isChecked();
    bool diesel = m_dieselRadio->isChecked();
    bool heated = m_heatedSeatsCheck->isChecked();
    bool parking = m_parkingSensorsCheck->isChecked();
    bool mats = m_matsCheck->isChecked();

    bool ok = true;
    QString errorMsg;

    // 1. Марка: только заглавные буквы (англ или рус)
    QRegularExpression nameRx("^[A-ZА-Я]+$");
    if (brand.isEmpty() || !nameRx.match(brand).hasMatch()) {
        ok = false;
        errorMsg = "Марка: должна быть не пустой и содержать только заглавные буквы (A-Z, А-Я)";
    }
    // 2. Модель: только не пустая (без ограничений на символы)
    else if (model.isEmpty()) {
        ok = false;
        errorMsg = "Модель: не должна быть пустой";
    }
    // 3. Год: 4 цифры, первая не ноль
    else if (!QRegularExpression("^[1-9]\\d{3}$").match(year).hasMatch()) {
        ok = false;
        errorMsg = "Год выпуска: должен быть четырёхзначным числом, начинающимся с цифры от 1 до 9";
    }
    // 4. Госномер: поддержка русских и латинских букв
    else {
        QString validChars = "АAВBЕEКKМMНHОOРPСCТTУYХX";
        QString pattern1 = "^[" + validChars + "]\\d{3}[" + validChars + "]{2}\\d{2}$";
        QString pattern2 = "^[" + validChars + "]\\d{3}[" + validChars + "]{2}\\d{3}$";
        QRegularExpression rx1(pattern1);
        QRegularExpression rx2(pattern2);

        if (!rx1.match(plate).hasMatch() && !rx2.match(plate).hasMatch()) {
            ok = false;
            errorMsg = "Гос. номер: должен быть формата A111AA22 или A111AA222 (буквы из допустимого набора)";
        } else if (rx2.match(plate).hasMatch()) {
            QString region = plate.mid(6, 3);
            if (region != "101" && region != "111") {
                ok = false;
                errorMsg = "Гос. номер: для трёхзначного региона допустимы только 101 или 111";
            }
        }
    }
    // 5. Двигатель: обязательно выбран
    if (ok && !petrol && !diesel) {
        ok = false;
        errorMsg = "Двигатель: необходимо выбрать Бензин или Дизель";
    }

    if (!ok) {
        QMessageBox::critical(this, "Ошибка", errorMsg);
        return;
    }

    Car car(brand, model, year, plate, petrol, diesel,
            heated, parking, mats);

    QFile file("result.txt");
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл для записи");
        return;
    }
    QTextStream out(&file);
    car.writeToFile(out);
    file.close();

    QMessageBox::information(this, "Успех", "Данные сохранены в result.txt");
}
