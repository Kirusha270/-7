#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QPlainTextEdit;
class QCheckBox;
class QTabWidget;
class QWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onEncode();
    void onTransmit();
    void onDecode();
    void onRecover();

private:
    // Построение вкладок
    QWidget *createEncodeTab();
    QWidget *createTransmitTab();
    QWidget *createDecodeTab();
    QWidget *createRecoverTab();
    QWidget *createTableTab();

    // Вспомогательные функции
    QWidget *makeEditorBlock(const QString &title, QPlainTextEdit *&edit, bool readOnly,
                             bool withLoad, bool withSave);
    void loadInto(QPlainTextEdit *edit);
    void saveFrom(QPlainTextEdit *edit);
    void logMessage(const QString &msg);

    QTabWidget *m_tabs = nullptr;
    QPlainTextEdit *m_log = nullptr;

    // 1. Кодирование
    QPlainTextEdit *m_encIn = nullptr;
    QPlainTextEdit *m_encOut = nullptr;
    // 2. Передача
    QPlainTextEdit *m_trIn = nullptr;
    QPlainTextEdit *m_trOut = nullptr;
    // 3. Декодирование
    QPlainTextEdit *m_decIn = nullptr;
    QPlainTextEdit *m_decOut = nullptr;
    QCheckBox *m_decForce = nullptr;
    // 4. Восстановление
    QPlainTextEdit *m_recIn = nullptr;
    QPlainTextEdit *m_recOut = nullptr;
};

#endif // MAINWINDOW_H
