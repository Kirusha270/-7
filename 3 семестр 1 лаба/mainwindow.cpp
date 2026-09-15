#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

#include <cstdlib>
#include <cstring>

// ============================================================
//  Категории символов
// ============================================================
namespace {

enum Cat { CAT_VOWEL = 0, CAT_CONSONANT = 1, CAT_DIGIT = 2, CAT_SIGN = 3 };

bool isVowel(QChar c) {
    static const QString v = QString::fromUtf8("аеёиоуыэюяАЕЁИОУЫЭЮЯ");
    return v.contains(c);
}

bool isConsonant(QChar c) {
    static const QString k = QString::fromUtf8(
        "бвгджзйклмнпрстфхцчшщБВГДЖЗЙКЛМНПРСТФХЦЧШЩ");
    return k.contains(c);
}

int categoryOf(QChar c) {
    if (isVowel(c))     return CAT_VOWEL;
    if (isConsonant(c)) return CAT_CONSONANT;
    if (c.isDigit())    return CAT_DIGIT;
    return CAT_SIGN;
}

QString categoryName(int cat) {
    switch (cat) {
    case CAT_VOWEL:     return QString::fromUtf8("Гласные");
    case CAT_CONSONANT: return QString::fromUtf8("Согласные");
    case CAT_DIGIT:     return QString::fromUtf8("Цифры");
    default:            return QString::fromUtf8("Знаки");
    }
}

// Цвета для раскраски результата
QString colorFor(int cat) {
    switch (cat) {
    case CAT_VOWEL:     return "#d32f2f";  // красный
    case CAT_CONSONANT: return "#1976d2";  // синий
    case CAT_DIGIT:     return "#388e3c";  // зелёный
    default:            return "#f9a825";  // жёлтый (знаки)
    }
}

} // namespace

// ============================================================
//  Конструктор / деструктор
// ============================================================
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi();
}

MainWindow::~MainWindow() = default;

// ============================================================
//  Интерфейс
// ============================================================
void MainWindow::setupUi()
{
    setWindowTitle(QString::fromUtf8("Зубчатый массив и цветной вывод"));
    resize(760, 620);

    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* layout = new QVBoxLayout(central);

    layout->addWidget(new QLabel(QString::fromUtf8(
        "Введите строку (не более 50 символов, без латиницы):")));

    auto* inputRow = new QHBoxLayout;
    m_inputEdit = new QLineEdit;
    m_inputEdit->setMaxLength(50);
    m_inputEdit->setPlaceholderText(
        QString::fromUtf8("Например: Привет, мир! 42"));
    m_processBtn = new QPushButton(QString::fromUtf8("Обработать"));

    inputRow->addWidget(m_inputEdit, 1);
    inputRow->addWidget(m_processBtn);
    layout->addLayout(inputRow);

    m_outputEdit = new QTextEdit;
    m_outputEdit->setReadOnly(true);
    m_outputEdit->setFontFamily("Consolas");
    layout->addWidget(m_outputEdit, 1);

    m_saveBtn = new QPushButton(QString::fromUtf8("Сохранить отчёт в result.txt"));
    layout->addWidget(m_saveBtn);

    connect(m_processBtn, &QPushButton::clicked, this, &MainWindow::onProcessClicked);
    connect(m_saveBtn,    &QPushButton::clicked, this, &MainWindow::onSaveClicked);
    connect(m_inputEdit,  &QLineEdit::returnPressed, this, &MainWindow::onProcessClicked);
}

// ============================================================
//  Вариант 2: построение через QVector
// ============================================================
QVector<QVector<QChar>> MainWindow::buildJaggedVector(const QString& s) const
{
    QVector<QVector<QChar>> rows(4);
    QVector<QSet<QChar>>    seen(4);

    for (QChar c : s) {
        if (c == QChar('@')) continue;         // @ не попадает в массив
        const int cat = categoryOf(c);
        if (seen[cat].contains(c)) continue;   // без повторов
        seen[cat].insert(c);
        rows[cat].append(c);
    }
    return rows;
}

// ============================================================
//  Вариант 1: построение через malloc/realloc
// ============================================================
JaggedMalloc MainWindow::buildJaggedMalloc(const QString& s) const
{
    JaggedMalloc j;
    j.rows = static_cast<char**>(std::malloc(4 * sizeof(char*)));
    j.lens = static_cast<int*>(std::calloc(4, sizeof(int)));

    for (int i = 0; i < 4; ++i) {
        j.rows[i]    = static_cast<char*>(std::malloc(1));
        j.rows[i][0] = '\0';
    }

    QVector<QSet<QChar>> seen(4);
    for (QChar c : s) {
        if (c == QChar('@')) continue;
        const int cat = categoryOf(c);
        if (seen[cat].contains(c)) continue;
        seen[cat].insert(c);

        const QByteArray cb = QString(c).toUtf8();          // UTF-8!
        const int oldLen = j.lens[cat];
        j.rows[cat] = static_cast<char*>(
            std::realloc(j.rows[cat], oldLen + cb.size() + 1));
        std::memcpy(j.rows[cat] + oldLen, cb.constData(), cb.size());
        j.lens[cat] = oldLen + cb.size();
        j.rows[cat][j.lens[cat]] = '\0';
    }
    return j;
}

void MainWindow::freeJaggedMalloc(JaggedMalloc& j) const
{
    if (!j.rows) return;
    for (int i = 0; i < j.n; ++i) std::free(j.rows[i]);
    std::free(j.rows);
    std::free(j.lens);
    j.rows = nullptr;
    j.lens = nullptr;
}

// ============================================================
//  Цветной HTML для QTextEdit
QString MainWindow::coloredHtml(const QString& s) const
{
    QString html;
    for (QChar c : s) {
        // QChar -> QString, затем экранируем
        QString esc = QString(c).toHtmlEscaped();

        if (c == QChar('@')) {                 // @ — игнорируем при раскраске
            html += esc;
            continue;
        }
        if (c == QChar(' ')) esc = "&nbsp;";   // чтобы пробел не «схлопнулся»

        html += QString("<span style=\"color:%1;font-weight:bold;\">%2</span>")
                    .arg(colorFor(categoryOf(c)), esc);
    }
    return html;
}

// ============================================================
//  Обработка строки
// ============================================================
void MainWindow::onProcessClicked()
{
    m_input = m_inputEdit->text().trimmed();
    if (m_input.isEmpty()) {
        QMessageBox::warning(this,
                             QString::fromUtf8("Ошибка"),
                             QString::fromUtf8("Введите строку."));
        return;
    }
    if (m_input.size() > 50)
        m_input = m_input.left(50);

    // 1) строим массив двумя способами
    m_rowsVec = buildJaggedVector(m_input);
    JaggedMalloc jm = buildJaggedMalloc(m_input);

    // 2) результат = исходная строка + "+123АБВ"
    m_result = m_input + QString::fromUtf8("+123АБВ");

    // 3) собираем текстовый отчёт (то, что уйдёт в файл и в верхнюю часть окна)
    QString report;
    report += QString::fromUtf8("Входная строка: ") + m_input + "\n\n";

    report += QString::fromUtf8("Зубчатый массив (QVector):\n");
    for (int i = 0; i < 4; ++i) {
        report += "  " + categoryName(i)
        + QString::fromUtf8(" [%1]: ").arg(m_rowsVec[i].size());
        for (QChar c : m_rowsVec[i]) report += QString(c) + ' ';
        report += '\n';
    }

    report += QString::fromUtf8("\nЗубчатый массив (malloc/realloc):\n");
    for (int i = 0; i < 4; ++i) {
        const QString row = QString::fromUtf8(jm.rows[i]);
        report += "  " + categoryName(i)
                  + QString::fromUtf8(" [%1]: ").arg(row.size());
        for (QChar c : row) report += QString(c) + ' ';
        report += '\n';
    }

    report += QString::fromUtf8(
                  "\nРезультат (+\"+123АБВ\"): ") + m_result + "\n";

    report += QString::fromUtf8(
        "\nЛегенда раскраски: гласные — красные, согласные — синие, "
        "цифры — зелёные, знаки — жёлтые, символ @ не окрашивается.\n");

    m_plainReport = report;

    // 4) показываем в окне: сначала отчёт, затем цветной результат
    QString html;
    html += "<pre style=\"font-family:Consolas,monospace;\">";
    html += report.toHtmlEscaped();
    html += "</pre><hr>";
    html += "<div style=\"font-family:Consolas,monospace;font-size:14pt;"
            "white-space:pre-wrap;\">";
    html += coloredHtml(m_result);
    html += "</div>";

    m_outputEdit->setHtml(html);

    freeJaggedMalloc(jm);
}

// ============================================================
//  Сохранение отчёта в файл (UTF-8 с BOM — без «кракозябр»)
// ============================================================
void MainWindow::onSaveClicked()
{
    if (m_plainReport.isEmpty()) {
        QMessageBox::warning(this,
                             QString::fromUtf8("Нет данных"),
                             QString::fromUtf8("Сначала нажмите «Обработать»."));
        return;
    }

    const QString path = QFileDialog::getSaveFileName(
        this,
        QString::fromUtf8("Сохранить отчёт"),
        "result.txt",
        QString::fromUtf8("Текстовые файлы (*.txt)"));

    if (path.isEmpty()) return;

    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QMessageBox::critical(this,
                              QString::fromUtf8("Ошибка"),
                              QString::fromUtf8("Не удалось открыть файл:\n") + f.errorString());
        return;
    }

    QTextStream ts(&f);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    ts.setEncoding(QStringConverter::Utf8);
#else
    ts.setCodec("UTF-8");
#endif
    ts.setGenerateByteOrderMark(true);   // ← ключевой момент для Windows

    ts << m_plainReport;
    ts.flush();
    f.close();

    QMessageBox::information(this,
                             QString::fromUtf8("Готово"),
                             QString::fromUtf8("Отчёт сохранён в UTF-8 (с BOM):\n") + path);
}
