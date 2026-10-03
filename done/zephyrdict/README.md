<img width="1280" height="720" alt="2026-09-25" src="https://github.com/user-attachments/assets/1a7fce85-f540-4401-ba58-20666b41f9b9" />

Youtube-запись от `2026-09-25`: https://youtu.be/7RQJwU7g4fE
# Zephyr на M5Stack со словарём
Можно относиться как к «маленькому компьютеру».
А значит, можно с чистой совестью ставить операционную систему.

Подробней: железо уже есть и уже согласовано.
Нет задачи собрать компьютер и согласовать подключённые железки.
Можно «сразу программировать».

[Zephyr](https://zephyrproject.org) — хороший выбор. Но не единственный, конечно.
- [?] **Какие ещё есть варианты?** [FreeRTOS](https://www.freertos.org) как минимум.

## Основные понятия реального мира

`Экосистема`, `среда разработки` — «вот это вот всё». Буквально всё в одном мешке.

`RTOS` — операционная система реального времени. Основная идея: всё, что можно, **не** буферизовано. Этим и отличается от «обычных» операционных систем. Можем рассчитывать на мгновенность передачи данных между частями системы.

Операционная система живёт у нас в виде кода на языке C прямо на диске.
Потому что ей нужно оборачивать любой наш код.
Исполняться будет она. И там уже внутри себя исполнять наш код.

Можно воспринимать как framework.

Ожидание: операционная система «живёт на компьютере» сама по себе. А программы мы ей «подбрасываем».
Реальность: операционная система приходит на компьютер вместе с программой.
- [?] **А можно сделать так, как ожидаем от ОС?** Нет, конечно, но вдруг да?

`Devicetree` — конфигурация железа. У разных компьютеров она, естественно, разная. Имеем дело с конкретным компьютером — значит, с конкретным Devicetree. Раз у нас компьютер готовый — значит, кто-то это Devicetree уже зафиксировал и отладил.

`Xtensa` — архитектура, она же система команд («ассемблер») основного микропроцессора. В эту систему команд надо будет в итоге перевести нашу программу. Код на языке C очень близок к любому «ассемблеру» — а значит, *иногда* есть смысл его писать с учётом нюансов архитектуры. Когда случается это «иногда», а когда нет, и что это за нюансы такие — отдельный очень интересный вопрос.

`HAL` — Hardware Abstraction Layer — «розетка» от производителя микроконтроллера, через которую OS и будет общаться с железом. Да-да, нюансы железа OS не знает и знать не хочет. Что сказал поставщик (вендор), тому и верит.

`toolbox` — весь мешок инструментов, которые нам понадобятся для разработки. Они есть двух типов: стандартные и те, что принадлежат именно системе Zephyr.

- Стандартные надо поставить заранее или по мере необходимости: `cmake`, `ninja`, `gperf`, `python3`, `dfu-util`, `openocd`. Как ни странно, менеджер сред разработки `west` тоже относится к стандартным.
 
- Специфические — это специфические для железа. Компиляторы уж точно. Обычно их собирают в один пакет и называют Software Development Kit (`SDK`). В прямом смысле это набор инструментов. Довольно низкоуровневых.


## Что приносит нам Zephyr

> Прежде чем он вообще что-то принесёт — сделайте для него уголок, в который всё тащить. Что такое «всё», потом разберёмся. Мой выбор — `~/.zephyr` (по аналогии с `~/.espressif`).

`Zephyr` — слово, которое может значить что угодно. Много разных штук:
- операционная система (конкретно RTOS);
- окружение разработки (тут ещё и модули вендоров подцепляются);
- SDK, то есть специфические инструменты разработки в их конфигурации.

Используем это слово во всех смыслах. Что, конечно, прибавляет понимания (нет).

 - [?] **Zephyr RTOS основан на Linux?** Уж больно похожи.

`workspace` — «прибитая гвоздями» конфигурация среды разработки. Фиксирует этот постоянно меняющийся дикий хаос, не даёт ему случайно обновляться там и сям и ломать проект. А в среде разработки Zephyr ооооочень много всего, контролировать это руками нереально. Да и зачем, если можно автоматизировать этот контроль. Бонусом можно переключаться между средами, если вы один проект пишете в одной среде, а другой в другой. Физически это файлы и папки где-то в одном конкретном месте основного компьютера.

`Zephyr SDK` — набор компиляторов и всякого такого. Кросс-компиляторы, линтеры, утилиты прошивки. Всё специфичное для железа. **Очень** большой пакет. И так же, как workspace, требует конкретики. Но тут хотя бы обновления единые, а не точечные там и сям.

Zephyr SDK вообще никак не связан с Zephyr, просто используется ей. Такая вот удобная подборка, которую создали создатели системы Zephyr для совместимости. Но инструменты всё те же: gcc (в той версии, которая точно скомпилирует код RTOS), openocd (с настройками, полезными для прошивки/отладки) и т.д. Конечно, этот SDK подзаточен под Zephyr. Конечно, можно его гордо отринуть и собрать свой.

Интересная связка: gcc фиксируется в SDK, а RTOS при этом живёт своей жизнью. Это значит, что для конкретной версии RTOS требуется конкретный диапазон версий SDK.

Знай и люби версию свой SDK:
- упомянуть в папке, где этот SDK установлен — например, `~/.zephyr/sdk_v0.17.0`
- посмотреть на переменную `ZEPHYR_SDK_INSTALL_DIR`
- заглянуть в `cmake/modules/sdk.cmake` внутри воркспейса

------------------------------
Начинаем с установки виртуальной среды для будущего workspace:
```bash
python3 -m venv ~/.zephyr/workspace_4.4.2_espressif/.venv
```
- создаём виртуальную Python-среду для изоляции всяких там Python-библиотек (они есть в инструментах)
- `~/.zephyr/` — сюда положим «всё зефирное» (вы не обязаны)
- `workspace_4.4.2_espressif` — рабочая среда (workspace) с версией экосистемы [Zephyr 4.4.2](https://github.com/zephyrproject-rtos/zephyr/releases/tag/v4.4.2), заточенная под разработку для микроконтроллеров производителя (вендора) Espressif (т.е. чаще всего ESP32)

Не забываем среду активировать:
```bash
source ~/.zephyr/workspace_4.4.2_espressif/.venv/bin/activate
```

**А деактивировать потом как?** Как обычно: `deactivate` из любого места операционной системы

Всё, дальше живём в этой среде. Чтобы не пачкать компьютер.

------------------------------

Если west стоит на уровне операционной системы — всё хорошо.
Но можно и в виртуальную среду поставить. Это всё равно.
Для Mac'а точно удобней в OS через Homebrew.

------------------------------
Мы просто создали виртуальную среду. Ещё никакого Zephyr тут нет.

Сейчас будет.

Инициируем workspace. Берём готовый. 

```bash
west init -m https://github.com/zephyrproject-rtos/zephyr ~/.zephyr/workspace_4.4.2_espressif
```

Посмотрим, что система говорит по ходу установки:
```bash
west init -m https://github.com/zephyrproject-rtos/zephyr ~/.zephyr/workspace_4.4.2_espressif
=== Initializing in /Users/op/.zephyr/workspace_4.4.2_espressif
--- Cloning manifest repository from https://github.com/zephyrproject-rtos/zephyr
Клонирование в «/Users/op/.zephyr/workspace_4.4.2_espressif/.west/manifest-tmp»...
remote: Enumerating objects: 1664697, done.
remote: Counting objects: 100% (1733/1733), done.
remote: Compressing objects: 100% (650/650), done.
remote: Total 1664697 (delta 1173), reused 1083 (delta 1083), pack-reused 1662964 (from 2)
Получение объектов: 100% (1664697/1664697), 942.62 MiB | 6.42 MiB/s, готово.
Определение изменений: 100% (1210484/1210484), готово.
Updating files: 100% (67956/67956), готово.
--- setting manifest.path to zephyr
=== Initialized. Now run "west update" inside /Users/op/.zephyr/workspace_4.4.2_espressif.
```

Нам казалось, что мы что-то инициируем. А система явно была уверена, что клонирует «репозиторий манифеста».

Что за манифест такой? Пойдём куда попало по новым файлам и сразу всё найдём.

Давайте сначала посмотрим, что к нам пришло: 
```bash
ls -D "" -la
total 0
drwxr-xr-x   5 op  staff   160  .
drwxr-xr-x   3 op  staff    96  ..
drwxr-xr-x   7 op  staff   224  .venv
drwxr-xr-x   3 op  staff    96  .west
drwxr-xr-x  56 op  staff  1792  zephyr
```

`.west` — явно каталог-маркер. Они все такие. По нему-то система и будет узнавать, что это не просто каталог, а каталог Zephyr workspace. Ну точь-в-точь как с `git`.

Что в нём? Одинокий файл `config`, даже без расширения.

А в нём?

```ini
[manifest]
path = zephyr
file = west.yml
```

Мы нашли манифест!

> Могли, кстати, и проще найти:
> ```shell
> west manifest --path
> ```

Идём туда. Какой-то YAML-файл. Что там интересного?

- babblesim — симулятор сетей, уже интересно
    ```yaml
    - name: babblesim 
    url-base: https://github.com/BabbleSim
    ```
- группировка чего-то (мы ещё не знаем, чего):
    ```yaml
    group-filter: [-babblesim, -optional, -testing]
    ```
- бесконечный список проектов (они-то и группируются!):
    ```yaml
     - name: hal_stm32
       revision: 37f1b7a34c6e0b684d0c4f1f3ac01ffb291b3e11
       path: modules/hal/stm32
       groups:
         - hal
    ```
  - часто упоминаются два каталога — `tools/` и `modules/`
   - [?] **Кто такие модули?** Ну мы догадываемся, но хочется знать точно.
  - кажется, группировать проекты очень просто, достаточно указать группу 

- и в конце отсылка к ещё одному YAML-файлу:
```yaml
   self:
     path: zephyr
     west-commands: scripts/west-commands.yml
     import: submanifests
```

Давайте и его заодно уж глянем:

```yaml
west-commands:
  - file: scripts/west_commands/completion.py
    commands:
      - name: completion
        class: Completion
        help: output shell completion scripts
  - file: scripts/west_commands/boards.py
    commands:
      - name: boards
        class: Boards
        help: display information about supported boards
```

Явно команды. Стоит попробовать запустить:
```bash
west list
```

Видим список каких-то репозиториев.
И их локальных адресов. Будущих. Пока по этим адресам ничего нет.
Всё нужно устанавливать.

**Всё?!**

Мы же собираемся работать только с микроконтроллерами [Espressif](https://www.espressif.com).
Есть ощущение, что нам об этом нужно как-то сообщить системе уже сейчас.

Для этого — группы.

Как увидеть группу?
```bash
west list -f "{name} [{groups}]" | grep "hal"
```

- [?] **Нужен манифест конкретно под Espressif — где взять?** Ну гуглить, наверное.

Может быть, на диске нам места и не жалко.
А вот в GitHub Actions для CI/CD может быть другая картинка.
Так что [тонкие настройки манифеста](https://docs.zephyrproject.org/latest/develop/west/manifest.html) заслуживают отдельного изучения.
Когда-нибудь.

------------------------------
Что нужно для экспериментов с манифестом?
Хорошо бы иметь уже склонированный репозиторий:
```bash
git clone https://github.com/zephyrproject-rtos/zephyr
```

И потом инициировать workspace из этой локальной копии:
```bash
west init -l .
```
- будут проблемы с `ZEPHYR_BASE`, если она уже установлена


------------------------------

Ладно, давайте устанавливать. Сначала как есть, эксперименты потом.

```bash
west update
=== updating acpica (modules/lib/acpica):
--- acpica: initializing
Инициализирован пустой репозиторий Git в /Users/op/.zephyr/workspace_4.4.2_espressif/modules/lib/acpica/.git/
--- acpica: fetching, need revision 8d24867bc9c9d81c81eeac59391cda59333affd4
remote: Enumerating objects: 144870, done.
remote: Counting objects: 100% (1051/1051), done.
remote: Compressing objects: 100% (177/177), done.
Получение объектов:  66% (95615/144870), 82.13 MiB | 5.98 MiB/s
…
```

Очевидно, клонирует репозитории. По списку, который мы уже видели. Это надолго.

Едет много всего действительно полезного, не зря ставим:
```log
…
=== updating lvgl (modules/lib/gui/lvgl):
--- lvgl: initializing
Инициализирован пустой репозиторий Git в /Users/op/.zephyr/workspace_4.4.2_espressif/modules/lib/gui/lvgl/.git/
--- lvgl: fetching, need revision d1fe35d350a1503db2ec7fd4dfff8e0cc26eea7e
remote: Enumerating objects: 144957, done.
remote: Counting objects: 100% (147/147), done.
remote: Compressing objects: 100% (20/20), done.
Получение объектов:  38% (55601/144957), 280.80 MiB | 5.60 MiB/s
…
```

Приехало:
```bash
ls -D "" -la
total 0
drwxr-xr-x   9 op  staff   288  .
drwxr-xr-x   4 op  staff   128  ..
drwxr-xr-x   7 op  staff   224  .venv
drwxr-xr-x   3 op  staff    96  .west
drwxr-xr-x   3 op  staff    96  bootloader
drwxr-xr-x   3 op  staff    96  doc
drwxr-xr-x   9 op  staff   288  modules
drwxr-xr-x   4 op  staff   128  tools
drwxr-xr-x  56 op  staff  1792  zephyr
```

Семь минут на установку.
Многовато. Определённо есть смысл в том, чтобы разобраться в группах манифеста.

------------------------------

Ну ладно, скачалось.

Но ещё не встало.

Это ведь всё сплошь Python-инструменты. А они любят зависимости.
Придётся ставить, куда денешься.

Посмотрим, что нам предстоит:
```bash
west packages pip
```
 - `pip` — менеджер зависимостей; `west` им только управляет

- [?] **А можно использовать `uv`, он же помодней будет?** Можно, вот [народ развлекается](https://github.com/zephyrproject-rtos/zephyr/issues/87260): `west packages pip | xargs uv pip install`

Поставим как обычно (и очень медленно):
```bash
west packages pip --install
```

> [!CAUTION]
> Возможны конфликты с уже установленными утилитами. \
> Например, с `libsigrok` у меня вся эта радость не подружилась.

Четыре минуты на установку.
Неплохо, но всё равно как-то заметно.

------------------------------

Теперь вспомним, что мы ставим не универсальный workspace, а под конкретную платформу.
Нас сегодня интересует [hal_espressif](https://github.com/zephyrproject-rtos/hal_espressif).
Пойдём почитаем.

**Ой!**

> Wi-Fi, Bluetooth, PHY, and coexistence libraries are distributed as binary blobs.

Мы знаем, почему так. Потому что код Espressif сильно не весь open source.
Вот эти вот все штуки они раздают только в скомпилированном виде.

Ладно, смотрим на месте:
```bash
cd ~/.zephyr/workspace_4.4.2_espressif/modules/hal/espressif
```

Тут явно есть в чём покопаться. Но вроде обещают, что всё уже встало. Скоро проверим.


------------------------------

Дальше снова поверим создателям Zephyr и сообщим CMake конфигурацию, которая будет нужна при компиляции:
```bash
west zephyr-export
```

Что получим? Знание CMake про то, где искать библиотеки.
Лежит в файле:
```bash
ls ~/.cmake/packages/Zephyr
69cdaf32d3f2841c2f14fa33da3611a2 b0da42909b5aad851fdae6324d178a7c
 
cat ~/.cmake/packages/Zephyr/b0da42909b5aad851fdae6324d178a7c
/Users/op/.zephyr/workspace_4.4.2_espressif/zephyr/share/zephyr-package/cmake%

cat ~/.cmake/packages/Zephyr/69cdaf32d3f2841c2f14fa33da3611a2
/opt/nordic/ncs/v3.4.0/zephyr/share/zephyr-package/cmake%
```

- [!] Хорошо бы к этому моменту уже **знать CMake** и понимать детальней, что происходит. Но с неба-то знания не упадут. Так что — закладка на будущее.

------------------------------

Смотрим, [какой релиз SDK нынче в моде](https://github.com/zephyrproject-rtos/sdk-ng/releases).

Потом инсталлируем в папку с его номером:
```bash
cd ~/.zephyr/workspace_4.4.2_espressif/zephyr
west sdk install -d ~/.zephyr/sdk_1.0.1
```
- важно перейти в папку `zephyr`, а иначе west не найдёт важные переменные
- «важные» — это, например, файл `SDK_VERSION`

> [!WARNING]
> **Ой!**
> Файлы `CLAUDE.md` и `AGENTS.md` — сюрприз!

- [?] Тоже много всего ползёт. **Как бы поменьше брать?** Не всё ж надо.


------------------------------

А можно не переходить в подкаталоги, а просто сообщить системе раз и навсегда, где они?

Можно и нужно:
```bash
# source .zenv
export ZEPHYR_BASE="$HOME/.zephyr/workspace_4.4.2_espressif/zephyr"
export ZEPHYR_SDK_INSTALL_DIR="$HOME/.zephyr/sdk_1.0.1"
export ZEPHYR_TOOLCHAIN_VARIANT="zephyr"

source "$HOME/.zephyr/workspace_4.4.2_espressif/.venv/bin/activate"
```

------------------------------

Давайте проверим, что система действительно в состоянии найти наш workspace (то есть код операционки) и SDK (то есть компилятор и его друзей):

```bash
west build -p always -b m5stack_stamps3/esp32s3/procpu --shield m5stack_cardputer zephyr/samples/drivers/display
```

Ан нет!
```log
CMake Error at /Users/op/.zephyr/workspace_4.4.2_espressif/zephyr/soc/espressif/common/CMakeLists.txt:8 (message):
  esptool>=5.0.2 not found in PATH.

  Please install it using:

    west packages pip --install
```

Врёт, между прочим. Надо так:
```bash
pip install esptool
```

Это не самый свежий esptool, но он точно встанет. И пример точно скомпилируется.

------------------------------

Теперь заливаем на компьютер.

```bash
west flash
```

Ну и всё.

------------------------------

Копаемся в примерах.

```bash
west build -p always -b m5stack_stamps3/esp32s3/procpu --shield m5stack_cardputer zephyr/samples/drivers/display

```


На [M5Stack Cardputer](https://docs.zephyrproject.org/latest/boards/shields/m5stack_cardputer/doc/index.html), да ещё и с другим микроконтроллером, встанет не всё. Какое поле для экспериментов!

[Нооооо…](https://docs.zephyrproject.org/latest/boards/shields/m5stack_cardputer/doc/index.html)

> [!NOTE]
> The NS4186 I2S Codec, SPM1423 microphone, IR LED and Keyboard functionality is not implemented yet.

- [?] **А что если мы тут самые умные?** И сами всё напишем?

Не знаю. Может, и правда.

