#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "tree.h"
class QPushButton;
class QLineEdit;
class QPlainTextEdit;
class TreeView;
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;
private slots:
    void onOpen();
    void onPostfix();
    void onSave();
    void onCount3();
    void onBst();
    void onSymmetric();
    void onSum();
    void onRelation();
    void onHeight();
private:
    bool needTree();
    bool readInt(QLineEdit *edit, int &out);
    void say(const QString &s);
    BinaryTree m_tree;
    TreeView *m_view;
    QLineEdit *m_edA;
    QLineEdit *m_edB;
    QLineEdit *m_edPost;
    QPlainTextEdit *m_log;
};
#endif // MAINWINDOW_H

