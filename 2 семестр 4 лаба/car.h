#ifndef CAR_H
#define CAR_H

#include <QString>
#include <QTextStream>

class Car
{
public:
    Car(const QString &brand, const QString &model, const QString &year,
        const QString &plate, bool isPetrol, bool isDiesel,
        bool heatedSeats, bool parkingSensors, bool mats);

    void writeToFile(QTextStream &out) const;

private:
    QString m_brand;
    QString m_model;
    QString m_year;
    QString m_plate;
    bool m_isPetrol;
    bool m_isDiesel;
    bool m_heatedSeats;
    bool m_parkingSensors;
    bool m_mats;
};

#endif // CAR_H
