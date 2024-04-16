#ifndef MYGRAPHICVIEW_H
#define MYGRAPHICVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QGraphicsRectItem>    // Для отрисовки квадратов
#include <QTimer>

#include <QDebug>

#include <chrono>       // Для рандомизации на основе времени

#include <QWheelEvent>  // Для масштабирования сцены

/**
 * @brief Расширение класса QGraphicsView
 */
class MyGraphicView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MyGraphicView(QWidget *parent = 0);
    ~MyGraphicView();

signals:

private slots:
    /**
     * @brief СЛОТ для обработчика переполнения Таймера
     *
     * Производится перерисовка виджета
     *
     */
    void slotAlarmTimer();

private:
    QGraphicsScene      *scene;             ///< Сцена для отрисовки

    QGraphicsItemGroup  *squaresGroup;      ///< Группа элементов-квадратов
    qreal                squareSize;        ///< Размер каждого квадрата
    QColor               squareBrashColor;  ///< Цвет заливки квадратов
    qreal                numSquaresWidth;   ///< Параметр ширины (чило квадратов по горизонтали)
    qreal                numSquaresHeight;  ///< Параметр высоты (чило квадратов по вертикали)

    QGraphicsItemGroup  *literalsGroup;     ///< Объявляем вторую группу элементов
    QPointF              pointA;            ///< Координаты точки А
    QPointF              pointB;            ///< Координаты точки Б

    /**
     * @brief Таймер для задержки отрисовки
     *
     * При создании окна и виджета
     * необходимо некоторое время, чтобы родительский слой
     * развернулся, чтобы принимать от него адекватные параметры
     * ширины и высоты.
     *
     */
    QTimer              *timer;

private:
    /**
     * @brief Метод для удаления всех элементов из группы элементов
     * @param [in] group
     */
    void deleteItemsFromGroup(QGraphicsItemGroup *group);

    /**
     * @brief Метод поиска и отображения пути
     */
    void findWay();

protected:
    /**
     * @brief Обработчик события смещения колеса прокрутки мыши
     * @param [in] event
     */
    void wheelEvent(QWheelEvent *event) override;

    /**
     * @brief Обработчик события клика кнопки мыши
     * @param [in] event
     */
    void mousePressEvent(QMouseEvent *event) override;

public:
    /**
     * @brief Метод генерации лабиринта
     * @param [in] width  Ширина в квадратах
     * @param [in] height Высота в квадратах
     */
    void generate(const qreal& width, const qreal& height);
};

#endif // MYGRAPHICVIEW_H
