#include "mainwindow.h"
#include "morse.h"

#include <QCheckBox>
#include <QFile>
#include <QFileDialog>
#include <QFont>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QHeaderView>
#include <QTabWidget>
#include <QTableWidget>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("Азбука Морзе — вариант 3: радист в панике"));
    resize(820, 680);

    m_tabs = new QTabWidget;
    m_tabs->addTab(createEncodeTab(), QStringLiteral("1. Кодирование"));
    m_tabs->addTab(createTransmitTab(), QStringLiteral("2. Передача"));
    m_tabs->addTab(createDecodeTab(), QStringLiteral("3. Декодирование"));
    m_tabs->addTab(createRecoverTab(), QStringLiteral("4. Восстановление"));
    m_tabs->addTab(createTableTab(), QStringLiteral("Азбука Морзе"));

    m_log = new QPlainTextEdit;
    m_log->setReadOnly(true);
    m_log->setMaximumHeight(130);
    m_log->setPlaceholderText(QStringLiteral("Журнал работы"));

    auto *central = new QWidget;
    auto *lay = new QVBoxLayout(central);
    lay->addWidget(m_tabs, 1);
    lay->addWidget(m_log);
    setCentralWidget(central);
}

// ---------- Вспомогательные ----------
QWidget *MainWindow::makeEditorBlock(const QString &title, QPlainTextEdit *&edit, bool readOnly,
                                     bool withLoad, bool withSave)
{
    auto *box = new QGroupBox(title);
    auto *lay = new QVBoxLayout(box);

    edit = new QPlainTextEdit;
    edit->setReadOnly(readOnly);
    QFont f = edit->font();
    f.setFamily(QStringLiteral("Consolas"));
    f.setStyleHint(QFont::Monospace);
    edit->setFont(f);
    lay->addWidget(edit);

    if (withLoad || withSave) {
        auto *row = new QHBoxLayout;
        row->addStretch();
        QPlainTextEdit *e = edit;
        if (withLoad) {
            auto *b = new QPushButton(QStringLiteral("Загрузить из файла..."));
            connect(b, &QPushButton::clicked, this, [this, e] { loadInto(e); });
            row->addWidget(b);
        }
        if (withSave) {
            auto *b = new QPushButton(QStringLiteral("Сохранить в файл..."));
            connect(b, &QPushButton::clicked, this, [this, e] { saveFrom(e); });
            row->addWidget(b);
        }
        lay->addLayout(row);
    }
    return box;
}

void MainWindow::loadInto(QPlainTextEdit *edit)
{
    const QString path = QFileDialog::getOpenFileName(
        this, QStringLiteral("Открыть файл"), QString(),
        QStringLiteral("Текстовые файлы (*.txt);;Все файлы (*)"));
    if (path.isEmpty())
        return;

    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, QStringLiteral("Ошибка"),
                             QStringLiteral("Не удалось открыть файл:\n%1").arg(path));
        return;
    }
    QString text = QString::fromUtf8(f.readAll());
    if (!text.isEmpty() && text[0] == QChar(0xFEFF)) // BOM
        text.remove(0, 1);
    edit->setPlainText(text);
    logMessage(QStringLiteral("Загружен файл: %1").arg(path));
}

void MainWindow::saveFrom(QPlainTextEdit *edit)
{
    const QString path = QFileDialog::getSaveFileName(
        this, QStringLiteral("Сохранить файл"), QString(),
        QStringLiteral("Текстовые файлы (*.txt);;Все файлы (*)"));
    if (path.isEmpty())
        return;

    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::warning(this, QStringLiteral("Ошибка"),
                             QStringLiteral("Не удалось записать файл:\n%1").arg(path));
        return;
    }
    f.write(edit->toPlainText().toUtf8());
    logMessage(QStringLiteral("Сохранено в файл: %1").arg(path));
}

void MainWindow::logMessage(const QString &msg)
{
    m_log->appendPlainText(msg);
}

// ---------- Вкладки ----------
QWidget *MainWindow::createEncodeTab()
{
    auto *w = new QWidget;
    auto *lay = new QVBoxLayout(w);
    lay->addWidget(makeEditorBlock(QStringLiteral("Исходный текст"), m_encIn, false, true, false));

    auto *btn = new QPushButton(QStringLiteral("Закодировать"));
    connect(btn, &QPushButton::clicked, this, &MainWindow::onEncode);
    lay->addWidget(btn);

    lay->addWidget(makeEditorBlock(QStringLiteral("Код Морзе"), m_encOut, true, false, true));
    return w;
}

QWidget *MainWindow::createTransmitTab()
{
    auto *w = new QWidget;
    auto *lay = new QVBoxLayout(w);
    lay->addWidget(makeEditorBlock(QStringLiteral("Исходный код Морзе"), m_trIn, false, true, false));

    auto *btn = new QPushButton(QStringLiteral("Передать (внести случайные ошибки)"));
    connect(btn, &QPushButton::clicked, this, &MainWindow::onTransmit);
    lay->addWidget(btn);

    lay->addWidget(makeEditorBlock(QStringLiteral("Код после передачи"), m_trOut, true, false, true));
    return w;
}

QWidget *MainWindow::createDecodeTab()
{
    auto *w = new QWidget;
    auto *lay = new QVBoxLayout(w);
    lay->addWidget(makeEditorBlock(QStringLiteral("Код Морзе"), m_decIn, false, true, false));

    auto *row = new QHBoxLayout;
    m_decForce = new QCheckBox(QStringLiteral("Запустить восстановление принудительно"));
    row->addWidget(m_decForce);
    row->addStretch();
    auto *btn = new QPushButton(QStringLiteral("Декодировать"));
    connect(btn, &QPushButton::clicked, this, &MainWindow::onDecode);
    row->addWidget(btn);
    lay->addLayout(row);

    lay->addWidget(makeEditorBlock(QStringLiteral("Текст"), m_decOut, true, false, true));
    return w;
}

QWidget *MainWindow::createRecoverTab()
{
    auto *w = new QWidget;
    auto *lay = new QVBoxLayout(w);
    lay->addWidget(makeEditorBlock(QStringLiteral("Код Морзе с ошибками"), m_recIn, false, true, false));

    auto *btn = new QPushButton(QStringLiteral("Восстановить"));
    connect(btn, &QPushButton::clicked, this, &MainWindow::onRecover);
    lay->addWidget(btn);

    lay->addWidget(makeEditorBlock(QStringLiteral("Восстановленный код Морзе"), m_recOut, true, false, true));
    return w;
}

QWidget *MainWindow::createTableTab()
{
    const auto &t = Morse::table();

    auto *w = new QWidget;
    auto *lay = new QVBoxLayout(w);

    auto *tbl = new QTableWidget(static_cast<int>(t.size()), 2);
    tbl->setHorizontalHeaderLabels({QStringLiteral("Буква"), QStringLiteral("Код (* - точка, - - тире)")});
    tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tbl->setSelectionMode(QAbstractItemView::NoSelection);
    tbl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tbl->verticalHeader()->setVisible(false);

    int row = 0;
    for (const auto &kv : t) {
        auto *letter = new QTableWidgetItem(QString(kv.first));
        auto *code = new QTableWidgetItem(kv.second);
        letter->setTextAlignment(Qt::AlignCenter);
        code->setTextAlignment(Qt::AlignCenter);
        tbl->setItem(row, 0, letter);
        tbl->setItem(row, 1, code);
        ++row;
    }
    lay->addWidget(tbl);
    return w;
}

// ---------- Обработчики ----------
void MainWindow::onEncode()
{
    int unknown = 0;
    auto tokens = Morse::encode(m_encIn->toPlainText(), unknown);
    m_encOut->setPlainText(Morse::format(tokens));
    logMessage(QStringLiteral("Кодирование выполнено. Пропущено неизвестных символов: %1").arg(unknown));
}

void MainWindow::onTransmit()
{
    auto tokens = Morse::parse(m_trIn->toPlainText());
    int errs = Morse::applyPanic(tokens);
    m_trOut->setPlainText(Morse::format(tokens));
    logMessage(QStringLiteral("Передача выполнена. Внесено ошибок: %1").arg(errs));
}

// Диалог выбора варианта восстановления (общий для вкладок 3 и 4)
static Morse::Chooser makeChooser(QWidget *parent)
{
    return [parent](int letterNo, int wordNo, const QString &received, bool valid,
                    const vector<Morse::Candidate> &variants) -> int {
        QStringList items;
        for (const auto &c : variants)
            items << QStringLiteral("%1   (%2)").arg(c.ch).arg(c.code);

        const QString label =
            QStringLiteral("Буква №%1 (слово %2), принято: «%3»%4.\nВыберите вариант:")
                .arg(letterNo).arg(wordNo).arg(received)
                .arg(valid ? QStringLiteral(" (корректна)") : QStringLiteral(" (ошибка)"));

        bool ok = false;
        const QString item = QInputDialog::getItem(
            parent, QStringLiteral("Восстановление"), label, items, 0, false, &ok);
        return ok ? static_cast<int>(items.indexOf(item)) : -1;
    };
}

void MainWindow::onDecode()
{
    auto tokens = Morse::parse(m_decIn->toPlainText());
    const bool force = m_decForce->isChecked();

    if (force || Morse::hasErrors(tokens)) {
        logMessage(force ? QStringLiteral("Принудительное восстановление...")
                         : QStringLiteral("Обнаружены ошибки, запускаю восстановление..."));
        QStringList log;
        Morse::recover(tokens, force, makeChooser(this), log);
        for (const QString &s : log)
            logMessage(s);
    }
    m_decOut->setPlainText(Morse::decode(tokens));
    logMessage(QStringLiteral("Декодирование завершено"));
}

void MainWindow::onRecover()
{
    auto tokens = Morse::parse(m_recIn->toPlainText());
    QStringList log;
    Morse::recover(tokens, true, makeChooser(this), log);
    for (const QString &s : log)
        logMessage(s);

    m_recOut->setPlainText(Morse::format(tokens));
    logMessage(QStringLiteral("Текст: %1").arg(Morse::decode(tokens)));
}
