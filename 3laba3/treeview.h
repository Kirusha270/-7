#pragma once
#include <QWidget>
#include <QPainter>
#include <QHash>
#include <QPointF>
#include <algorithm>
#include "tree.h"
// Виджет отрисовки дерева
class TreeView : public QWidget {
public:
    explicit TreeView(const BinaryTree *t, QWidget *parent = nullptr)
        : QWidget(parent), m_tree(t) {
        setMinimumSize(500, 350);
    }
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.fillRect(rect(), Qt::white);
        if (m_tree->isEmpty()) return;
        QHash<const TreeNode *, QPointF> pos;
        int counter = 0, maxDepth = 0;
        layout(m_tree->root(), 0, counter, maxDepth, pos);
        const double r = 18, margin = 30;
        double dx = counter > 1 ? (width() - 2 * margin) / (counter - 1) : 0;
        double dy = maxDepth > 0 ? (height() - 2 * margin) / maxDepth : 0;
        auto pt = [&](const TreeNode *n) {
            QPointF q = pos[n];
            return QPointF(counter > 1 ? margin + q.x() * dx : width() / 2.0,
                           margin + q.y() * dy);
        };
        p.setPen(QPen(Qt::darkGray, 2));
        for (auto it = pos.cbegin(); it != pos.cend(); ++it) {
            const TreeNode *n = it.key();
            if (n->left) p.drawLine(pt(n), pt(n->left));
            if (n->right) p.drawLine(pt(n), pt(n->right));
        }
        p.setPen(QPen(Qt::black, 2));

        for (auto it = pos.cbegin(); it != pos.cend(); ++it) {
            QPointF c = pt(it.key());
            p.setBrush(QColor(180, 215, 255));
            p.drawEllipse(c, r, r);
            p.drawText(QRectF(c.x() - r, c.y() - r, 2 * r, 2 * r), Qt::AlignCenter,
                       QString::number(it.key()->value));
        }
    }
private:
    const BinaryTree *m_tree;
    // x = порядковый номер при симметричном обходе, y = глубина
    static void layout(const TreeNode *n, int depth, int &counter, int &maxDepth,
                       QHash<const TreeNode *, QPointF> &pos) {
        if (!n) return;
        layout(n->left, depth + 1, counter, maxDepth, pos);
        pos[n] = QPointF(counter++, depth);
        maxDepth = std::max(maxDepth, depth);
        layout(n->right, depth + 1, counter, maxDepth, pos);
    }
};

