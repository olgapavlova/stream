
Youtube-запись от `2026-07-24`: https://youtu.be/2G9bti8JMWc

# Кодим на `C`…<br>На телефоне?<br>На `Ubuntu`?<br>В `vim`?<br>Ну… да

> [!TIP]
> Давайте покодим на телефоне.\
> И не сломаем его.

Для этого нам пригодятся контейнеры.

Да-да, те самые, которые «Docker — это не виртуальная машина, а контейнер, сколько можно повторять!»

## Всё становится лучше с ~~Bluetooth~~ SSH

Настраиваем SSH на телефоне.
Да, там есть терминал. Прямо в нём.

> [!WARNING] Важно установить пароль на телефон, иначе не заработает вообще никак.

Для начала запускаем SSH-сервер:
```bash
sudo systemctl start ssh
sudo systemctl status ssh
```

Можно попытаться включить на автозапуск, но, скорее всего, не выйдет:
```bash
sudo systemctl enable ssh
```

Потом выясняем IP:
```bash
hostname -I
```

Заходим с компьютера:
```bash
ssh phablet@[IP вашего телефона]
```


> [!TIP]
> Ура, можно больше не ломать глаза на маленьком экране.\
> Но можно и ломать.\
> Особенно если у вас есть внешняя клавиатура.

Предупреждение о рисках в далёком будущем:
```text
** WARNING: connection is not using a post-quantum key exchange algorithm.
** This session may be vulnerable to "store now, decrypt later" attacks.
** The server may need to be upgraded. See https://openssh.com/pq.html
```

Почему? Потому что разные версии OpenSSH на компьютере и телефоне. Узнать:
```bash
ssh -V
```

Можно ничего не делать, будущее ещё очень далеко.

## Контейнеры — объект нашего интереса

Теперь мы внутри и можем копаться в контейнерах.

Создать контейнер:
```bash
libertine-container-manager create \
    -i dev \
    -n dev \
    -d focal \
    -t chroot
```

Почему так?
- если делать через GUI, то система сама назначит ID контейнера — сюрприз!
- нам же разобраться, а не просто пользоваться
- увидим, как оно всё потихонечку встаёт (полчаса минимум)

Установили? Проверим:
```bash
libertine-container-manager list
```

Заходим в контейнер:
```bash
libertine-container-manager \
	exec -i dev \
	-c /bin/bash
```

Там будет неприятная ругань.
Выходим из контейнера, редактируем файл контейнерных настроек.
```bash
exit
nano ~/.cache/libertine-container/dev/rootfs/etc/nsswitch.conf
```

- Да, не vim, откуда у нас тут vim.

В начале файла должно быть так:
```text
passwd:         files systemd extrausers
group:          files systemd extrausers
shadow:         files extrausers
gshadow:        files extrausers
…
```
Новенькое — `extrausers` — убирает ругань.

Входим как root:
```bash
libertine-container-manager exec -i dev -c /bin/fakeroot
```

Смотрим на изобилие пакетов:
```bash
dpkg -l
```

Теперь можем ставить в контейнер пакеты какие захотим, телефон от наших экспериментов не сломается.

## Готовимся программировать прямо тут

Устанавливаем пакеты для разработки:
```bash
apt install \
    build-essential \
    git \
    gdb \
    vim \
    tmux \
    strace \
    binutils \
    pkg-config -y
```

Посмотрим, какие уже стоят:
```bash
dpkg -l
```

Выходим и снова входим, чтобы просто работать:
```bash
exit
libertine-launch -i dev /bin/bash
```

Подозрительно похоже на вход в Docker-контейнер, правда?

### Да что за контейнеры-то?

> [!IMPORTANT]
> Ядро — одно.\
> Пользовательские пространства — разные

- Пользователь — это не пользователь.
- **А кто?!**

`user space` — это режим процессора.
Противоположность `kernel space`.
Место для «обычных программ», а не место для человека.

`Docker` — это прям контейнеры-контейнеры.
`Libertine` попроще: контейнер == файловая система + среда для пакетов.

Но для изучения — хватит с головой.


## Давайте наконец что-нибудь напишем

Простой код заработает запросто:
```c
#include <stdio.h>
int main(void) {
	printf("Hello, phone!\n");
	return 0;
}
```

Компилируем и запускаем как обычно:
```bash
gcc hello.c -o hello
./hello
```

Покопаемся в картиночках:
```bash
ls /home/phablet/Pictures/camera.ubports/
```

Установим [SDL2](https://www.libsdl.org) — *но это на будущее*:
```bash
apt install \
    libsdl2-dev \
    libsdl2-image-dev -y
```

