#include "mainwindow.h"
#include "bag.h"

#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QLocale>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    setWindowTitle("Лаб. №6 — Программа 1: добавление сумки");
}

void MainWindow::setupUi()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    m_nameEdit = new QLineEdit(central);
    m_descriptionEdit = new QLineEdit(central);

    m_slotsSpin = new QSpinBox(central);
    m_slotsSpin->setRange(0, 1000);

    m_maxWeightSpin = new QDoubleSpinBox(central);
    m_maxWeightSpin->setRange(0.0, 100000.0);
    m_maxWeightSpin->setDecimals(2);

    QFormLayout *form = new QFormLayout;
    form->addRow("Название:", m_nameEdit);
    form->addRow("Описание:", m_descriptionEdit);
    form->addRow("Кол-во слотов:", m_slotsSpin);
    form->addRow("Максимальный вес:", m_maxWeightSpin);

    QPushButton *importBtn = new QPushButton("Импорт из TXT...", central);
    QPushButton *saveBtn = new QPushButton("Сохранить в JSON...", central);

    connect(importBtn, &QPushButton::clicked, this, &MainWindow::onImportFromTxt);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveToJson);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->addLayout(form);
    mainLayout->addWidget(importBtn);
    mainLayout->addWidget(saveBtn);

    resize(400, 250);
}

// Формат строки в txt (пример из задания, разделитель "/"):
// название/описание/кол-во слотов/максимальный вес
// Десятичный разделитель в примере - запятая, поддержим оба варианта.
void MainWindow::onImportFromTxt()
{
    QString path = QFileDialog::getOpenFileName(this, "Выберите TXT файл",
                                                 QString(), "Text files (*.txt)");
    if (path.isEmpty())
        return;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл.");
        return;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    QString line = in.readLine();
    file.close();

    if (line.isNull()) {
        QMessageBox::warning(this, "Ошибка", "Файл пуст.");
        return;
    }

    QStringList parts = line.split('/');
    if (parts.size() != 4) {
        QMessageBox::warning(this, "Ошибка",
                              "Неверный формат строки. Ожидается:\n"
                              "название/описание/кол-во слотов/максимальный вес");
        return;
    }

    // Заполняем форму - редактирование остаётся доступным после переноса
    m_nameEdit->setText(parts.at(0));
    m_descriptionEdit->setText(parts.at(1));

    bool okSlots = false;
    int slots = parts.at(2).trimmed().toInt(&okSlots);
    if (okSlots)
        m_slotsSpin->setValue(slots);

    QString weightStr = parts.at(3).trimmed();
    weightStr.replace(',', '.');
    bool okWeight = false;
    double weight = weightStr.toDouble(&okWeight);
    if (okWeight)
        m_maxWeightSpin->setValue(weight);

    if (!okSlots || !okWeight) {
        QMessageBox::information(this, "Внимание",
                                  "Часть числовых полей не удалось распознать, "
                                  "проверьте и заполните их вручную.");
    }
}

void MainWindow::onSaveToJson()
{
    Bag bag(m_nameEdit->text(), m_descriptionEdit->text(),
            m_slotsSpin->value(), m_maxWeightSpin->value());

    // Позволяет выбрать существующий json (данные допишутся в конец)
    // или ввести новое имя файла (будет создан новый json)
    QString path = QFileDialog::getSaveFileName(
        this, "Выберите или создайте JSON файл", QString(),
        "JSON files (*.json)", nullptr, QFileDialog::DontConfirmOverwrite);

    if (path.isEmpty())
        return;
    if (!path.endsWith(".json", Qt::CaseInsensitive))
        path += ".json";

    QJsonArray array;

    QFile inFile(path);
    if (inFile.exists() && inFile.open(QIODevice::ReadOnly)) {
        QJsonDocument existingDoc = QJsonDocument::fromJson(inFile.readAll());
        inFile.close();
        if (existingDoc.isArray())
            array = existingDoc.array();
    }

    array.append(bag.toJson());

    QFile outFile(path);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось записать файл.");
        return;
    }
    outFile.write(QJsonDocument(array).toJson(QJsonDocument::Indented));
    outFile.close();

    QMessageBox::information(this, "Готово",
                              QString("Объект добавлен в файл:\n%1").arg(path));
}
