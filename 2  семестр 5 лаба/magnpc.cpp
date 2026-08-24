#include "magnpc.h"
#include "carddialog.h"

MagNPC::MagNPC(const QString& name, Element element, int mana, int health, const Armor& armor)
    : NPC(name, health, armor), m_element(element), m_mana(mana)
{
}

void MagNPC::craft(QWidget* parent)
{
    CardDialog dialog(this, parent);
    dialog.exec();
}
