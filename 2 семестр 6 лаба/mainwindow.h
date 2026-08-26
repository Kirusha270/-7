#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonObject>
#include <QVector>
#include "bag.h"

class QTableWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onChooseJson();

private:
    QTableWidget *m_table1; // корректные объекты, обратный алфавитный порядок по названию
    QTableWidget *m_table2; // "битые" объекты

    void setupUi();
    void fillTable1(const QVector<Bag> &bags);
    void fillTable2(const QVector<QJsonObject> &badObjects);
    void saveBadObjects(const QVector<QJsonObject> &badObjects, const QString &sourcePath);
};

#endif // MAINWINDOW_H
