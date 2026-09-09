#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onReset();
    void onSave();

private:
    // Поля ввода
    QLineEdit *m_brandEdit;
    QLineEdit *m_modelEdit;
    QLineEdit *m_yearEdit;
    QLineEdit *m_plateEdit;

    // Радиокнопки двигателя
    QRadioButton *m_petrolRadio;
    QRadioButton *m_dieselRadio;

    // Чекбоксы опций
    QCheckBox *m_heatedSeatsCheck;
    QCheckBox *m_parkingSensorsCheck;
    QCheckBox *m_matsCheck;

    // Кнопки
    QPushButton *m_resetBtn;
    QPushButton *m_saveBtn;
};

#endif // MAINWINDOW_H
