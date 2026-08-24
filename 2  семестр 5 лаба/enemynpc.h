#ifndef ENEMYNPC_H
#define ENEMYNPC_H

#include "npc.h"

enum class Rarity { Common, Rare, Legend, Other };

class EnemyNPC : public NPC
{
public:
    EnemyNPC(const QString& name, Rarity rarity, int damage, int health, const Armor& armor);
    void craft(QWidget* parent = nullptr) override;
    QString typeName() const override { return "Враг"; }

    Rarity rarity() const { return m_rarity; }
    int damage() const { return m_damage; }
    void setRarity(Rarity rarity) { m_rarity = rarity; }
    void setDamage(int damage) { m_damage = damage; }

    QString imagePath() const override { return m_imagePath; }
    void setImagePath(const QString& path) { m_imagePath = path; }

private:
    Rarity m_rarity;
    int m_damage;
    QString m_imagePath;
};

#endif // ENEMYNPC_H
