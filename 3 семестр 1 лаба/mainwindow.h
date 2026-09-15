#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QSet>
#include <QString>

class QLineEdit;
class QPushButton;
class QTextEdit;

// ---------- Вариант 1: "сырой" зубчатый массив через malloc/realloc ----------
struct JaggedMalloc {
    char** rows = nullptr;   // 4 указателя на UTF-8-строки ('\0'-терминаторы)
    int*   lens = nullptr;   // длины в байтах
    int    n    = 4;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onProcessClicked();
    void onSaveClicked();

private:
    void setupUi();

    // построение массивов
    QVector<QVector<QChar>> buildJaggedVector(const QString& s) const;
    JaggedMalloc            buildJaggedMalloc(const QString& s) const;
    void                    freeJaggedMalloc(JaggedMalloc& j) const;

    // цветной HTML для QTextEdit
    QString coloredHtml(const QString& s) const;

    // виджеты
    QLineEdit*   m_inputEdit  = nullptr;
    QPushButton* m_processBtn = nullptr;
    QPushButton* m_saveBtn    = nullptr;
    QTextEdit*   m_outputEdit = nullptr;

    // данные
    QString                 m_input;       // введённая строка
    QString                 m_result;      // input + "+123АБВ"
    QString                 m_plainReport; // то, что сохраняем в файл
    QVector<QVector<QChar>> m_rowsVec;     // представление зубчатого массива
};

#endif // MAINWINDOW_H
