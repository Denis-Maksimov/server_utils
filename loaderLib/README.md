# loaderLib
модуль для загрузки в программу зашифрованной библиотеки.

## Описание
существование таких инструментов как ghidra делает уязвимым
код для анализа и изменения злоумышленниками. Для того, чтобы усложнить им жизнь, можно попробовать зашифровать используемые модули (библиотеки so, dll).

## состав библиотеки
shared - proxy-обёртка над dlopen для linux и LoadLibrary для Windows
dem.cpp (demangle) - попытка делать деманглинг c++ имён
pass.cpp - crypt/encrypt RSA-функции

## Примеры
``` c++
#include "shared.hpp"

typedef int (*add_t)(int ,int );

int main() {
    library lib("./mylib.so");
    auto fu=(add_t)lib.get_function("add");
    std::cout << fu(2,8)<<std::endl;
    return 0;
}

```
## status

в разработке ...

