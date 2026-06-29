# Nautilus hyprpaper extension
Это расширение для файлового менеджера Nautilus, или более известного как
утилита Files в графической оболочке Gnome. Данное расширение добавляет в
контекстное меню пункт "Установить как обои Hyprland" для файлов изображений.

Выбраное изображение сохраняется в `$HOME/.local/state/hyprland/wallpaper`,
для того чтобы иметь возможность восстановить его при перезапуске Hyprland, для
этого можно добавить в `.config/hypr/hyprland.lua` запуск такого сценария

```bash
#!/bin/sh

hyprpaper &

WALL=$(cat "$HOME/.local/state/hyprland/wallpaper")

until hyprctl hyprpaper wallpaper ",$WALL" >/dev/null 2>&1; do
    sleep 0.1
done
```

## Установка

```bash
git clone https://github.com/vakcinio/Nautilus-hyprpaper-extension.git
cd Nautilus-hyprpaper-extension
meson setup build
sudo ninja -C build install
```