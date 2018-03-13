# Intaractive Graphic Systems
Laboratory works OpenGL

## Установка библиотек

### Установка системных библиотек:
```bash
apt install libgl1-mesa-dev libxrande-dev libxinerama-dev libxcursor-dev libxi-dev
```

### Установка дополнителльных библиотек:
1. скачать [glad](http://glad.dav1d.de/), перенести include файлы в /usr/include и включить glad.c в проект;
2. скачать исходники [glfw](http://www.glfw.org/download.html), скомпилить и установить;
3. скачать исходники [glm](https://github.com/g-truc/glm/releases/tag/0.9.8.5), скомпилить и установить.

## Флаги компиляции
```bash
-lGL -lXinerama -lXcursor -lX11 -lXxf86vm -lXrandr -lglfw -pthread -lXi -ldl -lrt -lm -std=c++11
```
Рекомендую работать с проектами в [CLion](https://www.jetbrains.com/clion/download/).
