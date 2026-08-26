#include "mainwindow.h"

#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonArray>
#include <algorithm>

static const QStringList kHeaders = {"Название", "Описание", "Кол-во слотов", "Максимальный вес"};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    setWindowTitle("Лаб. №6 — Программа 2: просмотр сумок из JSON");
}

void MainWindow::setupUi()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QPushButton *chooseBtn = new QPushButton("Выбрать JSON файл...", central);
    connect(chooseBtn, &QPushButton::clicked, this, &MainWindow::onChooseJson);

    m_table1 = new QTableWidget(0, kHeaders.size(), central);
    m_table1->setHorizontalHeaderLabels(kHeaders);
    m_table1->horizontalHeader()->setStretchLastSection(true);
    m_table1->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table1->setSortingEnabled(false); // сортировать саму таблицу нельзя

    m_table2 = new QTableWidget(0, kHeaders.size(), central);
    m_table2->setHorizontalHeaderLabels(kHeaders);
    m_table2->horizontalHeader()->setStretchLastSection(true);
    m_table2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table2->setSortingEnabled(false);

    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->addWidget(chooseBtn);
    layout->addWidget(new QLabel("Корректные объекты (обратный алфавитный порядок по названию):", central));
    layout->addWidget(m_table1);
    layout->addWidget(new QLabel("«Битые» объекты:", central));
    layout->addWidget(m_table2);

    resize(700, 600);
}

void MainWindow::onChooseJson()
{
    QString path = QFileDialog::getOpenFileName(this, "Выберите JSON файл",
                                                 QString(), "JSON files (*.json)");
    if (path.isEmpty())
        return;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл.");
        return;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();

    if (parseError.error != QJsonParseError::NoError || !doc.isArray()) {
        QMessageBox::warning(this, "Ошибка", "Файл не содержит корректный JSON-массив.");
        return;
    }

    QJsonArray array = doc.array();

    QVector<Bag> validBags;
    QVector<QJsonObject> badObjects;

    for (const QJsonValue &val : array) {
        if (!val.isObject()) {
            // Не объект - тоже считаем "битым", оборачиваем как есть
            QJsonObject wrapper;
            wrapper["raw"] = val;
            badObjects.append(wrapper);
            continue;
        }

        QJsonObject obj = val.toObject();
        bool parsedOk = false;
        Bag bag = Bag::fromJson(obj, &parsedOk);

        if (parsedOk && bag.isValid())
            validBags.append(bag);
        else
            badObjects.append(obj);
    }

    // Сортируем вектор валидных объектов по названию в обратном
    // алфавитном порядке (саму таблицу не сортируем, только вставляем
    // уже готовые отсортированные данные)
    std::sort(validBags.begin(), validBags.end(),
              [](const Bag &a, const Bag &b) {
                  return QString::localeAwareCompare(a.name(), b.name()) > 0;
              });

    fillTable1(validBags);
    fillTable2(badObjects);
    saveBadObjects(badObjects, path);
}

void MainWindow::fillTable1(const QVector<Bag> &bags)
{
    m_table1->setRowCount(bags.size());
    for (int row = 0; row < bags.size(); ++row) {
        const Bag &b = bags.at(row);
        m_table1->setItem(row, 0, new QTableWidgetItem(b.name()));
        m_table1->setItem(row, 1, new QTableWidgetItem(b.description()));
        m_table1->setItem(row, 2, new QTableWidgetItem(QString::number(b.slots())));
        m_table1->setItem(row, 3, new QTableWidgetItem(QString::number(b.maxWeight(), 'f', 2)));
    }
}

void MainWindow::fillTable2(const QVector<QJsonObject> &badObjects)
{
    m_table2->setRowCount(badObjects.size());
    for (int row = 0; row < badObjects.size(); ++row) {
        const QJsonObject &obj = badObjects.at(row);

        auto fieldText = [&obj](const char *key) -> QString {
            if (!obj.contains(key))
                return "<нет поля>";
            QJsonValue v = obj.value(key);
            if (v.isString())
                return v.toString();
            if (v.isDouble())
                return QString::number(v.toDouble());
            return "<неверный формат>";
        };

        m_table2->setItem(row, 0, new QTableWidgetItem(fieldText("name")));
        m_table2->setItem(row, 1, new QTableWidgetItem(fieldText("description")));
        m_table2->setItem(row, 2, new QTableWidgetItem(fieldText("slots")));
        m_table2->setItem(row, 3, new QTableWidgetItem(fieldText("maxWeight")));
    }
}

void MainWindow::saveBadObjects(const QVector<QJsonObject> &badObjects, const QString &sourcePath)
{
    if (badObjects.isEmpty())
        return;

    QJsonArray array;
    for (const QJsonObject &obj : badObjects)
        array.append(obj);

    QFileInfo info(sourcePath);
    QString outPath = info.absolutePath() + "/" + info.completeBaseName() + "_bad.json";

    QFile outFile(outPath);
    if (outFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        outFile.write(QJsonDocument(array).toJson(QJsonDocument::Indented));
        outFile.close();
    }
}
