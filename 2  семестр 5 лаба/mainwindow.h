#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QVector>
#include "npc.h"
#include "magnpc.h"
#include "enemynpc.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void loadFile();
    void onTableDoubleClicked(int row, int column);
    void onItemChanged(QTableWidgetItem* item);
    void removeNpc(NPC* npc);

private:
    QTableWidget* m_table;
    QPushButton* m_loadBtn;
    QVector<NPC*> m_npcs;

    void setupUI();
    void loadFromTextFile(const QString& fileName);
    void loadFromJsonFile(const QString& fileName);
    void loadFromHardcodedData();
    void addToTable(NPC* npc);
    void updateTable();
    void clearAll();

    Element stringToElement(const QString& str);
    Rarity stringToRarity(const QString& str);
};

#endif // MAINWINDOW_H
