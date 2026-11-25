#include "mainwindow.h"
#include "Filters.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("BMP фильтры");
    resize(900, 650);

    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    auto *buttons = new QHBoxLayout();

    auto addBtn = [&](const QString &text, auto handler) {
        auto *b = new QPushButton(text, central);
        buttons->addWidget(b);
        connect(b, &QPushButton::clicked, this, handler);
    };

    addBtn("Открыть", [this] { openFile(); });
    addBtn("Сохранить", [this] { saveFile(); });
    addBtn("Серый", [this] { apply(Filters::grayscale); });
    addBtn("Негатив", [this] { apply(Filters::negative); });
    addBtn("Размытие", [this] { apply(Filters::blur); });
    addBtn("Резкость", [this] { apply(Filters::sharpen); });
    addBtn("Сброс", [this] { current_ = original_; showImage(); });

    label_ = new QLabel("Откройте BMP файл", central);
    label_->setAlignment(Qt::AlignCenter);

    auto *scroll = new QScrollArea(central);
    scroll->setWidget(label_);
    scroll->setWidgetResizable(true);

    layout->addLayout(buttons);
    layout->addWidget(scroll);
    setCentralWidget(central);
}

std::filesystem::path MainWindow::toPath(const QString &s)
{
    return std::filesystem::path(s.toStdWString());
}

void MainWindow::openFile()
{
    const QString name = QFileDialog::getOpenFileName(this, "Открыть BMP", {}, "BMP (*.bmp)");
    if (name.isEmpty())
        return;

    std::string err;
    BmpImage img;
    if (!img.load(toPath(name), err)) {
        QMessageBox::warning(this, "Ошибка", QString::fromStdString(err));
        return;
    }
    original_ = current_ = std::move(img);
    showImage();
}

void MainWindow::saveFile()
{
    if (current_.empty())
        return;

    QString name = QFileDialog::getSaveFileName(this, "Сохранить BMP", {}, "BMP (*.bmp)");
    if (name.isEmpty())
        return;
    if (!name.endsWith(".bmp", Qt::CaseInsensitive))
        name += ".bmp";

    std::string err;
    if (!current_.save(toPath(name), err))
        QMessageBox::warning(this, "Ошибка", QString::fromStdString(err));
}

void MainWindow::showImage()
{
    if (current_.empty())
        return;
    // QImage нужен только для отображения готовых пикселей
    QImage qi(current_.data.data(), current_.width, current_.height,
              current_.width * 3, QImage::Format_RGB888);
    label_->setPixmap(QPixmap::fromImage(qi.copy()));
}
