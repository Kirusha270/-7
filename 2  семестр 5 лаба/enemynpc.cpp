#include "enemynpc.h"
#include "carddialog.h"

EnemyNPC::EnemyNPC(const QString& name, Rarity rarity, int damage, int health, const Armor& armor)
    : NPC(name, health, armor), m_rarity(rarity), m_damage(damage)
{
}

void EnemyNPC::craft(QWidget* parent)
{
    CardDialog dialog(this, parent);
    dialog.exec();
}
