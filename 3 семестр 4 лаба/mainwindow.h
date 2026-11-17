#pragma once
#include <QMainWindow>
#include <QPixmap>
#include <QSet>
#include <QWidget>
#include "graph.h"

class QComboBox;
class QLabel;

// Виджет: скриншот карты + поверх него нарисованный граф
class GraphView : public QWidget {
    Q_OBJECT
public:
    explicit GraphView(Graph *g, QWidget *parent = nullptr);
    void setRoute(const QVector<int> &vertices, const QVector<int> &edges);
    void clearRoute();

signals:
    void vertexPicked(int v);

protected:
    void paintEvent(QPaintEvent *) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;

private:
    Graph *g_;
    QPixmap map_;
    QVector<int> routeV_;
    QSet<int> routeE_;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow();

private slots:
    void onCongestion();
    void onBuildRoute();
    void onSave();
    void onVertexPicked(int v);

private:
    void buildGraph();

    Graph graph_;
    GraphView *view_;
    QComboBox *boxA_, *boxK_;
    QLabel *result_;
    QString routeText_;
    bool pickA_ = true;
};
