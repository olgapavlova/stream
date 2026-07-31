# Минимальная версия CMake
cmake_minimum_required(VERSION 3.20)

# Метаданные проекта
project(
	wstp
	LANGUAGES C
)

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
