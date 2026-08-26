#ifndef BAG_H
#define BAG_H

#include <QString>
#include <QJsonObject>

class Bag
{
public:
    Bag() = default;
    Bag(const QString &name, const QString &description,
        int slots, double maxWeight);

    QString name() const { return m_name; }
    QString description() const { return m_description; }
    int slots() const { return m_slots; }
    double maxWeight() const { return m_maxWeight; }

    void setName(const QString &name) { m_name = name; }
    void setDescription(const QString &description) { m_description = description; }
    void setSlots(int slots) { m_slots = slots; }
    void setMaxWeight(double maxWeight) { m_maxWeight = maxWeight; }

    QJsonObject toJson() const;
    static Bag fromJson(const QJsonObject &obj, bool *ok = nullptr);

    bool isValid() const;

private:
    QString m_name;
    QString m_description;
    int m_slots = -1;
    double m_maxWeight = -1.0;
};

#endif // BAG_H
