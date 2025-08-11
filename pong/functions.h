#pragma once
#include "PornHub.h" 



int gravity = 15;
int jump = 0;
bool isJumping = false;

void ProcessInput()
{
    if (GetAsyncKeyState('A')) {
        player.hero_sprite.hBitmap = player.hBitmapLeft;
        player.hero_sprite.x -= player.hero_sprite.speed;
    }
    if (GetAsyncKeyState('D')) {
        player.hero_sprite.hBitmap = player.hBitmapRight;
        player.hero_sprite.x += player.hero_sprite.speed;
    }
    auto pl_s = player.hero_sprite;


    if (GetAsyncKeyState('W') && !isJumping) {
        jump = 70;
        isJumping = true;
    }

    if ((pl_s.y == window.height - pl_s.height)) {
        isJumping = false;
    }

    player.hero_sprite.y += gravity - jump;
    player.hero_sprite.y = min(window.height - player.hero_sprite.height, player.hero_sprite.y);
    jump *= 0.8;
}

void ClearVectors() //сначала очищаем битмапки, потом сами векторы
{
    for (int i = 0; i <= 2; i++) {
        for (auto& enemy : loc[i].enemies) {
            if (enemy.en_sprite.hBitmap) {
                DeleteObject(enemy.en_sprite.hBitmap);
            }
        }
        loc[i].enemies.clear();

        for (auto& plat : loc[i].plats) {
            if (plat.pl_sprite.hBitmap) {
                DeleteObject(plat.pl_sprite.hBitmap);
            }
        }
        loc[i].plats.clear();

        for (auto& item : loc[i].items) {
            if (item.Sprite.hBitmap) {
                DeleteObject(item.Sprite.hBitmap);
            }
        }
        loc[i].items.clear();
    }

    for (auto& item : player.player_items)
    {
        if (item.Sprite.hBitmap) {
            DeleteObject(item.Sprite.hBitmap);
        }
    }
    player.player_items.clear();

    for (auto& item : itemLib)
    {
        if (item.Sprite.hBitmap) {
            DeleteObject(item.Sprite.hBitmap);
        }
    }
    itemLib.clear();

}



void Collusion()
{
    auto pl_s = player.hero_sprite;

    for (int i = 0; i < loc[player.current_location].plats.size(); i++)
    {
        auto platform = loc[player.current_location].plats[i].pl_sprite;

        if (pl_s.y <= platform.y + platform.height &&
            pl_s.y + pl_s.height >= platform.y &&
            pl_s.x <= platform.x + platform.width &&
            pl_s.x + pl_s.width >= platform.x)
        {

            int UP = abs(platform.y - (pl_s.y + pl_s.height));
            int DOWN = abs((platform.y + platform.height) - pl_s.y);
            int over_Y = min(UP, DOWN);

            int LEFT = abs(platform.x - (pl_s.x + pl_s.width));
            int RIGHT = abs(platform.x + platform.width - pl_s.x);
            int over_X = min(LEFT, RIGHT);

            if (over_X < over_Y)
            {
                if (LEFT < RIGHT)
                {
                    player.hero_sprite.x = platform.x - pl_s.width;
                }
                else
                {
                    player.hero_sprite.x = platform.x + platform.width;
                }
            }
            else {

                if (UP < DOWN)
                {
                    player.hero_sprite.y = platform.y - player.hero_sprite.height;
                    isJumping = false;
                }
                else
                {
                    player.hero_sprite.y = platform.y + platform.height;
                }
            }
        }
    }
}

void Trace() {
    int trx_st = player.hero_sprite.x;
    int try_st = player.hero_sprite.y;
    int trx_end = player.hero_sprite.x;
    int try_end = player.hero_sprite.y;
    float lenth;
    //SetPixel(window.context, trx_st, try_st, 0x000000FF);
    if (GetAsyncKeyState('D')) {
        trx_end = trx_st + player.hero_sprite.speed;
        lenth = pow((pow((trx_end - trx_st), 2) + pow((try_end - try_st), 2)), 0.5);

        /*for (int i = 0; i <= loc[player.current_location].plats.size(); i++) {

        }*/

        for (float i = 0; i < lenth; i++) {
            float newX = trx_st + (trx_st + player.hero_sprite.speed) * (i / lenth);
            SetPixel(window.context, newX, player.hero_sprite.y, 0x000000FF);

        }
        //if ()
    }
    if (GetAsyncKeyState('A')) {
        trx_end = trx_st - player.hero_sprite.speed;

    }
    //SetPixel(window.context, trx_st + player.hero_sprite.speed, try_st, 0x000000FF);
}



void LimitHero()
{
    if (player.hero_sprite.x <= 0) {
        player.current_location = loc[player.current_location].left_portal;
        player.hero_sprite.x = window.width - player.hero_sprite.width;
    }
    if (player.hero_sprite.x >= window.width) {
        player.current_location = loc[player.current_location].right_portal;
        player.hero_sprite.x = 0;
    }
}