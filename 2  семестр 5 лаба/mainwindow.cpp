#include "mainwindow.h"
#include "carddialog.h"
#include <QHeaderView>
#include <QCoreApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUI();
    connect(m_loadBtn, &QPushButton::clicked, this, &MainWindow::loadFile);
    connect(m_table, &QTableWidget::cellDoubleClicked, this, &MainWindow::onTableDoubleClicked);
    connect(m_table, &QTableWidget::itemChanged, this, &MainWindow::onItemChanged);
}

MainWindow::~MainWindow()
{
    qDeleteAll(m_npcs);
    m_npcs.clear();
}

void MainWindow::setupUI()
{
    setWindowTitle("НПС менеджер");
    resize(800, 600);

    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);

    m_loadBtn = new QPushButton("Загрузить");
    mainLayout->addWidget(m_loadBtn);

    m_table = new QTableWidget;
    m_table->setColumnCount(8);
    QStringList headers = {"№", "Имя", "Тип", "Здоровье", "Броня (Ш/К/С)", "Доп1", "Доп2", "Путь к картинке"};
    m_table->setHorizontalHeaderLabels(headers);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
    mainLayout->addWidget(m_table);
}


void MainWindow::loadFile()
{

    QMessageBox msgBox;
    msgBox.setText("Выберите источник данных");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.button(QMessageBox::Yes)->setText("Заданный файл (встроенные данные)");
    msgBox.button(QMessageBox::No)->setText("Выбрать файл");
    int ret = msgBox.exec();

    clearAll();

    if (ret == QMessageBox::Yes) {

        loadFromHardcodedData();
    } else {

        QString fileName = QFileDialog::getOpenFileName(this, "Выберите файл", "",
                                                        "Текстовые файлы (*.txt);;JSON файлы (*.json);;Все файлы (*)");
        if (fileName.isEmpty())
            return;

        if (fileName.endsWith(".json", Qt::CaseInsensitive))
            loadFromJsonFile(fileName);
        else
            loadFromTextFile(fileName);
    }

    if (m_npcs.isEmpty())
        QMessageBox::information(this, "Информация", "Не найдено ни одной записи или данные повреждены.");
}


void MainWindow::loadFromHardcodedData()
{

    QString data =
        "1,Гэндальф,огонь,150,100,10,20,15\n"
        "2,Скелет,обычный,12,5,1,3,1\n"
        "2,Орк,редкий,25,80,5,15,10\n"
        "1,Саруман,вода,200,120,8,12,6";


    QStringList lines = data.split('\n', Qt::SkipEmptyParts);
    for (const QString& line : lines) {
        QStringList parts = line.split(',');
        if (parts.size() != 8) continue;

        bool ok;
        int code = parts[0].toInt(&ok);
        if (!ok || (code != 1 && code != 2)) continue;

        QString name = parts[1];
        QString extra1 = parts[2];
        int extra2 = parts[3].toInt(&ok);
        if (!ok) continue;
        int health = parts[4].toInt(&ok);
        if (!ok) continue;
        Armor armor;
        armor.helmet = parts[5].toInt(&ok);
        if (!ok) continue;
        armor.chest = parts[6].toInt(&ok);
        if (!ok) continue;
        armor.boots = parts[7].toInt(&ok);
        if (!ok) continue;

        NPC* npc = nullptr;
        if (code == 1) {
            npc = new MagNPC(name, stringToElement(extra1), extra2, health, armor);
        } else if (code == 2) {
            npc = new EnemyNPC(name, stringToRarity(extra1), extra2, health, armor);
        }
        if (npc) {
            m_npcs.append(npc);
            addToTable(npc);
        }
    }
}


void MainWindow::loadFromTextFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл");
        return;
    }

    QTextStream in(&file);
    int lineNum = 0;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        lineNum++;
        if (line.isEmpty()) continue;

        QStringList parts = line.split(',');
        if (parts.size() != 8) {
            QMessageBox::warning(this, "Ошибка формата",
                                 QString("Строка %1: ожидается 8 полей, получено %2").arg(lineNum).arg(parts.size()));
            continue;
        }

        bool ok;
        int code = parts[0].toInt(&ok);
        if (!ok || (code != 1 && code != 2)) {
            QMessageBox::warning(this, "Ошибка формата",
                                 QString("Строка %1: код должен быть 1 или 2").arg(lineNum));
            continue;
        }

        QString name = parts[1];
        QString extra1 = parts[2];
        int extra2 = parts[3].toInt(&ok);
        if (!ok) { QMessageBox::warning(this, "Ошибка", "Неверное число в строке " + QString::number(lineNum)); continue; }
        int health = parts[4].toInt(&ok);
        if (!ok) { QMessageBox::warning(this, "Ошибка", "Неверное число в строке " + QString::number(lineNum)); continue; }
        Armor armor;
        armor.helmet = parts[5].toInt(&ok);
        if (!ok) { QMessageBox::warning(this, "Ошибка", "Неверное число в строке " + QString::number(lineNum)); continue; }
        armor.chest = parts[6].toInt(&ok);
        if (!ok) { QMessageBox::warning(this, "Ошибка", "Неверное число в строке " + QString::number(lineNum)); continue; }
        armor.boots = parts[7].toInt(&ok);
        if (!ok) { QMessageBox::warning(this, "Ошибка", "Неверное число в строке " + QString::number(lineNum)); continue; }

        NPC* npc = nullptr;
        if (code == 1) {
            npc = new MagNPC(name, stringToElement(extra1), extra2, health, armor);
        } else if (code == 2) {
            npc = new EnemyNPC(name, stringToRarity(extra1), extra2, health, armor);
        }

        if (npc) {
            m_npcs.append(npc);
            addToTable(npc);
        }
    }
    file.close();
}


void MainWindow::loadFromJsonFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть JSON файл");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        QMessageBox::warning(this, "Ошибка", "JSON должен содержать массив объектов");
        return;
    }

    QJsonArray arr = doc.array();
    for (const QJsonValue& val : arr) {
        if (!val.isObject()) continue;
        QJsonObject obj = val.toObject();

        int type = obj["type"].toInt();
        QString name = obj["name"].toString();
        int health = obj["health"].toInt();
        Armor armor;
        QJsonObject armorObj = obj["armor"].toObject();
        armor.helmet = armorObj["helmet"].toInt();
        armor.chest = armorObj["chest"].toInt();
        armor.boots = armorObj["boots"].toInt();

        NPC* npc = nullptr;
        if (type == 1) {
            auto* mag = new MagNPC(name, stringToElement(obj["element"].toString()),
                                   obj["mana"].toInt(), health, armor);
            if (obj.contains("image")) mag->setImagePath(obj["image"].toString());
            npc = mag;
        } else if (type == 2) {
            auto* enemy = new EnemyNPC(name, stringToRarity(obj["rarity"].toString()),
                                       obj["damage"].toInt(), health, armor);
            if (obj.contains("image")) enemy->setImagePath(obj["image"].toString());
            npc = enemy;
        }
        if (npc) {
            m_npcs.append(npc);
            addToTable(npc);
        }
    }
}


void MainWindow::addToTable(NPC* npc)
{
    int row = m_table->rowCount();
    m_table->insertRow(row);

    m_table->setItem(row, 0, new QTableWidgetItem(QString::number(row+1)));
    m_table->setItem(row, 1, new QTableWidgetItem(npc->name()));
    m_table->setItem(row, 2, new QTableWidgetItem(npc->typeName()));
    m_table->setItem(row, 3, new QTableWidgetItem(QString::number(npc->health())));
    Armor a = npc->armor();
    m_table->setItem(row, 4, new QTableWidgetItem(QString("%1/%2/%3").arg(a.helmet).arg(a.chest).arg(a.boots)));

    if (auto* mag = dynamic_cast<MagNPC*>(npc)) {
        QString elemStr;
        switch (mag->element()) {
        case Element::Fire:  elemStr = "Огонь"; break;
        case Element::Water: elemStr = "Вода";  break;
        case Element::Earth: elemStr = "Земля"; break;
        default:             elemStr = "Другое"; break;
        }
        m_table->setItem(row, 5, new QTableWidgetItem("Стихия: " + elemStr));
        m_table->setItem(row, 6, new QTableWidgetItem("Мана: " + QString::number(mag->mana())));
    } else if (auto* enemy = dynamic_cast<EnemyNPC*>(npc)) {
        QString rarityStr;
        switch (enemy->rarity()) {
        case Rarity::Rare:   rarityStr = "Редкий";   break;
        case Rarity::Common: rarityStr = "Обычный";  break;
        case Rarity::Legend: rarityStr = "Легенда";  break;
        default:             rarityStr = "Другое";   break;
        }
        m_table->setItem(row, 5, new QTableWidgetItem("Редкость: " + rarityStr));
        m_table->setItem(row, 6, new QTableWidgetItem("Урон: " + QString::number(enemy->damage())));
    }

    m_table->setItem(row, 7, new QTableWidgetItem(npc->imagePath()));
}

void MainWindow::updateTable()
{
    m_table->setRowCount(0);
    for (auto* npc : m_npcs)
        addToTable(npc);
}

void MainWindow::clearAll()
{
    qDeleteAll(m_npcs);
    m_npcs.clear();
    m_table->setRowCount(0);
}


void MainWindow::onTableDoubleClicked(int row, int)
{
    if (row < 0 || row >= m_npcs.size()) return;
    NPC* npc = m_npcs[row];
    CardDialog* dialog = new CardDialog(npc, this);
    connect(dialog, &CardDialog::removeNpc, this, &MainWindow::removeNpc);
    dialog->exec();
    delete dialog;
}


void MainWindow::onItemChanged(QTableWidgetItem* item)
{
    int row = item->row();
    if (row < 0 || row >= m_npcs.size()) return;
    NPC* npc = m_npcs[row];

    switch (item->column()) {
    case 1: npc->setName(item->text()); break;
    case 3: npc->setHealth(item->text().toInt()); break;
    case 4: {
        QStringList parts = item->text().split('/');
        if (parts.size() == 3) {
            Armor a;
            a.helmet = parts[0].toInt();
            a.chest = parts[1].toInt();
            a.boots = parts[2].toInt();
            npc->setArmor(a);
        }
        break;
    }
    case 7:
        if (auto* mag = dynamic_cast<MagNPC*>(npc))
            mag->setImagePath(item->text());
        else if (auto* enemy = dynamic_cast<EnemyNPC*>(npc))
            enemy->setImagePath(item->text());
        break;
    default:;
    }
}


void MainWindow::removeNpc(NPC* npc)
{
    int idx = m_npcs.indexOf(npc);
    if (idx == -1) return;
    m_npcs.removeAt(idx);
    delete npc;
    updateTable();
}


Element MainWindow::stringToElement(const QString& str)
{
    if (str.compare("огонь", Qt::CaseInsensitive) == 0) return Element::Fire;
    if (str.compare("вода", Qt::CaseInsensitive) == 0) return Element::Water;
    if (str.compare("земля", Qt::CaseInsensitive) == 0) return Element::Earth;
    return Element::Other;
}

Rarity MainWindow::stringToRarity(const QString& str)
{
    if (str.compare("редкий", Qt::CaseInsensitive) == 0) return Rarity::Rare;
    if (str.compare("обычный", Qt::CaseInsensitive) == 0) return Rarity::Common;
    if (str.compare("легенда", Qt::CaseInsensitive) == 0) return Rarity::Legend;
    return Rarity::Other;
}
