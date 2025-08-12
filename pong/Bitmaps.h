#pragma once
#include "PornHub.h"


void ShowBitmap(HDC hDC, int x, int y, int x1, int y1, HBITMAP hBitmapBall, bool alpha = false)
{
    HBITMAP hbm, hOldbm;
    HDC hMemDC;
    BITMAP bm;

    hMemDC = CreateCompatibleDC(hDC); // Создаем контекст памяти, совместимый с контекстом отображения
    hOldbm = (HBITMAP)SelectObject(hMemDC, hBitmapBall);// Выбираем изображение bitmap в контекст памяти

    if (hOldbm) // Если не было ошибок, продолжаем работу
    {
        GetObject(hBitmapBall, sizeof(BITMAP), (LPSTR)&bm); // Определяем размеры изображения

        if (alpha)
        {
            TransparentBlt(window.context, x, y, x1, y1, hMemDC, 0, 0, x1, y1, RGB(0, 0, 0));//все пиксели черного цвета будут интепретированы как прозрачные
        }
        else
        {
            StretchBlt(hDC, x, y, x1, y1, hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY); // Рисуем изображение bitmap
        }

        SelectObject(hMemDC, hOldbm);// Восстанавливаем контекст памяти
    }

    DeleteDC(hMemDC); // Удаляем контекст памяти
}




void ShowSprites()
{
    ShowBitmap(window.context, 0, 0, window.width, window.height, loc[player.current_location].hBitmap);//задний фон

    for (int i = 0; i < loc[player.current_location].items.size(); i++) {
        auto item = loc[player.current_location].items[i].Sprite;
        ShowBitmap(window.context, item.x, item.y, item.width, item.height, item.hBitmap);//предметы на карте

        if (player.hero_sprite.x + player.hero_sprite.width >= item.x && player.hero_sprite.x <= item.x + item.width &&
            player.hero_sprite.y + player.hero_sprite.height >= item.y) {

            player.player_items.push_back(loc[player.current_location].items[i]);
            loc[player.current_location].items.erase(loc[player.current_location].items.begin() + i);
        }
    }

    if (!player.player_items.empty()) {
        for (int i = 0; i < player.player_items.size(); i++) {
            ShowBitmap(window.context, 200 + i * 120, 100, player.player_items[i].Sprite.width, player.player_items[i].Sprite.height, player.player_items[i].Sprite.hBitmap);//предметы в инвентаре
        }
    }

    ShowBitmap(window.context, player.hero_sprite.x, player.hero_sprite.y, player.hero_sprite.width, player.hero_sprite.height, player.hero_sprite.hBitmap);// ракетка игрока

    for (int i = 0; i < loc[player.current_location].plats.size(); i++)
    {
        auto platform = loc[player.current_location].plats[i].pl_sprite;
        ShowBitmap(window.context, platform.x, platform.y, platform.width, platform.height, platform.hBitmap);//платформы
    }
    /*  for (int i = 0; i < loc[player.current_location].enemies.size(); i++) {
          auto enemy = loc[player.current_location].enemies[i].en_sprite;
          ShowBitmap(window.context, enemy.x, enemy.y, enemy.width, enemy.height, enemy.hBitmap);
      }*/

    for (int i = 0; i < loc[player.current_location].chars.size(); i++) {
        auto charact = loc[player.current_location].chars[i]->Sprite;
        if (!charact.hBitmap) {
            MessageBox(window.hWnd, "ПУСТО", "PORNO", MB_OK);
        }
        else {
            ShowBitmap(window.context, charact.x, charact.y,
                charact.width, charact.height,
                charact.hBitmap);
        }
    }
    //ShowBitmap(window.context, sam.Sprite.x, sam.Sprite.y, sam.Sprite.width, sam.Sprite.height, sam.Sprite.hBitmap);

}

