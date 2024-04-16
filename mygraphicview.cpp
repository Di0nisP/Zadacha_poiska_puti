#include "mygraphicview.h"



MyGraphicView::MyGraphicView(QWidget *parent)
    : QGraphicsView(parent)
{
    scene = new QGraphicsScene();   // Инициализируем сцену для отрисовки
    this->setScene(scene);          // Устанавливаем сцену в виджет

    this->setDragMode(QGraphicsView::ScrollHandDrag);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Отключим скроллбар по горизонтали
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // Отключим скроллбар по вертикали
    this->setAlignment(Qt::AlignCenter);                        // Делаем привязку содержимого к центру
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);    // Растягиваем содержимое по виджету
    this->setRenderHint(QPainter::Antialiasing); // Включаем сглаживание для лучшего отображения

    group_1 = new QGraphicsItemGroup(); // Инициализируем первую группу элементов
    scene->addItem(group_1);            // Добавляем первую группу в сцену

    timer = new QTimer();               // Инициализируем Таймер
    timer->setSingleShot(true);
    // Подключаем СЛОТ для отрисовки к таймеру
    connect(timer, SIGNAL(timeout()), this, SLOT(slotAlarmTimer()));
    //timer->start(50);                   // Стартуем таймер на 50 миллисекунд
}

MyGraphicView::~MyGraphicView()
{

}

void MyGraphicView::slotAlarmTimer()
{
    /* Удаляем все элементы со сцены,
     * если они есть перед новой отрисовкой
     * */
    this->deleteItemsFromGroup(group_1);

    // Задаем параметры для отрисовки квадратов
    qreal squareSize = 20;      // Размер каждого квадрата

    // Создаем генератор случайных чисел:
    std::mt19937 gen(time(0));
    std::uniform_real_distribution<> dist(0,1);

    // Генерируем поле с квадратами:
    for (size_t row = 0; row < numSquaresHeight; ++row) {
        for (size_t col = 0; col < numSquaresWidth; ++col) {
            // Создаем квадратный элемент:
            QGraphicsRectItem *square = new QGraphicsRectItem(col * squareSize, row * squareSize, squareSize, squareSize);

            // Задаем случайную заливку для квадрата:
            if (dist(gen) > 0.5) {
                square->setBrush(Qt::blue);     // Устанавливаем заливку для квадрата
            } else {
                square->setBrush(Qt::NoBrush);  // Отключаем заливку для квадрата
            }

            scene->addItem(square); // Добавляем квадрат на сцену
            qDebug("%f", static_cast<float>(dist(gen)));
            qDebug("    ");
        }
    }
    qDebug("\n");
}

/* Этим методом перехватываем событие изменения размера виджет
 * */
void MyGraphicView::resizeEvent(QResizeEvent *event)
{
    timer->start(50);   // Как только событие произошло стартуем таймер для отрисовки
    QGraphicsView::resizeEvent(event);  // Запускаем событие родителького класса
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
