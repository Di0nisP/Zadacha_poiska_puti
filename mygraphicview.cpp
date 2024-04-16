#include "mygraphicview.h"

MyGraphicView::MyGraphicView(QWidget *parent)
    : QGraphicsView(parent)
{
    scene = new QGraphicsScene();                               // Инициализируем сцену для отрисовки
    squaresGroup = new QGraphicsItemGroup();                    // Инициализируем группу квадратов
    squaresGroup->setZValue(2);                                 // Устанавливаем слоем уровня 2
    scene->addItem(squaresGroup);                               // Добавляем первую группу в сцену
    literalsGroup = new QGraphicsItemGroup();
    squaresGroup->setZValue(1);                                 // Устанавливаем слоем уровня 1
    scene->addItem(literalsGroup);

    // Настройки сцены:
    this->setScene(scene);                                      // Устанавливаем сцену в виджет
    this->setDragMode(QGraphicsView::ScrollHandDrag);           //
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Отключим скроллбар по горизонтали
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // Отключим скроллбар по вертикали
    this->setAlignment(Qt::AlignCenter);                        // Делаем привязку содержимого к центру
    this->setSizePolicy(QSizePolicy::Expanding,
                        QSizePolicy::Expanding);                // Растягиваем содержимое по виджету
    this->setRenderHint(QPainter::Antialiasing);                // Включаем сглаживание для лучшего отображения

    // Задаем параметры для отрисовки квадратов
    squareSize = 20;                                            // Размер каждого квадрата
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
    // Удаляем все элементы со сцены,
    // если они есть, перед новой отрисовкой
    this->deleteItemsFromGroup(squaresGroup);
    this->deleteItemsFromGroup(literalsGroup);
    pointA = QPoint();
    pointB = QPoint();

    // Получаем текущее время с точностью до наносекунд
    auto now = std::chrono::high_resolution_clock::now();
    // Преобразуем время в наносекунды
    auto ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
    // Получаем количество наносекунд с начала эпохи
    auto nano_seconds = ns.time_since_epoch().count();

    // Создаем генератор случайных чисел:
    std::mt19937 gen(nano_seconds);
    std::uniform_real_distribution<> dist(0,1);

    // Генерируем поле с квадратами:
    for (size_t H = 0; H < numSquaresHeight; ++H) {
        for (size_t W = 0; W < numSquaresWidth; ++W) {
            // Создаем квадратный элемент
            QGraphicsRectItem *square =
                    new QGraphicsRectItem(W * squareSize,   // x
                                          H * squareSize,   // y
                                              squareSize,   // width
                                              squareSize);  // height

            // Задаем случайную заливку для квадрата:
            if (dist(gen) > 0.7) {
                square->setBrush(squareBrashColor);     // Устанавливаем заливку для квадрата
            } else {
                //square->setFlag(QGraphicsItem::ItemIsSelectable); // Разрешение выбора квадрата
                square->setBrush(Qt::NoBrush);  // Отключаем заливку для квадрата
            }

            squaresGroup->addToGroup(square);   // Добавляем квадрат в группу (и на сцену)
        }
    }
}

/* Метод для удаления всех элементов из группы
 * */
void MyGraphicView::deleteItemsFromGroup(QGraphicsItemGroup *group)
{
    /* Перебираем все элементы сцены, и если они принадлежат группе,
     * переданной в метод, то удаляем их
     * */
    foreach( QGraphicsItem *item, scene->items(group->boundingRect())) {
       if(item->group() == group ) {
          delete item;
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

                   this->pointA = square->rect().center();
                   // Устанавливаем букву "A" внутри квадрата
                   QGraphicsTextItem *textItem = new QGraphicsTextItem("A");
                   textItem->setDefaultTextColor(Qt::red); // Цвет текста
                   textItem->setFont(QFont("Arial", 12)); // Шрифт и размер текста
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

                   this->pointB = square->rect().center();
                   // Устанавливаем букву "A" внутри квадрата
                   QGraphicsTextItem *textItem = new QGraphicsTextItem("B");
                   textItem->setDefaultTextColor(Qt::red); // Цвет текста
                   textItem->setFont(QFont("Arial", 12)); // Шрифт и размер текста
                   QPointF textPos = square->rect().center() - QPointF(textItem->boundingRect().width() / 2, textItem->boundingRect().height() / 2);
                   textItem->setPos(textPos);
                   literalsGroup->addToGroup(textItem);
               }
           }
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
