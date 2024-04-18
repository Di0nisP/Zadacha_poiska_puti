#ifndef MYGRAPHICVIEW_H
#define MYGRAPHICVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>   // Для формирования групп элементов
#include <QGraphicsRectItem>    // Для отрисовки элементов-квадратов
#include <QWheelEvent>          // Для использования колеса мыши
//#include <QTimer>
#include <QThread>
//#include <QDebug>
#include <QtConcurrent/QtConcurrent>

#include <random>
#include <chrono>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <thread>


/**
 * @brief Расширение класса QGraphicsView
 */
class MyGraphicView : public QGraphicsView
{
    Q_OBJECT

    using MyGraph = std::unordered_map< QGraphicsRectItem*, QVector<QGraphicsRectItem*> >;

public:
    explicit MyGraphicView(QWidget *parent = 0);
    ~MyGraphicView();

signals:
    void replotRequested();
    void createGraphRequested();

private slots:
    /**
     * @brief СЛОТ для перерисовки сцены
     */
    void replot();

    /**
     * @brief СЛОТ для формирования двусвязного списка \c squaresGraph
     * @warning Времязатратное выполнение. Нелинейная сложность.
     */
    void createGraph();

private:
    QGraphicsScene      *scene;             ///< Сцена для отрисовки

    QGraphicsItemGroup  *squaresGroup;      ///< Группа элементов-квадратов
    qreal                squareSize;        ///< Размер квадратов
    QColor               squareBrashColor;  ///< Цвет заливки квадратов
    qreal                numSquaresWidth;   ///< Параметр ширины (чило квадратов по горизонтали)
    qreal                numSquaresHeight;  ///< Параметр высоты (чило квадратов по вертикали)

    /**
     * @brief Двусвязный список
     *
     * Содержит \c std\::unordered_map, ключами которого являются квадраты-вершины,
     * а данными - \c std\::vector смежных квадратов.
     *
     * Каждый незакрашенный квадрат является ключом в двусвязном списке.
     *
     */
    MyGraph              squaresGraph;

    QGraphicsItemGroup  *literalsGroup;     ///< Группа элементов-литералов
    QPointF              pointA;            ///< Координаты точки начала пути
    QPointF              pointB;            ///< Координаты точки конца пути
    QGraphicsRectItem   *squareA;           ///< Квадрат начала пути
    QGraphicsRectItem   *squareB;           ///< Квадрат конца пути
    QGraphicsItemGroup  *arrowGroup;        ///< Группа элементов стрелки
    QColor               wayPenColor;

    /**
     * @brief Таймер для задержки отрисовки
     *
     * При создании окна и виджета необходимо некоторое время,
     * чтобы родительский слой развернулся,
     * чтобы принимать от него адекватные параметры ширины и высоты.
     *
     */
    QTimer              *timer;

private:
    /**
     * @brief Метод для удаления всех элементов из группы
     *
     * Перебираем все элементы сцены, и если они принадлежат группе,
     * переданной в метод, то удаляем их
     *
     * @param [in] group
     */
    void deleteItemsFromGroup(QGraphicsItemGroup *group);



    void findWay(QGraphicsRectItem* start,
                                        QGraphicsRectItem*   end);

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

    void mouseMoveEvent(QMouseEvent *event) override;

public:
    /**
     * @brief Метод инициализации и генерации лабиринта
     * @param [in] width  Ширина в квадратах
     * @param [in] height Высота в квадратах
     */
    void generate(const qreal& width, const qreal& height);
};

#endif // MYGRAPHICVIEW_H
