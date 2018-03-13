# Intaractive Graphic Systems
Laboratory works OpenGL

## Установка библиотек

### Установка системных библиотек:
```shell
apt install libgl1-mesa-dev libxrande-dev libxinerama-dev libxcursor-dev libxi-dev
```

### Установка дополнителльных библиотек:
1. скачать glad, перенести include файлы в /usr/include и включить glad.c в проект;
2. скачать исходники glfw, скомпилить и установить;
3. скачать исходники glm, скомпилить и установить.

## Флаги компиляции
```shell
-lGL -lXinerama -lXcursor -lX11 -lXxf86vm -lXrandr -lglfw -pthread -lXi -ldl -lrt -lm -std=c++11
```
Рекомендую работать с проектами в CLion.