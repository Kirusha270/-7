#include "npc.h"

NPC::NPC(const QString& name, int health, const Armor& armor)
    : m_name(name), m_health(health), m_armor(armor)
{
}
