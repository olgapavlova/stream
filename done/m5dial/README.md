<img width="1280" height="720" alt="2026-08-14" src="https://github.com/user-attachments/assets/9546663c-a795-4569-bce5-7670ea2e3da4" />

Youtube-запись от `2026-08-14`: https://youtu.be/v3ToZhRdAHg

# Дизайн круглых интерфейсов сразу в коде

> [!WARNING]
> Обычно нет смысла делать продукт ради интерфейса

- Чаще всего интерфейс — вспомогательная и даже незаметная функция.
- Но круглый циферблат — уж слишком экзотичная штука.
- **Хочется** придумать продукт ради такого интерфейса.

### С вами говорит внутренний заказчик

> Пусть приложение показывает часы.
> *Просто часы.*
> **А, ещё!**
> Пусть показывает, в каком окне сколько времени я сегодня провела.
> ~~А, ещё!~~ **Хватит для начала.**


### Код напишем как попало
`Raycast` — собирает логи активных окон — ==TypeScript==
`Самописный сервер` — агрегирует и транслирует эти логи в JSON-формате — ==Python==
`M5Dial` — показывает дашборд — ==C==

> [!NOTE]
> Тут уже не до чистоты кода, лишь бы шевелилось.

### А вот дизайн будем прорабатывать

- [LVGL](https://lvgl.io) — библиотека для создания UI на разном железе.
- [LVGL Pro](https://lvgl.io/pro) — IDE для работы с этой библиотекой.

> [!TIP]
> Главное: LVGL Pro сразу показывает картинки 

<img width="725" height="316" alt="Pasted image 20260814175140" src="https://github.com/user-attachments/assets/6874455e-6171-42e3-9df8-b7103657a2c8" />

**Возможно**, это аргумент для изучения XML-нотации для описания такого дизайна.

#### Засосём дизайн в LVGL Pro

```mermaid
flowchart LR
code(код):::orange
behavior(поведение):::yellow
ui(UI):::blue
xml(XML):::green

code <==> behavior
code <==> ui

ui --> xml

classDef green fill:#9a9a00,stroke:#9a9a00,color:#fefbe5;
classDef blue fill:#0a6fc3,stroke:#0a6fc3,color:#fefbe5;
classDef yellow fill:#f3ac06,stroke:#f3ac06,color:#2b291f;
classDef orange fill:#d6670e,stroke:#d6670e,color:#fefbe5;

```


#### Сгенерим UI-код из XML

```mermaid
flowchart LR
xml(XML):::green
c_mock(_gen.c):::violet
adapter(адаптер):::orange
c_ui(UI):::blue

xml --> c_mock --> adapter --> c_ui

classDef green fill:#9a9a00,stroke:#9a9a00,color:#fefbe5;
classDef blue fill:#0a6fc3,stroke:#0a6fc3,color:#fefbe5;
classDef orange fill:#d6670e,stroke:#d6670e,color:#fefbe5;
classDef violet fill:#7c33de,stroke:#7c33de,color:#fefbe5;

```

- И вот у нас уже есть обновлённая версия UI-части **кода**.


#### Хотим [Figma](https://www.figma.com/), не хотим тратить [$20 000](https://lvgl.io/pro/pricing)

> [!NOTE]
> Хорошая задача на вечерок. Или нет.
