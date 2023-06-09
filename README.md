# Городской маршрутизатор

## Проект в рамках обучения на курсе Яндекс Практикум

TransportCatalogue представляет собой интерактивный транспортный каталог с модулем ввода/вывода данных о маршрутах и остановках в формате `.JSON` и модулем отрисовки графического изображения карты маршрутов в формате `.SVG`.

## Реализованный функционал

Проект находится на стадии разработки и внедрения дополнительного функционала, но уже сейчас доступно большое количество операций для работы с ним:

* Загрузка данных в формате JSON и их парсинг за счёт применения собственной библиотеки `json.h`;
* Проецирование заданных географических расстояний между остановками на плоскость;
* Рендеринг карты маршрутов и остановок благодаря внедрению собственной библиотека `svg.h`;
* Поддержка стандартного для формата SVG выбора цветовой палитры, используемой при отрисовке карты;
* Хранение данных маршрутов и остановок в каталоге с использованием `std::string_view` и указателей.

### Используемые технологии

* C++ 17
* библиотека STL
* библиотека JSON
* библиотека SVG

### Сборка и запуск проекта

Сборка возможна с помощью IDE либо командной строки. Требуется компилятор С++ с поддержкой стандарта C++17 и выше.