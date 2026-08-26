#include "bag.h"

Bag::Bag(const QString &name, const QString &description,
         int slots, double maxWeight)
    : m_name(name), m_description(description),
      m_slots(slots), m_maxWeight(maxWeight)
{
}

QJsonObject Bag::toJson() const
{
    QJsonObject obj;
    obj["name"] = m_name;
    obj["description"] = m_description;
    obj["slots"] = m_slots;
    obj["maxWeight"] = m_maxWeight;
    return obj;
}

Bag Bag::fromJson(const QJsonObject &obj, bool *ok)
{
    Bag bag;
    bool localOk = true;

    if (obj.contains("name") && obj.value("name").isString())
        bag.m_name = obj.value("name").toString();
    else
        localOk = false;

    if (obj.contains("description") && obj.value("description").isString())
        bag.m_description = obj.value("description").toString();
    else
        localOk = false;

    if (obj.contains("slots") && obj.value("slots").isDouble())
        bag.m_slots = obj.value("slots").toInt(-1);
    else
        localOk = false;

    if (obj.contains("maxWeight") && obj.value("maxWeight").isDouble())
        bag.m_maxWeight = obj.value("maxWeight").toDouble(-1.0);
    else
        localOk = false;

    if (ok)
        *ok = localOk;

    return bag;
}

bool Bag::isValid() const
{
    if (m_name.trimmed().isEmpty())
        return false;
    if (m_description.trimmed().isEmpty())
        return false;
    if (m_slots <= 0)
        return false;
    if (m_maxWeight <= 0.0)
        return false;
    return true;
}
