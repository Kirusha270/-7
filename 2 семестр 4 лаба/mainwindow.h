#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

class QLineEdit;
class QRadioButton;
class QCheckBox;
class QPushButton;

// ---------- Класс данных ----------
struct Car {
    QString brand;              // марка
    QString model;              // модель
    int     year;               // год выпуска
    QString plate;              // гос номер
    QString fuel;               // топливо ("Бензин"/"Дизель"/)
    bool    heatedSeats  = false;   // подогрев сидений
    bool    parkingAids  = false;   // парктроники
    bool    floorMats    = false;   // коврики

    Car() : year(0) {}

    Car(const QString& b, const QString& m, int y,
        const QString& p, const QString& f,
        bool hs, bool pa, bool fm)
        : brand(b), model(m), year(y), plate(p), fuel(f),
        heatedSeats(hs), parkingAids(pa), floorMats(fm) {}

    // Метод записи в файл (дозапись, UTF-8)
    bool writeToFile(const QString& fileName) const;
};

// ---------- Окно ----------
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

private slots:
    void onSaveClicked();
    void onResetClicked();

private:
    QLineEdit*    brandEdit     = nullptr;
    QLineEdit*    modelEdit     = nullptr;
    QLineEdit*    yearEdit      = nullptr;
    QLineEdit*    plateEdit     = nullptr;
    QRadioButton* petrolRadio   = nullptr;
    QRadioButton* dieselRadio   = nullptr;
    QCheckBox*    heatedSeatsCb = nullptr;
    QCheckBox*    parkingAidsCb = nullptr;
    QCheckBox*    floorMatsCb   = nullptr;
    QPushButton*  saveButton    = nullptr;
    QPushButton*  resetButton   = nullptr;

    void resetForm();
};

#endif // MAINWINDOW_H
