#include "carddialog.h"
#include "magnpc.h"
#include "enemynpc.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QCoreApplication>
#include <QScreen>
#include <QGuiApplication>

CardDialog::CardDialog(NPC* npc, QWidget* parent)
    : QDialog(parent), m_npc(npc)
{
    setupUI();
    setupImage();
}

void CardDialog::setupUI()
{
    setWindowTitle("Карточка персонажа");
    setMinimumSize(400, 500);

    QGridLayout* mainLayout = new QGridLayout(this);


    m_imageLabel = new QLabel;
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setFixedSize(150, 150);
    m_imageLabel->setStyleSheet("border: 1px solid gray;");
    mainLayout->addWidget(m_imageLabel, 0, 0, 1, 2, Qt::AlignCenter);


    QLabel* nameLabel = new QLabel("Имя: " + m_npc->name());
    nameLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    mainLayout->addWidget(nameLabel, 1, 0, 1, 2);

    QLabel* healthLabel = new QLabel("Здоровье: " + QString::number(m_npc->health()));
    if (dynamic_cast<MagNPC*>(m_npc))
        healthLabel->setStyleSheet("color: green;");
    else if (dynamic_cast<EnemyNPC*>(m_npc))
        healthLabel->setStyleSheet("color: red;");
    mainLayout->addWidget(healthLabel, 2, 0, 1, 2);

    Armor a = m_npc->armor();
    QLabel* armorLabel = new QLabel(QString("Броня: Шлем %1, Кираса %2, Сапоги %3")
                                        .arg(a.helmet).arg(a.chest).arg(a.boots));
    mainLayout->addWidget(armorLabel, 3, 0, 1, 2);

    if (auto* mag = dynamic_cast<MagNPC*>(m_npc)) {
        QLabel* manaLabel = new QLabel("Мана: " + QString::number(mag->mana()));
        manaLabel->setStyleSheet("color: blue;");
        mainLayout->addWidget(manaLabel, 4, 0, 1, 2);

        QString elemStr;
        QColor color;
        switch (mag->element()) {
        case Element::Fire:  elemStr = "Огонь";  color = Qt::red;   break;
        case Element::Water: elemStr = "Вода";   color = Qt::blue;  break;
        case Element::Earth: elemStr = "Земля";  color = Qt::green; break;
        default:             elemStr = "Другое"; color = Qt::black; break;
        }
        QLabel* elemLabel = new QLabel("Стихия: " + elemStr);
        elemLabel->setStyleSheet("color: " + color.name() + ";");
        mainLayout->addWidget(elemLabel, 5, 0, 1, 2);
    } else if (auto* enemy = dynamic_cast<EnemyNPC*>(m_npc)) {
        QLabel* damageLabel = new QLabel("Урон: " + QString::number(enemy->damage()));
        mainLayout->addWidget(damageLabel, 4, 0, 1, 2);

        QString rarityStr;
        QColor color;
        switch (enemy->rarity()) {
        case Rarity::Rare:   rarityStr = "Редкий";   color = Qt::yellow;  break;
        case Rarity::Common: rarityStr = "Обычный";  color = Qt::gray;    break;
        case Rarity::Legend: rarityStr = "Легенда";  color = Qt::magenta; break;
        default:             rarityStr = "Другое";   color = Qt::black;   break;
        }
        QLabel* rarityLabel = new QLabel("Редкость: " + rarityStr);
        rarityLabel->setStyleSheet("color: " + color.name() + ";");
        mainLayout->addWidget(rarityLabel, 5, 0, 1, 2);
    }


    QHBoxLayout* btnLayout = new QHBoxLayout;
    m_cancelBtn = new QPushButton("Отмена");
    m_printBtn = new QPushButton("Печать");
    m_saveImageBtn = new QPushButton("Сохранить изображение");
    btnLayout->addWidget(m_cancelBtn);
    btnLayout->addWidget(m_printBtn);
    btnLayout->addWidget(m_saveImageBtn);
    mainLayout->addLayout(btnLayout, 6, 0, 1, 2);

    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_printBtn, &QPushButton::clicked, this, &CardDialog::onPrint);
    connect(m_saveImageBtn, &QPushButton::clicked, this, &CardDialog::saveImage);
}

void CardDialog::setupImage()
{
    QPixmap pix(150, 150);
    QPainter painter(&pix);

    if (dynamic_cast<MagNPC*>(m_npc)) {
        // Маг: синий фон, белая буква "М"
        pix.fill(Qt::blue);
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 72, QFont::Bold));
        painter.drawText(pix.rect(), Qt::AlignCenter, "М");
    } else if (dynamic_cast<EnemyNPC*>(m_npc)) {
        // Враг: красный фон, белая буква "В"
        pix.fill(Qt::red);
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 72, QFont::Bold));
        painter.drawText(pix.rect(), Qt::AlignCenter, "В");
    } else {

        pix.fill(Qt::gray);
        painter.setPen(Qt::black);
        painter.drawText(pix.rect(), Qt::AlignCenter, "?");
    }

    m_imageLabel->setPixmap(pix);
}

void CardDialog::onPrint()
{
    emit removeNpc(m_npc);
    accept();
}

void CardDialog::saveImage()
{

    m_cancelBtn->hide();
    m_printBtn->hide();
    m_saveImageBtn->hide();

    QPixmap pix = this->grab();

    m_cancelBtn->show();
    m_printBtn->show();
    m_saveImageBtn->show();

    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить изображение", "", "PNG (*.png)");
    if (!fileName.isEmpty() && !pix.save(fileName, "PNG"))
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить изображение");
}
