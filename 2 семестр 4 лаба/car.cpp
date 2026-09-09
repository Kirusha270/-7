#include "car.h"

Car::Car(const QString &brand, const QString &model, const QString &year,
         const QString &plate, bool isPetrol, bool isDiesel,
         bool heatedSeats, bool parkingSensors, bool mats)
    : m_brand(brand), m_model(model), m_year(year), m_plate(plate),
    m_isPetrol(isPetrol), m_isDiesel(isDiesel),
    m_heatedSeats(heatedSeats), m_parkingSensors(parkingSensors), m_mats(mats)
{
}

void Car::writeToFile(QTextStream &out) const
{
    out << "Марка: " << m_brand << "\n"
        << "Модель: " << m_model << "\n"
        << "Год выпуска: " << m_year << "\n"
        << "Гос. номер: " << m_plate << "\n"
        << "Двигатель: " << (m_isPetrol ? "Бензин" : (m_isDiesel ? "Дизель" : "Не выбран")) << "\n"
        << "Опции:\n"
        << "  Подогрев сидений: " << (m_heatedSeats ? "Да" : "Нет") << "\n"
        << "  Парктроники: " << (m_parkingSensors ? "Да" : "Нет") << "\n"
        << "  Коврики: " << (m_mats ? "Да" : "Нет") << "\n"
        << "----------------------------------------\n";
}
