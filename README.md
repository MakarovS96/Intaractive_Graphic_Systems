# Intaractive Graphic Systems
Laboratory works OpenGL

## Установка библиотек

### Установка системных библиотек:
```bash
apt install libgl1-mesa-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

### Установка дополнителльных библиотек:
1. скачать [glad](http://glad.dav1d.de/), перенести include файлы в /usr/include и включить glad.c в проект;
2. скачать исходники [glfw](http://www.glfw.org/download.html), скомпилить и установить;
3. скачать исходники [glm](https://github.com/g-truc/glm/releases/tag/0.9.8.5), скомпилить и установить.
4. скачать [assimp](https://sourceforge.net/projects/assimp/files/assimp-3.1/), скомпилить и установить. Основные ошибки при сборке Assimp (актуально для Windows):
    - CMake выдает ошибки о отсутствующих библиотеках DirectX, такие как:
    ```
    Could not locate DirectX
    CMake Error at cmake-modules/FindPkgMacros.cmake:110 (message):
    Required library DirectX not found! Install the library (including dev packages)
    and try again. If the library is already installed, set the missing variables
    manually in cmake.
    ```
    Нужно установить DirectX SDK, в случае если он не был установлен. Вы можете скачать SDK [здесь](http://www.microsoft.com/en-us/download/details.aspx?id=6812) .

    - Во время установка DirectX SDK выскакивает код ошибки s1023.
    В этом случае вам сначала нужно установить пакет С++, до того как устанавливать SDK,
    как описано [здесь](http://blogs.msdn.com/b/chuckw/archive/2011/12/09/known-issue-directx-sdk-june-2010-setup-and-the-s1023-error.aspx).

    - После завершения настройки можно создать файл проекта, открыть его и скомпилировать библиотеки.

    - По умолчанию, Assimp предоставляется в виде динамической библиотеки, поэтому нам нужно включить в проект соответствующую dll с именем assimp.DLL. Можно просто файл библиотеку DLL в ту же папку, где находится исполняемый файл приложения.

    - После компиляции, библиотека и её dll файл будут находится в папках code/Debug или code/Release.

    - Далее просто свяжите файл библиотеки и dll файл с вашим проектом, и убедитесь, что вы не забыли также подключить заголовочные файлы Assimp.


## Флаги компиляции
```bash
-lGL -lXinerama -lXcursor -lX11 -lXxf86vm -lXrandr -lglfw -pthread -lXi -ldl -lrt -lm -lassimp -std=c++11
```
Рекомендую работать с проектами в [CLion](https://www.jetbrains.com/clion/download/).
