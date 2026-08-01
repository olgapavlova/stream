<img width="1280" height="720" alt="2026-07-10" src="https://github.com/user-attachments/assets/f3232fd8-f022-4954-a239-5c1df3eaf70a" />


Youtube-запись от `2026-07-10`: https://youtu.be/mXOjK-27A90

# Программируем рядышком с KiCad

- [KiCad](https://www.kicad.org) — программа для рисования вот такого
(и всего, что из него следует).

<img width="1305" height="919" alt="drawing" src="https://github.com/user-attachments/assets/90fd9ec1-5910-4fd8-94ac-da991f0beafc" />

- Чертёж в KiCad. PCB — если есть — там же.
- **А монтажную плату куда?**
<img width="3366" height="2442" alt="mvp-2" src="https://github.com/user-attachments/assets/14d77a5b-fcc9-4185-9cdb-7d1b00fe7b0c" />

> [!CAUTION]
> Давайте делать массивы в KiCad.\
> Потому что сам он не может.

## Нужны посадочные места
И неважно, что мы только что узнали этот термин.
Всё равно нужны.
- [ ] Массив `NxM`, и наверняка в нём есть подводные грабли
- [ ] Это не просто геометрия, там должна быть медь
- [ ] Мы знаем расстояние между точками — `2.54 мм`
- [ ] Надо, чтобы детали схемы «прилипали» к точкам
- [ ] Генерить будем где-то, где попроще
- [ ] А ещё круто будет получить 3D-модель!

### Что может быть проще текстовых файлов?

> [!TIP]
> Да, «штуки» в KiCad — обычные текстовые файлы.\
> Проекты, схемы, платы, библиотеки символов, посадочные места,…

- [Формат S-Expression](https://dev-docs.kicad.org/en/file-formats/sexpr-intro/index.html) — человекочитаем, хоть и с трудом
  &nbsp;
  &nbsp;
  
  
- Lisp • польская запись • IMAP-протокол • наверняка и другие гики
  &nbsp;
  &nbsp;
  
- Дерево с очень, очень простым обработчиком

<img width="253" height="300" alt="lisp-s-expression" src="https://github.com/user-attachments/assets/22efbe90-9832-479b-8956-ae48d8dc9c44" />


### Давайте сгенерим S-файл посадочного места
По сути мы хотим описать отверстия и их омеднение. Так и сделаем.

Все S-файлы выглядят *примерно* одинаково — там много скобок:
```bash
(файл_посадочного_места "Имя"
	(версия ВЕРСИЯ)
	(генератор КТО_СДЕЛАЛ_ТО)
	(точка)
	(точка)
	(точка)
	…
	(точка)
)
```


#### Что конкретно писать в файл
- Там будут — сюрприз! — токены
- В этом вся суть S-Expression Format

##### [Заголовок файла](https://dev-docs.kicad.org/en/file-formats/sexpr-footprint/index.html)

```text
(footprint "NAME"
	(version VERSION)
	(generator GENERATOR)
	…
)
```

Дальше — [очень много всяких вариантов](https://dev-docs.kicad.org/en/file-formats/sexpr-intro/index.html#_footprint) описания посадочных мест:

<img width="677" height="610" alt="footprint-details" src="https://github.com/user-attachments/assets/7339ae82-f4b2-45a3-92c7-4233bc758b34" />


Нам нужны:
```text
	…
	(layer F.Cu)
	(attr through_hole)
	(fp_text … )
	…
```

- На самом деле `fp_text` — это `GRAPHIC_ITEMS`
- А [вот так](https://dev-docs.kicad.org/en/file-formats/sexpr-intro/index.html#_footprint_graphics_items)!

```text
	…
	(fp_text reference "REF**"
		(at 0 -PITCH)
		(layer "F.SilkS"
		(effects
			(font (size 1 1) (thickness 0.15))
		)
	)
 
	(fp_text value "NAME"
		(at 0 ROWS * PITCH)
		(layer "F.Fab")
			(effects
				(font (size 1 1) (thickness 0.15))
			)
	)
	…
```

##### Контактная площадка

Тоже возможностей немало:

<img width="587" height="605" alt="pad-description" src="https://github.com/user-attachments/assets/bcd18fb6-a601-4b40-b11a-302d77f29b38" />


Нам нужны:
```text
	(pad "NAME" thru_hole circle
		(at X Y)
		(size D_PAD D_PAD)
		(drill D_HOLE)
		(layers "*.Cu" "*.Mask")
	)
```


#### Измерим макетную плату
- Расстояние между точками — 2,54 мм → расчёт `X` и `Y`
- Диаметр контактного пятна — 1,8 мм → `D_PAD`
- Диаметр отверстия — 1 мм → `D_HOLE`

#### Нюансы
- У библиотеки посадочных мест должен быть суффикс `.pretty`
- К `NAME` никаких особых требований — можно нумеровать
- На `*.Mask` можно и не писать — это для производства (чтобы площадку не закрасили)

### Всё знаем, можно писать код

==<вот в код и посмотрим, а не в конспект>==

> [!TIP]
> Пользуйтесь родным языком.\
> Тут `C` вовсе не обязателен.\
> Он пригодится **позже**.\
> Пока же — да хоть в Excel.

### И что теперь делать?
1. Настроим библиотеку
	- можно глобальную, можно проектную
2. Создадим отдельную плату
	- официально KiCad этого не умеет, но мы не скажем никому
3. Загрузим посадочные места прямо на плату
	- ==Редактор посадочных мест==
	- По умолчанию библиотеки спрятаны — покажем, найдём
4. Покидаем каких-нибудь деталек
		- не забудем и им присвоить посадочные места
		- для точного попадания в отверстия установим сетку с шагом 2.54 мм
5. Проведём соединения
	- `Cmd + Alt + W` — только под прямым углом
6. Посмотрим на эту красоту во FreeCAD
		- сначала верстак [KiCad StepUp](https://www.kicad.org/external-tools/stepup/)
		- дорожки добавляем отдельно — `Add Tracks`

> [!WARNING]
> Welcome to Ugly UI League 🔥
