#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <filesystem>

#include "BmpImage.h"

class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

private:
    BmpImage original_;
    BmpImage current_;
    QLabel *label_ = nullptr;

    static std::filesystem::path toPath(const QString &s);

    void openFile();
    void saveFile();
    void showImage();

    template <typename F>
    void apply(F filter)
    {
        if (current_.empty())
            return;
        filter(current_);
        showImage();
    }
};

#endif // MAINWINDOW_H
