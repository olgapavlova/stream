# Отделяем математику от программирования

- [Wolfram](https://www.wolfram.com/?source=nav) — вся математика в одном движке
- WSTP: https://www.wolfram.com/wstp/index.php.ru
- C/Link: https://reference.wolfram.com/language/guide/CLanguageInterface.html


## Халява, сэр!

Фишка Raspberry Pi: Wolfram Language & Wolfram Engine доступны для некоммерческого использования.
Прям хорошая там лицензия именно для Raspberry Pi: https://www.wolfram.com/raspberry-pi/
<img width="1002" height="268" alt="Pasted image 20260731120205" src="https://github.com/user-attachments/assets/22ade822-1393-40eb-96db-81772c193b1f" />

Ставим [Wolfram Engine](https://www.wolfram.com/engine/) `14.2.1` **не из репозитория** (в версии `14.3` ошибка с лицензированием):
```bash
cd /tmp
wget https://wolfr.am/wolfram-engine.deb
sudo apt install ./wolfram-engine.deb
```

> [!CAUTION]
> `https://wolfr.am/wolfram-engine.deb` — слишком умная ссылка.
> Если она не угадает вашу архитектуру — берите из [архива](https://archive.raspberrypi.org/debian/pool/main/w/wolfram-engine/)


Создаём Wolfram ID для активации:
<img width="847" height="448" alt="Pasted image 20260731120935" src="https://github.com/user-attachments/assets/577b96fb-c246-4b75-8f36-7edce7277f11" />


https://www.wolfram.com/engine/free-license/

<img width="695" height="409" alt="Pasted image 20260731121147" src="https://github.com/user-attachments/assets/efd65ea3-1a35-4834-86fb-f5076b352f06" />
<img width="694" height="529" alt="Pasted image 20260731121116" src="https://github.com/user-attachments/assets/ab220af8-981c-47f9-8485-14559cfc44f0" />

Запускаем ядро:
```bash
cd /opt/Wolfram/WolframEngine/13.3/Executables/
./WolframKernel
```

Попросит ключ.
Его берём по ссылке, про которую вам сам Wolfram почему-то не говорит: https://www.wolframcloud.com/users/user-current/activationkeys
```bash
dpi • Executables $ ./WolframKernel
Mathematica 13.3.1 Kernel for Linux ARM (64-bit)
Copyright 1988-2023 Wolfram Research, Inc.

Mathematica 13.3.1 Kernel cannot find a valid password.

For automatic Web Activation enter your activation key
(enter return to skip Web Activation): |
```

Допустим, ключ правильный:
```bash
Automatic Web Activation received a password.

Creating password file entry in:
/home/op/.Mathematica/Licensing/mathpass

In[1]:= 2+4

Out[1]= 4
```

Не так сложно войти, как выйти:
```wolfram
In[2]:=Quit
```

- GUI-версия тоже придёт с этой установкой — но сейчас не о ней.


## Немножко осваиваем сам язык Wolfram Mathematica

Основная идея — вычисляем выражения.
Symbolic programming.
Оперирует абстрактными синтаксическими деревьями.
Близко к Lisp.
Немного, совсем немного, похоже на функциональное программирование.
Короче, можно изучать непривычную парадигму программирования, и это хорошо.

### Проклятье факториала
В общем, уже не проклятье: `384!` считает мгновенно.

### Можем рисовать картинки
Однострочником:
```bash
wolframscript \
  -code 'Plot[Sin[x]/x, {x, 0, 2 Pi}]' \
  -format PNG \
  > sinx_x.png
```

Честной функцией:
```bash
wolframscript -code '
Export[
  "sinx.png",
  Plot[Sin[x], {x, 0, 2 Pi}]
]
'
```

Да и саму функцию:
```wolfram
In[3]:=Export["function.png", Series[Factorial[x], {x, \[Infinity], 2}] // FullSimplify // Normal]
```

В терминале смотрим через `viu`

### С картами вообще красота
```bash
wolframscript -code '
center = GeoPosition[{32.812939, 35.000097}];

map = GeoGraphics[
  {},
  GeoCenter -> center,
  GeoRange -> Quantity[1, "Kilometers"],
  GeoRangePadding -> None,
  GeoScaleBar -> "Kilometers",
  ImageSize -> 1400
];

Export["streets.png", map, "PNG"]
'
```


### И наконец, ~~автомобиль~~ дифференциальные уравнения

Создадим файл ballistics.wls:
```wolfram
v0 = 50;
g = 9.81;

angles = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 80} Degree;

plots = Table[
  theta = angles[[i]];

  Plot[
    x Tan[theta] - g x^2/(2 v0^2 Cos[theta]^2),
    {x, 0, v0^2 Sin[2 theta]/g},
    PlotStyle -> ColorData[97][i]
  ],
  {i, Length[angles]}
];

image = Show[
  plots,
  PlotRange -> All,
  AxesLabel -> {"x (m)", "y (m)"},
  GridLines -> Automatic,
  ImageSize -> Large,
  PlotLabel -> "Ballistic trajectories"
];

Export["ballistics.png", image, "PNG"]
```

И запустим:
```bash
wolframscript -f ballistics.wls
```

## Подключаем к программе на C

Основная инструкция: https://reference.wolfram.com/language/tutorial/WSTPDeveloperGuide-CMake.html

Сначала найдём заголовочный файл, который упоминается буквально на всех углах:
```bash
find /opt/Wolfram -type f -name wstp.h -print
```

Нас точно интересует `/opt/Wolfram/WolframEngine/13.3/SystemFiles/Links/WSTP/DeveloperKit/Linux-ARM64/`

Начнём с `/opt/Wolfram/WolframEngine/13.3/SystemFiles/Links/WSTP/DeveloperKit/Linux-ARM64/CompilerAdditions`

> [!WARNING]
> Мы ни разу не обсуждали CMake всерьёз.
> Это нехорошо. Попробуем осваивать с колёс. Так бывает.

Основная идея CMake — декларативность: «Опишите, что вам нужно, а не как этого достичь».

Напишем программу-пустышку (нужна, иначе CMake встанет):
```c
#include <stdio.h>
int main(void) {
    return 0;
}
```

Настроим сборку проекта в каталоге build:
```cmake
# Минимальная версия CMake
cmake_minimum_required(VERSION 3.20)

# Метаданные проекта
project(
	wstp
	LANGUAGES C)

# На входе нужно знать, где лежит WSTP SDK
if(NOT WSTP_ROOT_DIR)
    message(FATAL_ERROR "Нет обязательной переменной WSTP_ROOT_DIR")
endif()

# Подключаем настройки WSTP
include("${WSTP_ROOT_DIR}/WSTP.cmake")

# Что и из чего компилируем
add_executable(wstp.run main.c)

# Какие библиотеки подключаем
target_link_libraries(
	wstp.run
    PRIVATE
    WSTP::STATIC_LIBRARY
    stdc++
)
```

Скажем CMake создать скрипт сборки и положить его в build:
```bash
cmake \
  -S . \
  -B build \
  -DWSTP_ROOT_DIR=/opt/Wolfram/WolframEngine/13.3/SystemFiles/Links/WSTP/DeveloperKit/Linux-ARM64/CompilerAdditions
```

Теперь инициируем ту систему сборки, что лежит в build:
```bash
cmake --build build
```

Можно выполнять:
```bash
./build/wstp.run
```

### Простые расчёты
Начнём с великого достижения цивилизации: `2 • 2 = 4`
```c
#include <stdio.h>
#include <stdlib.h>
#include <wstp.h>

#define WOLFRAM_KERNEL "/opt/Wolfram/WolframEngine/13.3/Executables/WolframKernel"

int main(void) {
    WSENV environment;
    WSLINK link;
    int error;
    int packet;
    int result;

    char *arguments[] = {
        "wstp",
        "-linkmode",
        "launch",
        "-linkname",
        WOLFRAM_KERNEL " -wstp"
    };

    int argument_count =
        sizeof(arguments) / sizeof(arguments[0]);

    environment = WSInitialize(NULL);

    if (environment == NULL) {
        fprintf(stderr, "Не удалось инициализировать WSTP\n");
        return EXIT_FAILURE;
    }

    link = WSOpenArgcArgv(
        environment,
        argument_count,
        arguments,
        &error
    );

    if (link == NULL) {
        fprintf(stderr,
                "Не удалось создать WSTP-соединение: ошибка %d\n",
                error);

        WSDeinitialize(environment);
        return EXIT_FAILURE;
    }

    if (!WSActivate(link)) {
        fprintf(stderr,
                "Не удалось активировать WSTP-соединение: %s\n",
                WSErrorMessage(link));

        WSClose(link);
        WSDeinitialize(environment);
        return EXIT_FAILURE;
    }

    /*
     * Отправляем выражение:
     *
     *     EvaluatePacket[Plus[2, 2]]
     */

    WSPutFunction(link, "EvaluatePacket", 1);
    WSPutFunction(link, "ToExpression", 1);
    WSPutString(link, "2+2");
    WSEndPacket(link);

// Это для случаев динамической компоновки выражения
#if 0
    WSPutFunction(link, "EvaluatePacket", 1);
    WSPutFunction(link, "Plus", 2);
    WSPutInteger32(link, 2);
    WSPutInteger32(link, 2);
    WSEndPacket(link);
#endif

    /*
     * Ядро может прислать служебные пакеты.
     * Пропускаем их, пока не получим ReturnPacket.
     */

    while ((packet = WSNextPacket(link)) != RETURNPKT) {
        if (packet == 0) {
            fprintf(stderr,
                    "Ошибка при чтении ответа: %s\n",
                    WSErrorMessage(link));

            WSClose(link);
            WSDeinitialize(environment);
            return EXIT_FAILURE;
        }

        WSNewPacket(link);
    }

    if (!WSGetInteger32(link, &result)) {
        fprintf(stderr,
                "Ответ не является целым числом: %s\n",
                WSErrorMessage(link));

        WSClose(link);
        WSDeinitialize(environment);
        return EXIT_FAILURE;
    }

    printf("Результат: %d\n", result);

    WSClose(link);
    WSDeinitialize(environment);

    return EXIT_SUCCESS;
}
```

Конечно, если данные динамические, то прямая сборка — это очень плохая идея.
Прямо провоцирует на что-то вроде инъекции кода.

### Математика отдельно, код отдельно
Но можно выгрузить функцию во внешний .wl-файл и вызывать её с очищенными аргументами.
Для этого воспользуемся Wolfram-функцией Get:
```bash
wolframscript -code '
Get["./ballistics.wl"];
maxHeightAndAngle[50, 100]
'
```

Заработает, как только обзаведёмся файлом ballistics.wl с функцией maxHeigthAtDistance:
```wolfram
ClearAll[maxHeightAtDistance];

maxHeightAtDistance[v0_?NumericQ, l_?NumericQ, g_: 9.81] :=
    If[
        v0 <= 0 || l < 0 || g <= 0,
        Indeterminate,
        v0^2/(2 g) - g l^2/(2 v0^2)
    ];
```

На самом деле мы ведь хотим большего:
```wolfram
ClearAll[maxHeightAndAngle];

maxHeightAndAngle[v0_?NumericQ, l_?NumericQ, g_: 9.81] :=
    Module[
        {height, angle},

        If[v0 <= 0 || l < 0 || g <= 0,
            Return[$Failed]
        ];

        height =
            v0^2/(2 g) -
            g l^2/(2 v0^2);

        angle =
            If[
                l == 0,
                Pi/2,
                ArcTan[v0^2/(g l)]
            ];

        N[{height, angle}]
    ];
```

Ну и немаленький такой код для использования этой функции:
```c
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <wstp.h>

#define WOLFRAM_KERNEL "/opt/Wolfram/WolframEngine/13.3/Executables/WolframKernel"

static void close_wstp(WSLINK link, WSENV environment) {
    if (link != NULL) {
        WSClose(link);
    }

    if (environment != NULL) {
        WSDeinitialize(environment);
    }
}

static void print_wstp_error(WSLINK link, const char *context) {
    fprintf(
        stderr,
        "%s: WSTP error %d: %s\n",
        context,
        WSError(link),
        WSErrorMessage(link)
    );
}

/*
 * Пропускает служебные пакеты и останавливается
 * на содержимом ReturnPacket.
 */
static int wait_for_return_packet(WSLINK link) {
    int packet;

    for (;;) {
        packet = WSNextPacket(link);

        if (packet == 0) {
            return 0;
        }

        if (packet == RETURNPKT) {
            return 1;
        }

        if (!WSNewPacket(link)) {
            return 0;
        }
    }
}

static int load_wolfram_file(
    WSLINK link,
    const char *filename
) {
    /*
     * Отправляем:
     *
     * EvaluatePacket[
     *     Get["/полный/путь/ballistics.wl"]
     * ]
     */
    if (!WSPutFunction(link, "EvaluatePacket", 1) ||
        !WSPutFunction(link, "Get", 1) ||
        !WSPutString(link, filename) ||
        !WSEndPacket(link)) {

        return 0;
    }

    if (!wait_for_return_packet(link)) {
        return 0;
    }

    /*
     * Get возвращает Null.
     * Содержимое ReturnPacket нам не нужно.
     */
    return WSNewPacket(link);
}

static int call_max_height_and_angle(
    WSLINK link,
    double velocity,
    double distance,
    double *height,
    double *angle
) {
    const char *head = NULL;
    int argument_count = 0;

    /*
     * Отправляем:
     *
     * EvaluatePacket[
     *     maxHeightAndAngle[velocity, distance]
     * ]
     */
    if (!WSPutFunction(link, "EvaluatePacket", 1) ||
        !WSPutFunction(link, "maxHeightAndAngle", 2) ||
        !WSPutReal64(link, velocity) ||
        !WSPutReal64(link, distance) ||
        !WSEndPacket(link)) {

        return 0;
    }

    if (!wait_for_return_packet(link)) {
        return 0;
    }

    /*
     * Ожидаемый ответ:
     *
     * List[height, angle]
     *
     * То есть в обычной записи:
     *
     * {height, angle}
     */
    if (!WSGetFunction(link, &head, &argument_count)) {
        return 0;
    }

    if (strcmp(head, "List") != 0 || argument_count != 2) {
        fprintf(
            stderr,
            "Unexpected result: %s with %d arguments\n",
            head,
            argument_count
        );

        WSReleaseSymbol(link, head);
        return 0;
    }

    WSReleaseSymbol(link, head);

    if (!WSGetReal64(link, height) ||
        !WSGetReal64(link, angle)) {

        return 0;
    }

    return 1;
}

static int parse_nonnegative_double(
    const char *text,
    const char *name,
    double *value
) {
    char *end = NULL;

    errno = 0;
    *value = strtod(text, &end);

    if (errno != 0 ||
        end == text ||
        *end != '\0') {

        fprintf(stderr, "Invalid %s: %s\n", name, text);
        return 0;
    }

    if (*value < 0.0) {
        fprintf(stderr, "%s must not be negative\n", name);
        return 0;
    }

    return 1;
}

int main(int argc, char **argv) {
    WSENV environment = NULL;
    WSLINK link = NULL;

    int error = WSEOK;

    double velocity;
    double distance;
    double height;
    double angle;

    char wolfram_file[PATH_MAX];

    char *link_arguments[] = {
        "wstp",
        "-linkmode",
        "launch",
        "-linkname",
        WOLFRAM_KERNEL " -wstp"
    };

    int link_argument_count =
        (int)(sizeof(link_arguments) /
              sizeof(link_arguments[0]));

    if (argc != 3) {
        fprintf(
            stderr,
            "Usage: %s VELOCITY DISTANCE\n"
            "Example: %s 50 100\n",
            argv[0],
            argv[0]
        );

        return EXIT_FAILURE;
    }

    if (!parse_nonnegative_double(
            argv[1],
            "velocity",
            &velocity)) {

        return EXIT_FAILURE;
    }

    if (velocity == 0.0) {
        fprintf(stderr, "Velocity must be greater than zero\n");
        return EXIT_FAILURE;
    }

    if (!parse_nonnegative_double(
            argv[2],
            "distance",
            &distance)) {

        return EXIT_FAILURE;
    }

    /*
     * Получаем абсолютный путь к ballistics.wl.
     * Файл должен находиться в текущем каталоге.
     */
    if (realpath("ballistics.wl", wolfram_file) == NULL) {
        perror("realpath ballistics.wl");
        return EXIT_FAILURE;
    }

    environment = WSInitialize(NULL);

    if (environment == NULL) {
        fprintf(stderr, "WSInitialize failed\n");
        return EXIT_FAILURE;
    }

    link = WSOpenArgcArgv(
        environment,
        link_argument_count,
        link_arguments,
        &error
    );

    if (link == NULL || error != WSEOK) {
        fprintf(
            stderr,
            "WSOpenArgcArgv failed: WSTP error %d\n",
            error
        );

        close_wstp(link, environment);
        return EXIT_FAILURE;
    }

    if (!WSActivate(link)) {
        print_wstp_error(link, "WSActivate");
        close_wstp(link, environment);
        return EXIT_FAILURE;
    }

    if (!load_wolfram_file(link, wolfram_file)) {
        print_wstp_error(link, "Loading ballistics.wl");
        close_wstp(link, environment);
        return EXIT_FAILURE;
    }

    if (!call_max_height_and_angle(
            link,
            velocity,
            distance,
            &height,
            &angle)) {

        print_wstp_error(
            link,
            "Calling maxHeightAndAngle"
        );

        close_wstp(link, environment);
        return EXIT_FAILURE;
    }

    printf("Velocity:       %.6f m/s\n", velocity);
    printf("Distance:       %.6f m\n", distance);
    printf("Maximum height: %.6f m\n", height);
    printf("Launch angle:   %.12f rad\n", angle);

    close_wstp(link, environment);

    return EXIT_SUCCESS;
}
```

Ой, да мы же хотим в градусах!
Меняем:
```wolfram
angle =
    If[
        l == 0,
        90,
        N[ArcTan[v0^2/(g l)]/Degree]
    ];
```

Ну и в выводе ° вместо rad. И всё.

> [!TIP]
> Да мы же только что отделили математику от программирования!
> Ну ничего себе.
