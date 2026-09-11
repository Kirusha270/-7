#include "mainwindow.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>
#include <QMessageBox>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>
#include <QStringConverter>

bool Car::writeToFile(const QString& fileName) const
{
    QFile file(fileName);
    if (!file.open(QIODevice::Append | QIODevice::Text))
        return false;

    QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
#else
    out.setEncoding(QStringConverter::Utf8);
#endif

    auto yesNo = [](bool v) {
        return v ? QStringLiteral("Да") : QStringLiteral("Нет");
    };

    out << "Марка:            " << brand << '\n'
        << "Модель:           " << model << '\n'
        << "Год выпуска:      " << year  << '\n'
        << "Гос номер:        " << plate << '\n'
        << "Топливо:          " << (fuel.isEmpty() ? QStringLiteral("—") : fuel) << '\n'
        << "Подогрев сидений: " << yesNo(heatedSeats) << '\n'
        << "Парктроники:      " << yesNo(parkingAids) << '\n'
        << "Коврики:          " << yesNo(floorMats)   << '\n'
        << "----------------------------------------\n";

    file.close();
    return true;
}

// ==================== Конструктор окна ====================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* form = new QFormLayout;
    form->setLabelAlignment(Qt::AlignRight);

    brandEdit = new QLineEdit;
    brandEdit->setPlaceholderText("Только заглавные буквы, напр. LADA или ВАЗ");
    form->addRow("Марка:", brandEdit);

    modelEdit = new QLineEdit;
    modelEdit->setPlaceholderText("Например: Vesta");
    form->addRow("Модель:", modelEdit);

    yearEdit = new QLineEdit;
    yearEdit->setPlaceholderText("Например: 2020");
    form->addRow("Год выпуска:", yearEdit);

    plateEdit = new QLineEdit;
    plateEdit->setPlaceholderText("Например: А111АА22 или А111АА101");
    form->addRow("Гос номер:", plateEdit);

    auto* fuelBox    = new QGroupBox("Топливо");
    auto* fuelLayout = new QHBoxLayout(fuelBox);
    petrolRadio = new QRadioButton("Бензин");
    dieselRadio = new QRadioButton("Дизель");
    fuelLayout->addWidget(petrolRadio);
    fuelLayout->addWidget(dieselRadio);
    fuelLayout->addStretch();
    form->addRow(fuelBox);

    auto* optBox    = new QGroupBox("Опции");
    auto* optLayout = new QVBoxLayout(optBox);
    heatedSeatsCb = new QCheckBox("Подогрев сидений");
    parkingAidsCb = new QCheckBox("Парктроники");
    floorMatsCb   = new QCheckBox("Коврики");
    optLayout->addWidget(heatedSeatsCb);
    optLayout->addWidget(parkingAidsCb);
    optLayout->addWidget(floorMatsCb);
    form->addRow(optBox);


    saveButton  = new QPushButton("Сохранить");
    resetButton = new QPushButton("Сброс");

    auto* btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(saveButton);
    btnLayout->addWidget(resetButton);

    auto* mainLayout = new QVBoxLayout(central);
    mainLayout->addLayout(form);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(btnLayout);

    connect(saveButton,  &QPushButton::clicked, this, &MainWindow::onSaveClicked);
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::onResetClicked);

    setWindowTitle("Автомобиль — форма ввода");
    resize(500, 460);
}


void MainWindow::resetForm()
{
    brandEdit->clear();
    modelEdit->clear();
    yearEdit->clear();
    plateEdit->clear();

    petrolRadio->setAutoExclusive(false);
    dieselRadio->setAutoExclusive(false);
    petrolRadio->setChecked(false);
    dieselRadio->setChecked(false);
    petrolRadio->setAutoExclusive(true);
    dieselRadio->setAutoExclusive(true);

    heatedSeatsCb->setChecked(false);
    parkingAidsCb->setChecked(false);
    floorMatsCb->setChecked(false);

    brandEdit->setFocus();
}


void MainWindow::onResetClicked()
{
    resetForm();
}


void MainWindow::onSaveClicked()
{
    const QString brand   = brandEdit->text().trimmed();
    const QString model   = modelEdit->text().trimmed();
    const QString yearStr = yearEdit->text().trimmed();
    const QString plate   = plateEdit->text().trimmed();


    if (brand.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Поле «Марка» обязательно для заполнения.");
        brandEdit->setFocus();
        return;
    }
    static const QRegularExpression brandRe(QStringLiteral("^[A-ZА-ЯЁ]+$"));
    if (!brandRe.match(brand).hasMatch()) {
        QMessageBox::warning(this, "Ошибка",
                             "Поле «Марка»: допускаются только заглавные буквы "
                             "английского или русского алфавита (без пробелов и цифр).");
        brandEdit->setFocus();
        return;
    }


    if (model.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Поле «Модель» обязательно для заполнения.");
        modelEdit->setFocus();
        return;
    }


    if (yearStr.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Поле «Год выпуска» обязательно для заполнения.");
        yearEdit->setFocus();
        return;
    }
    static const QRegularExpression yearRe(QStringLiteral("^[1-9]\\d{3}$"));
    if (!yearRe.match(yearStr).hasMatch()) {
        QMessageBox::warning(this, "Ошибка",
                             "Поле «Год выпуска»: должно содержать ровно 4 цифры, "
                             "причём первая цифра не может быть нулём.");
        yearEdit->setFocus();
        return;
    }


    if (plate.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Поле «Гос номер» обязательно для заполнения.");
        plateEdit->setFocus();
        return;
    }
    const QString letters =
        QStringLiteral("АВЕКМНОРСТУХавекмнорстухABEKMHOPCTYXabekmhopctyx");
    const QRegularExpression plateRe(
        QStringLiteral("^[%1]\\d{3}[%1]{2}(\\d{2}|101|111)$").arg(letters));

    if (!plateRe.match(plate).hasMatch()) {
        QMessageBox::warning(this, "Ошибка",
                             "Поле «Гос номер»: неверный формат.\n\n"
                             "Буквы — только из набора АВЕКМНОРСТУХ "
                             "(либо латинские A,B,E,K,M,H,O,P,C,T,Y,X), в любом регистре.\n"
                             "Регион: любые две цифры, либо строго 101 или 111.");
        plateEdit->setFocus();
        return;
    }

    QString fuel;
    if (petrolRadio->isChecked())      fuel = QStringLiteral("Бензин");
    else if (dieselRadio->isChecked()) fuel = QStringLiteral("Дизель");


    Car car(brand, model, yearStr.toInt(), plate, fuel,
            heatedSeatsCb->isChecked(),
            parkingAidsCb->isChecked(),
            floorMatsCb->isChecked());

    if (!car.writeToFile(QStringLiteral("result.txt"))) {
        QMessageBox::critical(this, "Ошибка",
                              "Не удалось записать данные в файл «result.txt».");
        return;
    }

    QMessageBox::information(this, "Успех",
                             "Данные успешно сохранены в файл «result.txt».");
    resetForm();
}
