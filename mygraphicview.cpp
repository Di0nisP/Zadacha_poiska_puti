#include "mygraphicview.h"

MyGraphicView::MyGraphicView(QWidget *parent)
    : QGraphicsView(parent)
{
    scene = new QGraphicsScene();                               // Инициализируем сцену для отрисовки
    squaresGroup = new QGraphicsItemGroup();                    // Инициализируем группу квадратов
    squaresGroup->setZValue(2);                                 // Устанавливаем слоем уровня 2
    //squaresGroup->setFlags(QGraphicsItem::ItemContainsChildrenInShape);
    scene->addItem(squaresGroup);                               // Добавляем первую группу в сцену
    literalsGroup = new QGraphicsItemGroup();
    squaresGroup->setZValue(1);                                 // Устанавливаем слоем уровня 1
    scene->addItem(literalsGroup);

    // Настройки сцены:
    this->setScene(scene);                                      // Устанавливаем сцену в виджет
    this->setDragMode(QGraphicsView::ScrollHandDrag);           // Разрешаем перетаскивание
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Отключим скроллбар по горизонтали
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // Отключим скроллбар по вертикали
    this->setAlignment(Qt::AlignCenter);                        // Делаем привязку содержимого к центру
    this->setSizePolicy(QSizePolicy::Expanding,
                        QSizePolicy::Expanding);                // Растягиваем содержимое по виджету
    this->setRenderHint(QPainter::Antialiasing);                // Включаем сглаживание для лучшего отображения

    // Задаем параметры для отрисовки квадратов
    squareSize = 20;
    squareBrashColor = Qt::blue;

    // Настраиваем Таймер:
    timer = new QTimer();                                       // Инициализируем Таймер
    timer->setSingleShot(true);                                 // Таймер срабатывает единожды (по команде)
    connect(timer,
            SIGNAL(timeout()), this, SLOT(slotAlarmTimer()));   // Подключаем СЛОТ для отрисовки к Таймеру
}

MyGraphicView::~MyGraphicView()
{
    delete timer;
    deleteItemsFromGroup(literalsGroup);
    delete literalsGroup;
    deleteItemsFromGroup(squaresGroup);
    delete squaresGroup;
    delete scene;
}

void MyGraphicView::slotAlarmTimer()
{
    this->deleteItemsFromGroup(squaresGroup);
    this->deleteItemsFromGroup(literalsGroup);
    pointA = QPoint();
    pointB = QPoint();
    squaresGraph.clear();

    // Получаем текущее время с точностью до наносекунд
    auto now = std::chrono::high_resolution_clock::now();
    // Преобразуем время в наносекунды
    auto ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
    // Получаем количество наносекунд с начала эпохи
    auto nano_seconds = ns.time_since_epoch().count();

    // Создаем генератор случайных чисел:
    std::mt19937 gen(nano_seconds);
    std::uniform_real_distribution<> dist(0,1);

    // Добавление квадратов на сцену:
    for (size_t H = 0; H < numSquaresHeight; ++H) {
        for (size_t W = 0; W < numSquaresWidth; ++W) {
            //*
            QGraphicsRectItem *square =
                new QGraphicsRectItem(W * squareSize,   // x
                                      H * squareSize,   // y
                                          squareSize,   // width
                                          squareSize);  // height

            if (dist(gen) > 0.7) {
                square->setBrush(squareBrashColor);     // Устанавливаем заливку для квадрата
            } else {                
                square->setBrush(Qt::NoBrush);          // Отключаем заливку для квадрата
            }

            //square->setZValue(3);

            squaresGroup->addToGroup(square);           // Добавляем квадрат в группу (и на сцену)
        }
    }

    // Формирование графа:
    foreach(QGraphicsItem *item, squaresGroup->childItems()) {
        QGraphicsRectItem *square = qgraphicsitem_cast<QGraphicsRectItem*>(item);
        if (!square) continue;
        if (square->brush().style() == Qt::NoBrush) {
            qreal x = square->rect().x();
            qreal y = square->rect().y();
            foreach(QGraphicsItem *nbItem, square->collidingItems(Qt::IntersectsItemBoundingRect)) {
                QGraphicsRectItem* nbSquare = qgraphicsitem_cast<QGraphicsRectItem*>(nbItem);
                if (!nbSquare || nbSquare == square) continue;
                if (nbSquare->brush().style() == Qt::NoBrush) {
                    qreal nbX = nbSquare->rect().x();
                    qreal nbY = nbSquare->rect().y();
                    if ( (squareSize * 0.5 < qAbs(x - nbX) && qAbs(x - nbX) < squareSize * 1.5 && y == nbY) ||
                         (squareSize * 0.5 < qAbs(y - nbY) && qAbs(y - nbY) < squareSize * 1.5 && x == nbX) )
                    {
                        // Проверяем наличие square в squaresGraph:
                        auto it = squaresGraph.find(square);
                        if (it == squaresGraph.end()) {
                            // Если square не найден, создаем новый вектор tmp и добавляем nbSquare в него
                            QVector<QGraphicsRectItem*> tmp;
                            tmp.push_back(nbSquare);
                            // Вставляем square и связанный с ним вектор tmp в squaresGraph
                            squaresGraph.insert({square, tmp});
                        } else {
                            // Если square уже есть в squaresGraph, просто добав добав добавляем nbSquare в связанный с ним вектор
                            it->second.push_back(nbSquare);
                        }
                    }
                }
            }
        }
    }
}

void MyGraphicView::deleteItemsFromGroup(QGraphicsItemGroup *group)
{
    foreach( QGraphicsItem *item, scene->items(group->boundingRect())) {
       if(item->group() == group ) {
          delete item;
       }
    }
}

QVector<QGraphicsRectItem*> MyGraphicView::findWay(QGraphicsRectItem* start,
                                                   QGraphicsRectItem*   end)
{
    // Очередь для BFS
    std::queue<QGraphicsRectItem*> q;
    q.push(start);

    // Словарь для отслеживания пути
    std::unordered_map<QGraphicsRectItem*, QGraphicsRectItem*> path;
    path[start] = nullptr;

    // BFS
    while (!q.empty()) {
        QGraphicsRectItem* current = q.front();
        q.pop();

        // Проверяем соседей текущей вершины
        for (QGraphicsRectItem* neighbor : squaresGraph[current]) {
            if (!path.count(neighbor)) {
                q.push(neighbor);
                path[neighbor] = current;
            }
        }
    }

    // Восстановление пути
    QVector<QGraphicsRectItem*> shortestPath;
    QGraphicsRectItem* current = end;
    while (current != nullptr) {
        shortestPath.push_back(current);
        current = path[current];
    }

    std::reverse(shortestPath.begin(), shortestPath.end()); // Путь от начала к концу

    return shortestPath;
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
    // Проверяем, была ли нажата левая кнопка мыши без модификаторов клавиатуры
    if (event->button() == Qt::LeftButton && event->modifiers() == Qt::ControlModifier) {
        // Получаем координаты клика мыши
        QPointF clickPos = mapToScene(event->pos());

        // Находим объект, на который был произведен клик
        QGraphicsItem *item = scene->itemAt(clickPos, QTransform());

        // Проверяем, что был произведен клик на квадрате
        if (item && squaresGroup->isAncestorOf(item) && item->type() == QGraphicsRectItem::Type) {
            QGraphicsRectItem *square = qgraphicsitem_cast<QGraphicsRectItem *>(item);
            if (square->brush() == Qt::NoBrush) {
                if (!pointA.isNull()) {
                    pointA = QPointF(); // Сбрасываем точку А
                    foreach(QGraphicsItem *item, literalsGroup->childItems()) {
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
                textItem->setDefaultTextColor(Qt::red);     // Цвет текста
                textItem->setFont(QFont("Arial", 12));      // Шрифт и размер текста
                QPointF textPos = square->rect().center() - QPointF(textItem->boundingRect().width() / 2, textItem->boundingRect().height() / 2);
                textItem->setPos(textPos);
                literalsGroup->addToGroup(textItem);
            }
        }
    }

    if (event->button() == Qt::RightButton && !pointA.isNull()) {
        // Получаем координаты клика мыши
        QPointF clickPos = mapToScene(event->pos());

        // Находим объект, на который был произведен клик
        QGraphicsItem *item = scene->itemAt(clickPos, QTransform());

        // Проверяем, что был произведен клик на квадрате
        if (item && squaresGroup->isAncestorOf(item) && item->type() == QGraphicsRectItem::Type) {
            QGraphicsRectItem *square = qgraphicsitem_cast<QGraphicsRectItem *>(item);
            if (square->brush() == Qt::NoBrush) {
                if (!pointB.isNull()) {
                    pointB = QPointF(); // Сбрасываем точку B
                    foreach(QGraphicsItem *item, literalsGroup->childItems()) {
                        // Проверяем, является ли текущий элемент текстовым элементом
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
                textItem->setDefaultTextColor(Qt::red); // Цвет текста
                textItem->setFont(QFont("Arial", 12)); // Шрифт и размер текста
                QPointF textPos = square->rect().center() - QPointF(textItem->boundingRect().width() / 2, textItem->boundingRect().height() / 2);
                textItem->setPos(textPos);
                literalsGroup->addToGroup(textItem);
            }
        }
        auto way = findWay(squareA, squareB);
        //for (auto& i : way)


    }

    /// @todo Временное решение: перемещение нажатием левой кнопки мыши
    QGraphicsView::mousePressEvent(event);
}

void MyGraphicView::generate(const qreal& width, const qreal& height)
{
    numSquaresWidth  = width;
    numSquaresHeight = height;
    timer->start(0);
}
