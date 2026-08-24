#ifndef NPC_H
#define NPC_H

#include <QString>
#include <QWidget>

struct Armor {
    int helmet;
    int chest;
    int boots;
};

class NPC
{
public:
    NPC(const QString& name, int health, const Armor& armor);
    virtual ~NPC() = default;

    QString name() const { return m_name; }
    int health() const { return m_health; }
    Armor armor() const { return m_armor; }

    void setName(const QString& name) { m_name = name; }
    void setHealth(int health) { m_health = health; }
    void setArmor(const Armor& armor) { m_armor = armor; }

    virtual void craft(QWidget* parent = nullptr) = 0;
    virtual QString typeName() const = 0;
    virtual QString imagePath() const { return QString(); }

protected:
    QString m_name;
    int m_health;
    Armor m_armor;
};

#endif // NPC_H
