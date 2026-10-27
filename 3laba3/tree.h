#pragma once
#include <QString>
#include <QVector>
#include <QQueue>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QRegularExpression>
#include <algorithm>
// Узел дерева — тоже класс, а не структура
class TreeNode {
public:
    explicit TreeNode(int v) : value(v) {}
    int value;
    TreeNode *left = nullptr;
    TreeNode *right = nullptr;
};
class BinaryTree {
public:
    BinaryTree() = default;
    ~BinaryTree() { clear(); }
    BinaryTree(const BinaryTree &) = delete;
    BinaryTree &operator=(const BinaryTree &) = delete;
    const TreeNode *root() const { return m_root; }
    bool isEmpty() const { return m_root == nullptr; }
    void clear() { destroy(m_root); m_root = nullptr; }
    // ---------- Загрузка: послойный обход, "-" = пустое место ----------
    bool loadFromFile(const QString &path) {
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
        QString text = QTextStream(&f).readAll();
        return loadFromString(text);
    }
    bool loadFromString(const QString &text) {
        clear();
        QStringList t = text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        bool ok = true;

        m_root = build(t, 0, ok); // потомки узла i: 2i+1 и 2i+2
        if (!ok) clear();
        return ok;
    }
    // ---------- Постфиксный обход (ЛПУ: левое, правое, узел) ----------
    QVector<int> postorder() const {
        QVector<int> res;
        post(m_root, res);
        return res;
    }
    // ---------- Количество узлов, значение которых кратно 3 ----------
    int countDivisibleBy3() const { return countDiv3(m_root); }
    // ---------- Доп. задачи ----------
    // Дерево поиска (строгое: слева < узел < справа)
    bool isBST() const { return bst(m_root, nullptr, nullptr); }
    // Симметрично ли дерево (по структуре и значениям)
    bool isSymmetric() const {
        return !m_root || mirror(m_root->left, m_root->right);
    }
    // Высота поддерева с вершиной value (в рёбрах, лист = 0); -1 если нет такой вершины
    int subtreeHeight(int value) const {
        const TreeNode *n = find(m_root, value);
        return n ? height(n) : -1;
    }
    // Сумма элементов без корня и без листьев
    long long sumWithoutLeavesAndRoot() const { return sumInner(m_root, m_root); }
    // Кто чей потомок и какого поколения (1 — сын, 2 — внук, ...)
    QString relation(int a, int b) const {
        const TreeNode *na = find(m_root, a);
        const TreeNode *nb = find(m_root, b);
        if (!na || !nb) return "Одно из значений не найдено в дереве";
        if (na == nb) return "Это одна и та же вершина";
        int g = depthBelow(na, nb);
        if (g > 0)
            return QString("%1 — потомок %2, поколение: %3 (%2 — предок)").arg(b).arg(a).arg(g);
        g = depthBelow(nb, na);
        if (g > 0)
            return QString("%1 — потомок %2, поколение: %3 (%2 — предок)").arg(a).arg(b).arg(g);
        return "Вершины не являются предком и потомком друг друга";

    }
private:
    TreeNode *m_root = nullptr;
    static TreeNode *build(const QStringList &t, int i, bool &ok) {
        if (!ok || i >= t.size() || t[i] == "-") return nullptr;
        int v = t[i].toInt(&ok);
        if (!ok) return nullptr;
        TreeNode *n = new TreeNode(v);
        n->left = build(t, 2 * i + 1, ok);
        n->right = build(t, 2 * i + 2, ok);
        return n;
    }
    static void destroy(TreeNode *n) {
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }
    static void post(const TreeNode *n, QVector<int> &out) {
        if (!n) return;
        post(n->left, out);
        post(n->right, out);
        out.append(n->value);
    }
    static int countDiv3(const TreeNode *n) {
        if (!n) return 0;
        return (n->value % 3 == 0 ? 1 : 0) + countDiv3(n->left) + countDiv3(n->right);
    }
    static bool bst(const TreeNode *n, const int *lo, const int *hi) {
        if (!n) return true;
        if (lo && n->value <= *lo) return false;
        if (hi && n->value >= *hi) return false;
        return bst(n->left, lo, &n->value) && bst(n->right, &n->value, hi);
    }
    static bool mirror(const TreeNode *a, const TreeNode *b) {
        if (!a && !b) return true;
        if (!a || !b) return false;
        return a->value == b->value && mirror(a->left, b->right) && mirror(a->right, b->left);
    }
    static int height(const TreeNode *n) {
        if (!n) return -1;
        return 1 + std::max(height(n->left), height(n->right));
    }
    static const TreeNode *find(const TreeNode *n, int v) {

        if (!n) return nullptr;
        if (n->value == v) return n;
        if (const TreeNode *l = find(n->left, v)) return l;
        return find(n->right, v);
    }
    // расстояние от from вниз до target, -1 если target не в поддереве from
    static int depthBelow(const TreeNode *from, const TreeNode *target) {
        if (!from) return -1;
        if (from == target) return 0;
        int l = depthBelow(from->left, target);
        if (l >= 0) return l + 1;
        int r = depthBelow(from->right, target);
        return r >= 0 ? r + 1 : -1;
    }
    static long long sumInner(const TreeNode *n, const TreeNode *root) {
        if (!n) return 0;
        bool leaf = !n->left && !n->right;
        long long s = (n == root || leaf) ? 0 : n->value;
        return s + sumInner(n->left, root) + sumInner(n->right, root);
    }
};

