#pragma once
#include <QColor>
#include <QPointF>
#include <QString>
#include <QVector>

    // Ребро: длина постоянная, k - переменный коэффициент загруженности
    struct Edge {
    int a, b;
    double len;
    double k;
};

// Граф, хранится СПИСКОМ РЁБЕР (вариант 3)
class Graph {
public:
    int addVertex(const QString &name, const QPointF &pos);
    void addEdge(int a, int b, double len = -1); // len < 0 -> считается по пикселям

    int vertexCount() const { return names_.size(); }
    int edgeCount() const { return edges_.size(); }
    const QString &name(int v) const { return names_[v]; }
    const QPointF &pos(int v) const { return pos_[v]; }
    const Edge &edge(int i) const { return edges_[i]; }

    void randomizeCongestion();
    static QColor colorFor(double k);

    // Кратчайший (по len*k) маршрут. false - маршрута нет.
    bool shortestPath(int from, int to, QVector<int> &vertices,
                      QVector<int> &edgeIds, double &total) const;

private:
    QVector<QString> names_;
    QVector<QPointF> pos_;
    QVector<Edge> edges_;
};
