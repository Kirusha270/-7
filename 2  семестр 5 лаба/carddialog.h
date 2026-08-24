#ifndef CARDDIALOG_H
#define CARDDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPixmap>
#include <QPainter>
#include "npc.h"

class CardDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CardDialog(NPC* npc, QWidget* parent = nullptr);
    ~CardDialog() = default;

signals:
    void removeNpc(NPC* npc);

private slots:
    void onPrint();
    void saveImage();

private:
    NPC* m_npc;
    QLabel* m_imageLabel;
    QPushButton* m_cancelBtn;
    QPushButton* m_printBtn;
    QPushButton* m_saveImageBtn;

    void setupUI();
    void setupImage();
};

#endif // CARDDIALOG_H
