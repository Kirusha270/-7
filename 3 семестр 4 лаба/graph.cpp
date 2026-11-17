#include "graph.h"
#include <QRandomGenerator>
#include <algorithm>
#include <cmath>

    namespace {
    const double KM_PER_PIXEL = 0.012; // масштаб карты (подберите под свой скриншот)
}

int Graph::addVertex(const QString &name, const QPointF &pos)
{
    names_.push_back(name);
    pos_.push_back(pos);
    return names_.size() - 1;
}

void Graph::addEdge(int a, int b, double len)
{
    if (len < 0) {
        QPointF d = pos_[a] - pos_[b];
        len = std::hypot(d.x(), d.y()) * KM_PER_PIXEL;
        len = std::max(0.1, std::round(len * 10) / 10);
    }
    edges_.push_back({a, b, len, 0.8});
}

// зелёный 0.8, оранжевый 1.2, красный 1.9, фиолетовый 3.3
QColor Graph::colorFor(double k)
{
    if (k < 1.0) return QColor(0x1f, 0xb0, 0x50);
    if (k < 1.5) return QColor(0xff, 0x8c, 0x00);
    if (k < 2.5) return QColor(0xe0, 0x1b, 0x24);
    return QColor(0x9b, 0x3f, 0xa8);
}

void Graph::randomizeCongestion()
{
    static const double ks[] = {0.8, 1.2, 1.9, 3.3};
    static const int weights[] = {40, 30, 20, 10}; // зелёных больше, фиолетовых меньше
    for (Edge &e : edges_) {
        int r = QRandomGenerator::global()->bounded(100), i = 0, acc = weights[0];
        while (r >= acc && i < 3) acc += weights[++i];
        e.k = ks[i];
    }
}

bool Graph::shortestPath(int from, int to, QVector<int> &vs,
                         QVector<int> &es, double &total) const
{
    const int n = names_.size();
    const double INF = 1e18;
    QVector<double> dist(n, INF);
    QVector<int> prevV(n, -1), prevE(n, -1);
    QVector<bool> done(n, false);
    dist[from] = 0;

    for (int it = 0; it < n; ++it) {
        int u = -1;
        for (int i = 0; i < n; ++i)
            if (!done[i] && dist[i] < INF && (u < 0 || dist[i] < dist[u])) u = i;
        if (u < 0 || u == to) break;
        done[u] = true;
        for (int e = 0; e < edges_.size(); ++e) {
            const Edge &ed = edges_[e];
            int v;
            if (ed.a == u) v = ed.b;
            else if (ed.b == u) v = ed.a;
            else continue;
            double w = ed.len * ed.k;
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                prevV[v] = u;
                prevE[v] = e;
            }
        }
    }
    if (dist[to] >= INF) return false;

    vs.clear();
    es.clear();
    for (int v = to; v != -1; v = prevV[v]) {
        vs.prepend(v);
        if (prevE[v] != -1) es.prepend(prevE[v]);
    }
    total = dist[to];
    return true;
}
