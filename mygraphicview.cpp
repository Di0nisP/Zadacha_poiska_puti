#include "mygraphicview.h"

MyGraphicView::MyGraphicView(QWidget *parent)
    : QGraphicsView(parent)
{
    scene = new QGraphicsScene();                               // Инициализируем сцену для отрисовки

    squaresGroup  = new QGraphicsItemGroup();                   // Инициализируем группу квадратов
    squaresGroup->setZValue(3);                                 // Устанавливаем слоем уровня 2
    scene->addItem(squaresGroup);                               // Добавляем первую группу в сцену

    literalsGroup = new QGraphicsItemGroup();
    squaresGroup->setZValue(2);                                 // Устанавливаем слоем уровня 1
    scene->addItem(literalsGroup);

    arrowGroup    = new QGraphicsItemGroup();
    arrowGroup->setZValue(1);
    scene->addItem(arrowGroup);

    // Настройки сцены:
    this->setScene(scene);                                      // Устанавливаем сцену в виджет
    this->setDragMode(QGraphicsView::ScrollHandDrag);           // Разрешаем перетаскивание
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Отключим скроллбар по горизонтали
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // Отключим скроллбар по вертикали
    this->setAlignment(Qt::AlignCenter);                        // Делаем привязку содержимого к центру
    this->setSizePolicy(QSizePolicy::Expanding,
                        QSizePolicy::Expanding);                // Растягиваем содержимое по виджету
    this->setRenderHint(QPainter::Antialiasing);                // Включаем сглаживание для лучшего отображения

    // Задаем дополнительные параметры для отрисовки
    squareSize       = 20;
    squareBrashColor = Qt::blue;
    wayPenColor      = Qt::red;

    // Настройка сигналов и слотов:
    connect(this, SIGNAL(replotRequested()),
            this, SLOT  (replot()));
    connect(this, SIGNAL(createGraphRequested(QProgressBar*)),
            this, SLOT  (createGraph(QProgressBar*)));
}

MyGraphicView::~MyGraphicView()
{
    //delete timer;
    deleteItemsFromGroup(arrowGroup);
    delete arrowGroup;
    deleteItemsFromGroup(literalsGroup);
    delete literalsGroup;
    deleteItemsFromGroup(squaresGroup);
    delete squaresGroup;
    delete scene;
}

void MyGraphicView::replot()
{
    // Удаление старых данных:
    deleteItemsFromGroup(arrowGroup);
    deleteItemsFromGroup(literalsGroup);
    deleteItemsFromGroup(squaresGroup);
    pointA = QPoint();    squareA = nullptr;
    pointB = QPoint();    squareB = nullptr;
    squareCount = 0;

    // Получаем текущее время с точностью до наносекунд
    auto now = std::chrono::high_resolution_clock::now();
    // Преобразуем время в наносекунды
    auto ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
    // Получаем количество наносекунд с начала эпохи
    auto nanoSeconds = ns.time_since_epoch().count();

    // Создаем генератор случайных чисел:
    std::mt19937 gen(nanoSeconds);
    std::uniform_real_distribution<> dist(0,1);

    qreal sizeMax = 100 * 100;
    qreal sizeFact = numSquaresHeight * numSquaresWidth;
    qreal oldW = numSquaresWidth;
    qreal oldH = numSquaresHeight;

    // Добавление квадратов на сцену:
    // из левого верхенего угла вправо и вниз:
    for (size_t H = 0; H < numSquaresHeight; ++H) {
        for (size_t W = 0; W < numSquaresWidth; ++W) {
            QGraphicsRectItem *square =
                new QGraphicsRectItem(W * squareSize,   // x
                                      H * squareSize,   // y
                                          squareSize,   // width
                                          squareSize);  // height
            if (dist(gen) > 0.7) {
                square->setBrush(squareBrashColor);     // Устанавливаем заливку для квадрата
            } else {                
                square->setBrush(Qt::NoBrush);          // Отключаем заливку для квадрата
                ++squareCount;
            }

            squaresGroup->addToGroup(square);           // Добавляем квадрат в группу (и на сцену)

            // Замедляет процесс - как следствие
            if (sizeMax < sizeFact)
                QCoreApplication::processEvents();          // Для обработки событий во время цикла
                if (oldW != numSquaresWidth || oldH != numSquaresHeight) return;
        }
    }
}

void MyGraphicView::deleteItemsFromGroup(QGraphicsItemGroup* group)
{
    scene->removeItem(group); // -

    auto childItems = group->childItems();

    foreach(QGraphicsItem *item, childItems)
        if(item && item->group() == group) {
            group->removeFromGroup(item);
            delete item;
        }

    scene->addItem(group);   // -
}

void MyGraphicView::createGraph(QProgressBar* parent)
{
    // Инициализациия прогресса:
    squaresGraph.clear(); // Удаление старых данных
    size_t progressValue = 0;
    parent->setValue(progressValue);
    parent->setVisible(true);

    // Задаём дипазон для сравнения (с запасом):
    qreal squareMinSize = squareSize * 0.5;
    qreal squareMaxSize = squareSize * 1.5;

    qreal oldW = numSquaresWidth;
    qreal oldH = numSquaresHeight;

    auto squares = squaresGroup->childItems();
    foreach(QGraphicsItem *item, squares) {
        QCoreApplication::processEvents(); // Для обработки событий во время цикла
        if (oldW != numSquaresWidth || oldH != numSquaresHeight) return;
        QGraphicsRectItem *square = qgraphicsitem_cast<QGraphicsRectItem*>(item);
        if (!square) continue;
        // Смотрим только незакрашенные квадраты:
        if (square->brush().style() == Qt::NoBrush) {
            qreal x = square->rect().x();
            qreal y = square->rect().y();
            // Поиск столкновений квадратов:
            auto collidingSquares = square->collidingItems(Qt::IntersectsItemBoundingRect);
            foreach(QGraphicsItem *nbItem, collidingSquares) {
                QGraphicsRectItem* nbSquare = qgraphicsitem_cast<QGraphicsRectItem*>(nbItem);
                if (!nbSquare || nbSquare == square) continue;
                // Смотрим только незакрашенные квадраты:
                if (nbSquare->brush().style() == Qt::NoBrush) {
                    qreal nbX = nbSquare->rect().x();
                    qreal nbY = nbSquare->rect().y();
                    // Правило для искомых столкновений:
                    if ( (squareMinSize < qAbs(x - nbX) && qAbs(x - nbX) < squareMaxSize && y == nbY) ||
                         (squareMinSize < qAbs(y - nbY) && qAbs(y - nbY) < squareMaxSize && x == nbX) )
                    {
                        // Проверяем наличие вершины square в squaresGraph:
                        auto it = squaresGraph.find(square);
                        if (it == squaresGraph.end()) { // Если обозначенная вершина отсутствует
                            QVector<QGraphicsRectItem*> tmp;
                            tmp.push_back(nbSquare);
                            squaresGraph.insert({square, tmp}); // Пара "вершина-данные"
                        } else {
                            it->second.push_back(nbSquare);
                        }
                    }
                }
            }
            // Обновление прогресса
            parent->setValue(static_cast<qreal>(++progressValue * 100)
                             / static_cast<qreal>(squareCount) );
        }
    }

    parent->setVisible(false);
}

void MyGraphicView::findWay(QGraphicsRectItem* start,
                            QGraphicsRectItem*   end)
{
    // Очередь для BFS:
    QQueue<QGraphicsRectItem*> q;
    q.push_back(start);

    // Словарь для отслеживания пути:
    std::unordered_map<QGraphicsRectItem*, QGraphicsRectItem*> way;
    way[start] = nullptr;

    // BFS:
    while (!q.empty()) {
        QGraphicsRectItem* current = q.front();
        q.pop_front();

        // Проверяем соседей текущей вершины:
        for (QGraphicsRectItem* neighbor : squaresGraph[current]) {
            // Проверяем, имеет ли сосед соседей:
            if (!way.count(neighbor)) {
                q.push_back(neighbor);
                way[neighbor] = current;
            }
        }
    }

    // Восстановление пути:
    QVector<QGraphicsRectItem*> shortestWay;
    QGraphicsRectItem* current = end;
    while (current != nullptr) {
        shortestWay.push_back(current);
        current = way[current];
    }

    // Путь от начала к концу
    std::reverse(shortestWay.begin(), shortestWay.end());

    // Отрисовка пути:
    if (shortestWay.size() > 1) {
        size_t arrowLength = shortestWay.size() - 1;
        for (size_t i = 0; i < arrowLength; ++i) {
            QGraphicsLineItem* arrow = new QGraphicsLineItem;
            qreal arrowheadSize  = squareSize / 4;
            if (i == 0 || i == arrowLength - 1) {
                qreal angle = atan2(shortestWay[i + 1]->boundingRect().center().y()
                                       - shortestWay[i]->boundingRect().center().y(),
                                         shortestWay[i + 1]->boundingRect().center().x()
                                       - shortestWay[i]->boundingRect().center().x());

                int bx = (squareSize / 4) * cos(angle);
                int by = (squareSize / 4) * sin(angle);

                if (arrowLength == 1) {
                    arrow->setLine(shortestWay[i]->boundingRect().center().x() + bx,
                                   shortestWay[i]->boundingRect().center().y() + by,
                                   shortestWay[i + 1]->boundingRect().center().x() - bx,
                                   shortestWay[i + 1]->boundingRect().center().y() - by);
                    // Добавление наконечника стрелки ---------------------------------------
                    qreal arrowheadAngle = acos(arrow->line().dx() / arrow->line().length());

                    if (arrow->line().dy() >= 0)
                        arrowheadAngle = (M_PI * 2) - arrowheadAngle;

                    QPointF arrowP1 = arrow->line().p2()
                        - QPointF(sin(arrowheadAngle + M_PI / 3) * arrowheadSize,
                                  cos(arrowheadAngle + M_PI / 3) * arrowheadSize);
                    QGraphicsLineItem* arrowL1 = new QGraphicsLineItem(arrowP1.x(),
                                                                       arrowP1.y(),
                                                                       arrow->line().x2(),
                                                                       arrow->line().y2());
                    arrowL1->setPen(wayPenColor);
                    arrowGroup->addToGroup(arrowL1);

                    QPointF arrowP2 = arrow->line().p2()
                        - QPointF(sin(arrowheadAngle + M_PI - M_PI / 3) * arrowheadSize,
                                  cos(arrowheadAngle + M_PI - M_PI / 3) * arrowheadSize);
                    QGraphicsLineItem* arrowL2 = new QGraphicsLineItem(arrowP2.x(),
                                                                       arrowP2.y(),
                                                                       arrow->line().x2(),
                                                                       arrow->line().y2());
                    arrowL2->setPen(wayPenColor);
                    arrowGroup->addToGroup(arrowL2);
                    // ----------------------------------------------------------------------
                } else if (i == 0) {
                    arrow->setLine(shortestWay[i]->boundingRect().center().x() + bx,
                                   shortestWay[i]->boundingRect().center().y() + by,
                                   shortestWay[i + 1]->boundingRect().center().x(),
                                   shortestWay[i + 1]->boundingRect().center().y());
                } else if (i == arrowLength - 1) {
                    arrow->setLine(shortestWay[i]->boundingRect().center().x(),
                                   shortestWay[i]->boundingRect().center().y(),
                                   shortestWay[i + 1]->boundingRect().center().x() - bx,
                                   shortestWay[i + 1]->boundingRect().center().y() - by);
                    // Добавление наконечника стрелки ---------------------------------------
                    qreal arrowheadAngle = acos(arrow->line().dx() / arrow->line().length());

                    if (arrow->line().dy() >= 0)
                        arrowheadAngle = (M_PI * 2) - arrowheadAngle;

                    QPointF arrowP1 = arrow->line().p2()
                        - QPointF(sin(arrowheadAngle + M_PI / 3) * arrowheadSize,
                                  cos(arrowheadAngle + M_PI / 3) * arrowheadSize);
                    QPointF arrowP2 = arrow->line().p2()
                        - QPointF(sin(arrowheadAngle + M_PI - M_PI / 3) * arrowheadSize,
                                  cos(arrowheadAngle + M_PI - M_PI / 3) * arrowheadSize);

                    QGraphicsLineItem* arrowL1 = new QGraphicsLineItem(arrowP1.x(),
                                                                       arrowP1.y(),
                                                                       arrow->line().x2(),
                                                                       arrow->line().y2());
                    arrowL1->setPen(wayPenColor);
                    arrowGroup->addToGroup(arrowL1);

                    QGraphicsLineItem* arrowL2 = new QGraphicsLineItem(arrowP2.x(),
                                                                       arrowP2.y(),
                                                                       arrow->line().x2(),
                                                                       arrow->line().y2());
                    arrowL2->setPen(wayPenColor);
                    arrowGroup->addToGroup(arrowL2);
                    // ----------------------------------------------------------------------
                }
            } else {
                arrow->setLine(shortestWay[i]->boundingRect().center().x(),
                               shortestWay[i]->boundingRect().center().y(),
                               shortestWay[i + 1]->boundingRect().center().x(),
                               shortestWay[i + 1]->boundingRect().center().y());
            }

            arrow->setPen(wayPenColor);
            arrowGroup->addToGroup(arrow);
        }
    }
}

void MyGraphicView::wheelEvent(QWheelEvent *event)
{
    // Масштабируем сцену при прокрутке колеса мыши
    qreal scaleFactor = 1.15; // Фактор масштабирования

    if (event->angleDelta().y() > 0)    // Qt 6.6
    //if (event->delta() > 0)           // Qt 5.6
        this->scale(scaleFactor, scaleFactor); // Увеличиваем масштаб
    else
        this->scale(1.0 / scaleFactor, 1.0 / scaleFactor); // Уменьшаем масштаб

    event->accept(); // Помечаем событие как обработанное
}

void MyGraphicView::mousePressEvent(QMouseEvent *event)
{
    // Проверяем, была ли нажата левая кнопка мыши + Ctrl
    if (event->button() == Qt::LeftButton && event->modifiers() == Qt::ControlModifier) {
        deleteItemsFromGroup(arrowGroup);

        // Получаем координаты клика мыши
        QPointF clickPos = mapToScene(event->pos());

        // Находим объект, на который был произведен клик
        QGraphicsItem *item = scene->itemAt(clickPos, QTransform());

        // Проверяем, что был произведен клик на квадрате
        if (item && squaresGroup->isAncestorOf(item) && item->type() == QGraphicsRectItem::Type) {
            QGraphicsRectItem *square = qgraphicsitem_cast<QGraphicsRectItem *>(item);
            if (square->brush() == Qt::NoBrush) {
                if (!pointA.isNull()) {
                    auto textItems = literalsGroup->childItems();
                    foreach(QGraphicsItem *item, textItems) {
                        // Проверяем, является ли текущий элемент текстовым элементом
                        QGraphicsTextItem *textItem = qgraphicsitem_cast<QGraphicsTextItem *>(item);
                        if (textItem) {
                            // Проверяем текст элемента
                            if (textItem->toPlainText() == "A") {
                                literalsGroup->removeFromGroup(textItem);
                                scene->removeItem(textItem);
                                delete textItem;
                            }
                        }
                    }
                }
                squareA = square;
                pointA = square->rect().center();
                // Устанавливаем букву "A" внутри квадрата
                QGraphicsTextItem *textItem = new QGraphicsTextItem("A");
                textItem->setDefaultTextColor(wayPenColor);     // Цвет текста
                textItem->setFont(QFont("Arial", 12));      // Шрифт и размер текста
                QPointF textPos = square->rect().center() - QPointF(textItem->boundingRect().width() / 2, textItem->boundingRect().height() / 2);
                textItem->setPos(textPos);
                literalsGroup->addToGroup(textItem);
            }
        }

        if (squareB)
            findWay(squareA, squareB);
    }

/*    if (event->button() == Qt::RightButton && !pointA.isNull()) {
        deleteItemsFromGroup(arrowGroup);

        // Получаем координаты клика мыши
        QPointF clickPos = mapToScene(event->pos());

        // Находим объект, на который был произведен клик
        QGraphicsItem *item = scene->itemAt(clickPos, QTransform());

        // Проверяем, что был произведен клик на квадрате
        if (item && squaresGroup->isAncestorOf(item) && item->type() == QGraphicsRectItem::Type) {
            QGraphicsRectItem *square = qgraphicsitem_cast<QGraphicsRectItem *>(item);
            if (square->brush() == Qt::NoBrush) {
                if (!pointB.isNull()) {
                    auto textItems = literalsGroup->childItems();
                    foreach(QGraphicsItem *item, textItems) {
                        QGraphicsTextItem *textItem = qgraphicsitem_cast<QGraphicsTextItem *>(item);
                        if (textItem) {
                            // Проверяем текст элемента
                            if (textItem->toPlainText() == "B") {
                                literalsGroup->removeFromGroup(textItem);
                                scene->removeItem(textItem);
                                delete textItem;
                            }
                        }
                    }
                }
                squareB = square;
                pointB = square->rect().center();
                // Устанавливаем букву "A" внутри квадрата
                QGraphicsTextItem *textItem = new QGraphicsTextItem("B");
                textItem->setDefaultTextColor(wayPenColor); // Цвет текста
                textItem->setFont(QFont("Arial", 12)); // Шрифт и размер текста
                QPointF textPos = square->rect().center() - QPointF(textItem->boundingRect().width() / 2, textItem->boundingRect().height() / 2);
                textItem->setPos(textPos);
                literalsGroup->addToGroup(textItem);
            }
        }

        findWay(squareA, squareB);
    }   //*/

    /// @todo Временное решение: перемещение нажатием левой кнопки мыши
    QGraphicsView::mousePressEvent(event);
}

void MyGraphicView::mouseMoveEvent(QMouseEvent *event)
{
    if (!pointA.isNull()) {
        deleteItemsFromGroup(arrowGroup);

        // Получаем координаты клика мыши
        QPointF clickPos = mapToScene(event->pos());

        // Находим объект, на который был произведен клик
        QGraphicsItem *item = scene->itemAt(clickPos, QTransform());

        // Проверяем, что был произведен клик на квадрате
        if (item && squaresGroup->isAncestorOf(item) && item->type() == QGraphicsRectItem::Type) {
            QGraphicsRectItem *square = qgraphicsitem_cast<QGraphicsRectItem *>(item);
            if (square->brush() == Qt::NoBrush) {
                if (!pointB.isNull()) {
                    auto textItems = literalsGroup->childItems();
                    foreach(QGraphicsItem *item, textItems) {
                        QGraphicsTextItem *textItem = qgraphicsitem_cast<QGraphicsTextItem *>(item);
                        if (textItem) {
                            // Проверяем текст элемента
                            if (textItem->toPlainText() == "B") {
                                literalsGroup->removeFromGroup(textItem);
                                scene->removeItem(textItem);
                                delete textItem;
                            }
                        }
                    }
                }
                squareB = square;
                pointB = square->rect().center();
                // Устанавливаем букву "A" внутри квадрата
                QGraphicsTextItem *textItem = new QGraphicsTextItem("B");
                textItem->setDefaultTextColor(wayPenColor); // Цвет текста
                textItem->setFont(QFont("Arial", 12)); // Шрифт и размер текста
                QPointF textPos = square->rect().center() - QPointF(textItem->boundingRect().width() / 2, textItem->boundingRect().height() / 2);
                textItem->setPos(textPos);
                literalsGroup->addToGroup(textItem);
            }
        }

        findWay(squareA, squareB);
    }

    // Вызываем базовую реализацию обработчика события
    QGraphicsView::mouseMoveEvent(event);
}

void MyGraphicView::generate(const qreal& width, const qreal& height, QProgressBar* parent)
{  
    numSquaresWidth  = width;
    numSquaresHeight = height;

    // Формирование поля:
    emit replotRequested();

    // Формирование графа:
    emit createGraphRequested(parent);
}
