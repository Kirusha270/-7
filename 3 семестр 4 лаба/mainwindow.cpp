#include "mainwindow.h"
#include <QComboBox>
#include <QCoreApplication>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QTextStream>
#include <QVBoxLayout>
#include <QLabel>
#include <cmath>

    // ---------------------------------------------------------------- GraphView
    GraphView::GraphView(Graph *g, QWidget *parent)
    : QWidget(parent), g_(g), map_(QCoreApplication::applicationDirPath() + "/map.png")
{
    setFixedSize(map_.size());
}

void GraphView::setRoute(const QVector<int> &vs, const QVector<int> &es)
{
    routeV_ = vs;
    routeE_ = QSet<int>(es.begin(), es.end());
    update();
}

void GraphView::clearRoute()
{
    routeV_.clear();
    routeE_.clear();
    update();
}

void GraphView::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawPixmap(0, 0, map_);

    // рёбра (цвет = загруженность), маршрут - синим и толще
    for (int i = 0; i < g_->edgeCount(); ++i) {
        const Edge &e = g_->edge(i);
        bool inRoute = routeE_.contains(i);
        p.setPen(QPen(inRoute ? QColor(0x3b, 0x4c, 0xd6) : Graph::colorFor(e.k),
                      inRoute ? 6 : 4, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(g_->pos(e.a), g_->pos(e.b));
    }

    // подписи рёбер в виде X(X*k)
    QFont f = font();
    f.setPointSize(7);
    p.setFont(f);
    for (int i = 0; i < g_->edgeCount(); ++i) {
        const Edge &e = g_->edge(i);
        QString t = QString("%1(%2)").arg(e.len, 0, 'f', 1).arg(e.len * e.k, 0, 'f', 1);
        QPointF m = (g_->pos(e.a) + g_->pos(e.b)) / 2;
        QRectF r(m.x() - 22, m.y() - 7, 44, 14);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255, 255, 255, 190));
        p.drawRoundedRect(r, 3, 3);
        p.setPen(Qt::black);
        p.drawText(r, Qt::AlignCenter, t);
    }

    // вершины
    f.setPointSize(8);
    f.setBold(true);
    p.setFont(f);
    for (int v = 0; v < g_->vertexCount(); ++v) {
        bool inRoute = routeV_.contains(v);
        p.setPen(QPen(inRoute ? QColor(0x3b, 0x4c, 0xd6) : Qt::black, 2));
        p.setBrush(QColor(190, 190, 190));
        p.drawEllipse(g_->pos(v), 7, 7);
        p.setPen(inRoute ? QColor(0x3b, 0x4c, 0xd6) : Qt::black);
        p.drawText(g_->pos(v) + QPointF(8, -8), g_->name(v));
    }
}

// двойной клик по вершине: 1-й клик задаёт А, 2-й - К
void GraphView::mouseDoubleClickEvent(QMouseEvent *e)
{
    for (int v = 0; v < g_->vertexCount(); ++v) {
        QPointF d = g_->pos(v) - e->position();
        if (std::hypot(d.x(), d.y()) <= 12) {
            emit vertexPicked(v);
            return;
        }
    }
}

// --------------------------------------------------------------- MainWindow
MainWindow::MainWindow()
{
    setWindowTitle("Графы и маршруты");
    buildGraph();

    view_ = new GraphView(&graph_);
    boxA_ = new QComboBox;
    boxK_ = new QComboBox;
    for (int v = 0; v < graph_.vertexCount(); ++v) {
        boxA_->addItem(graph_.name(v));
        boxK_->addItem(graph_.name(v));
    }
    boxK_->setCurrentIndex(graph_.vertexCount() - 1);

    auto *btnGo = new QPushButton("Вперёд!");
    auto *btnCong = new QPushButton("Имитация загруженности");
    auto *btnSave = new QPushButton("Сохранить");
    result_ = new QLabel("Выберите точки");
    result_->setWordWrap(true);
    result_->setMinimumHeight(40);

    auto *top = new QHBoxLayout;
    top->addWidget(new QLabel("Точка:"));
    top->addWidget(boxA_);
    top->addWidget(new QLabel("Точка:"));
    top->addWidget(boxK_);
    top->addWidget(btnGo);
    top->addWidget(btnCong);

    auto *bottom = new QHBoxLayout;
    bottom->addWidget(result_, 1);
    bottom->addWidget(btnSave);

    auto *lay = new QVBoxLayout;
    lay->addLayout(top);
    lay->addWidget(view_);
    lay->addLayout(bottom);
    auto *central = new QWidget;
    central->setLayout(lay);
    setCentralWidget(central);

    connect(btnGo, &QPushButton::clicked, this, &MainWindow::onBuildRoute);
    connect(btnCong, &QPushButton::clicked, this, &MainWindow::onCongestion);
    connect(btnSave, &QPushButton::clicked, this, &MainWindow::onSave);
    connect(view_, &GraphView::vertexPicked, this, &MainWindow::onVertexPicked);
}

// Граф, снятый вручную со скриншота (координаты в пикселях map.png).
// Если скриншот у вас другого размера - поправьте координаты.
void MainWindow::buildGraph()
{
    static const char *names[] = {"А","Б","В","Г","Д","Е","Ж","З","И","Й","К","Л","М",
                                  "Н","О","П","Р","С","Т","У","Ф","Х","Ц","Ч","Ш","Щ"};
    static const QPointF pts[] = {
                                  {31,38},{68,30},{146,25},{211,18},{315,25},{417,20},{417,65},{521,65},{580,84},
                                  {599,67},{53,84},{111,87},{136,90},{35,128},{101,166},{133,170},{43,205},{69,211},
                                  {47,230},{73,277},{91,337},{302,256},{389,270},{420,277},{477,285},{458,334}};
    for (int i = 0; i < 26; ++i)
        graph_.addVertex(QString::fromUtf8(names[i]), pts[i]);

    static const int edges[][2] = {
                                   {0,1},{1,2},{2,3},{3,4},{4,6},{5,6},{6,7},{7,8},{8,9},{0,13},{1,10},{10,13},
                                   {10,11},{11,12},{12,2},{12,3},{11,14},{14,15},{13,16},{16,17},{16,18},{18,19},
                                   {19,20},{19,25},{17,21},{14,17},{21,4},{21,22},{22,6},{22,23},{23,24},{23,7},{24,8}};
    for (auto &e : edges)
        graph_.addEdge(e[0], e[1]);
}

void MainWindow::onCongestion()
{
    graph_.randomizeCongestion();
    view_->clearRoute();
    result_->setText("Загруженность обновлена, постройте маршрут заново");
}

void MainWindow::onBuildRoute()
{
    int a = boxA_->currentIndex(), k = boxK_->currentIndex();
    if (a == k) {
        QMessageBox::warning(this, "Ошибка", "Выберите две разные точки");
        return;
    }
    QVector<int> vs, es;
    double total = 0;
    if (!graph_.shortestPath(a, k, vs, es, total)) {
        view_->clearRoute();
        routeText_.clear();
        result_->setText("Маршрута нет");
        QMessageBox::information(this, "Маршрут", "Маршрут между выбранными точками не найден");
        return;
    }
    QStringList parts;
    for (int v : vs) parts << graph_.name(v);
    routeText_ = parts.join("-");
    result_->setText(QString("%1  (время в пути: %2)").arg(routeText_).arg(total, 0, 'f', 1));
    routeText_ += QString("\nВремя в пути (длина*коэффициент): %1\n").arg(total, 0, 'f', 1);
    view_->setRoute(vs, es);
}

void MainWindow::onSave()
{
    if (routeText_.isEmpty()) {
        QMessageBox::information(this, "Сохранение", "Сначала постройте маршрут");
        return;
    }
    QString fn = QFileDialog::getSaveFileName(this, "Сохранить маршрут", "route.txt",
                                              "Текст (*.txt)");
    if (fn.isEmpty()) return;
    QFile f(fn);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл");
        return;
    }
    QTextStream out(&f);
    out.setEncoding(QStringConverter::Utf8);
    out << routeText_;
}

void MainWindow::onVertexPicked(int v)
{
    (pickA_ ? boxA_ : boxK_)->setCurrentIndex(v);
    pickA_ = !pickA_;
}
