#ifndef MAGNPC_H
#define MAGNPC_H

#include "npc.h"

enum class Element { Fire, Water, Earth, Other };

class MagNPC : public NPC
{
public:
    MagNPC(const QString& name, Element element, int mana, int health, const Armor& armor);
    void craft(QWidget* parent = nullptr) override;
    QString typeName() const override { return "Маг"; }

    Element element() const { return m_element; }
    int mana() const { return m_mana; }
    void setElement(Element elem) { m_element = elem; }
    void setMana(int mana) { m_mana = mana; }


    QString imagePath() const override { return m_imagePath; }
    void setImagePath(const QString& path) { m_imagePath = path; }

private:
    Element m_element;
    int m_mana;
    QString m_imagePath;
};

#endif // MAGNPC_H
