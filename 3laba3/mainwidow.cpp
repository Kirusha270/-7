#include "mainwindow.h"
#include "treeview.h"
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QPlainTextEdit>
#include <QGridLayout>
#include <QFileDialog>
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Лабораторная No3. Бинарные деревья");
    auto *central = new QWidget(this);
    setCentralWidget(central);
    m_view = new TreeView(&m_tree);
    auto *btnOpen = new QPushButton("Открыть дерево из файла");
    auto *btnPost = new QPushButton("Постфиксный обход (ЛПУ)");
    auto *btnSave = new QPushButton("Сохранить обход в файл");
    auto *btnCnt3 = new QPushButton("Узлы, кратные 3");
    auto *btnBst = new QPushButton("Это BST?");
    auto *btnSym = new QPushButton("Симметрично?");
    auto *btnSum = new QPushButton("Сумма без листьев и корня");
    auto *btnRel = new QPushButton("Родство A и B");
    auto *btnH = new QPushButton("Высота поддерева A");
    m_edA = new QLineEdit; m_edA->setPlaceholderText("A");
    m_edB = new QLineEdit; m_edB->setPlaceholderText("B");
    m_edPost = new QLineEdit; m_edPost->setReadOnly(true);
    m_log = new QPlainTextEdit; m_log->setReadOnly(true);
    auto *g = new QGridLayout(central);
    g->addWidget(m_view, 0, 0, 1, 4);
    g->addWidget(btnOpen, 1, 0);
    g->addWidget(btnPost, 1, 1);
    g->addWidget(btnSave, 1, 2);
    g->addWidget(btnCnt3, 1, 3);
    g->addWidget(new QLabel("Постфиксный обход:"), 2, 0);
    g->addWidget(m_edPost, 2, 1, 1, 3);
    g->addWidget(btnBst, 3, 0);
    g->addWidget(btnSym, 3, 1);
    g->addWidget(btnSum, 3, 2, 1, 2);

    g->addWidget(m_edA, 4, 0);
    g->addWidget(m_edB, 4, 1);
    g->addWidget(btnRel, 4, 2);
    g->addWidget(btnH, 4, 3);
    g->addWidget(m_log, 5, 0, 1, 4);
    connect(btnOpen, &QPushButton::clicked, this, &MainWindow::onOpen);
    connect(btnPost, &QPushButton::clicked, this, &MainWindow::onPostfix);
    connect(btnSave, &QPushButton::clicked, this, &MainWindow::onSave);
    connect(btnCnt3, &QPushButton::clicked, this, &MainWindow::onCount3);
    connect(btnBst, &QPushButton::clicked, this, &MainWindow::onBst);
    connect(btnSym, &QPushButton::clicked, this, &MainWindow::onSymmetric);
    connect(btnSum, &QPushButton::clicked, this, &MainWindow::onSum);
    connect(btnRel, &QPushButton::clicked, this, &MainWindow::onRelation);
    connect(btnH, &QPushButton::clicked, this, &MainWindow::onHeight);
    resize(720, 700);
}
void MainWindow::say(const QString &s) { m_log->appendPlainText(s); }
bool MainWindow::needTree() {
    if (m_tree.isEmpty()) {
        say("Дерево пусто — сначала загрузите файл.");
        return false;
    }
    return true;
}
bool MainWindow::readInt(QLineEdit *edit, int &out) {
    bool ok = false;
    out = edit->text().toInt(&ok);
    if (!ok) say("Введите целое число в поле " + edit->placeholderText());
    return ok;
}
void MainWindow::onOpen() {
    QString path = QFileDialog::getOpenFileName(this, "Файл с деревом", "", "Text (*.txt);;All (*)");
    if (path.isEmpty()) return;
    if (!m_tree.loadFromFile(path)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось прочитать дерево");
        m_view->update();
        return;
    }
    m_edPost->clear();
    m_view->update();

    say("Дерево загружено: " + path);
}
void MainWindow::onPostfix() {
    if (!needTree()) return;
    QStringList s;
    for (int v : m_tree.postorder()) s << QString::number(v);
    m_edPost->setText(s.join(" "));
}
void MainWindow::onSave() {
    if (m_edPost->text().isEmpty()) { say("Сначала выполните обход."); return; }
    QString path = QFileDialog::getSaveFileName(this, "Сохранить обход", "postfix.txt", "Text (*.txt)");
    if (path.isEmpty()) return;
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        say("Не удалось открыть файл для записи.");
        return;
    }
    QTextStream(&f) << m_edPost->text() << "\n";
    say("Обход сохранён: " + path);
}
void MainWindow::onCount3() {
    if (needTree()) say("Узлов, кратных 3: " + QString::number(m_tree.countDivisibleBy3()));
}
void MainWindow::onBst() {
    if (needTree()) say(m_tree.isBST() ? "Дерево является BST" : "Дерево НЕ является BST");
}
void MainWindow::onSymmetric() {
    if (needTree()) say(m_tree.isSymmetric() ? "Дерево симметрично" : "Дерево несимметрично");
}
void MainWindow::onSum() {
    if (needTree()) say("Сумма без листьев и корня: " + QString::number(m_tree.sumWithoutLeavesAndRoot()));
}
void MainWindow::onRelation() {
    int a, b;
    if (!needTree() || !readInt(m_edA, a) || !readInt(m_edB, b)) return;
    say(m_tree.relation(a, b));
}
void MainWindow::onHeight() {

    int a;
    if (!needTree() || !readInt(m_edA, a)) return;
    int h = m_tree.subtreeHeight(a);
    say(h < 0 ? "Вершина не найдена" : QString("Высота поддерева с вершиной %1: %2").arg(a).arg(h));
}

