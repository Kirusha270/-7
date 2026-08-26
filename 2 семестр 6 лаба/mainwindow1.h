#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLineEdit;
class QSpinBox;
class QDoubleSpinBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onImportFromTxt();  // перенос данных из txt на форму
    void onSaveToJson();     // добавление в конец существующего/создание нового json

private:
    QLineEdit       *m_nameEdit;
    QLineEdit       *m_descriptionEdit;
    QSpinBox        *m_slotsSpin;
    QDoubleSpinBox  *m_maxWeightSpin;

    void setupUi();
};

#endif // MAINWINDOW_H
