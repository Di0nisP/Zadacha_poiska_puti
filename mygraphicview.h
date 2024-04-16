#ifndef MYGRAPHICVIEW_H
#define MYGRAPHICVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QGraphicsItemGroup>
#include <QTimer>

#include <QGraphicsRectItem>
#include <QDebug>

#include <ctime>

#include <QWheelEvent>

// Расширяем класс QGraphicsView
class MyGraphicView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MyGraphicView(QWidget *parent = 0);
    ~MyGraphicView();

signals:

public slots:
    void slotAlarmTimer();  /* слот для обработчика переполнения таймера
                             * в нём будет производиться перерисовка
                             * виджета
                             * */

private:
    QGraphicsScene      *scene;     // Объявляем сцену для отрисовки
    QGraphicsItemGroup  *group_1;   // Объявляем первую группу элементов

public:
    qreal numSquaresWidth;
    qreal numSquaresHeight;
//    QGraphicsItemGroup  *group_2;   // Объявляем вторую группу элементов

    /* Таймер для задержки отрисовки.
     * Дело в том, что при создании окна и виджета
     * необходимо некоторое время, чтобы родительский слой
     * развернулся, чтобы принимать от него адекватные параметры
     * ширины и высоты
     * */
    QTimer              *timer;

private:
    /* Перегружаем событие изменения размера окна,
     * чтобы перехватывать его
     * */
    void resizeEvent(QResizeEvent *event);
    /* Метод для удаления всех элементов
     * из группы элементов
     * */
    void deleteItemsFromGroup(QGraphicsItemGroup *group_1);

    void wheelEvent(QWheelEvent *event) override {
            //timer->start(50);
            // Масштабируем сцену при прокрутке колеса мыши
            qreal scaleFactor = 1.15; // Фактор масштабирования

            if (event->delta() > 0)
                scale(scaleFactor, scaleFactor); // Увеличиваем масштаб
            else
                scale(1.0 / scaleFactor, 1.0 / scaleFactor); // Уменьшаем масштаб

            event->accept(); // Помечаем событие как обработанное
        }
};

#endif // MYGRAPHICVIEW_H
