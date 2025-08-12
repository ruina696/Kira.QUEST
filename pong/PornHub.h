#pragma once
#include "windows.h"
#include <vector>
#include <string>

using namespace std;


// секция данных игры  
typedef struct {
    float x, y, width, height, rad, dx, dy, speed;
    HBITMAP hBitmap;//хэндл к спрайту шарика 
} sprite;

enum class itemID {
    axe, hemlet, sword
};

struct item_ {
    string name;
    sprite Sprite;
};

vector<item_> itemLib;

struct player_ {
    sprite hero_sprite;
    int life;
    int current_location = 0;
    vector <item_> player_items;
    HBITMAP hBitmapRight;
    HBITMAP hBitmapLeft;
};

player_ player;

struct ball_ {
    sprite Sprite;
};

ball_ ball;

auto Load(LPCSTR name) {
    return (HBITMAP)LoadImageA(NULL, name, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

class Platform
{
public:
    sprite pl_sprite;

    Platform(int plat_x, int plat_y, int plat_height, int plat_width)
    {
        pl_sprite.x = plat_x;
        pl_sprite.y = plat_y;
        pl_sprite.height = plat_height;
        pl_sprite.width = plat_width;
        pl_sprite.hBitmap = Load("platform.bmp");
    }
    //~Platform() {
    //    DeleteObject(pl_sprite.hBitmap);
    //}
};



class Character {
public:
    sprite Sprite;
    int life;
    HBITMAP Sprite_Right;
    HBITMAP Sprite_Left;
    int a = 0;
    float Startpos;
    float Endpos;
    bool Moving;

    Character(int x, int y, float endpos, int sp)
    {
        Sprite.x = x;
        Sprite.y = y;
        Startpos = x;
        Endpos = endpos;
        Sprite.height = 100;
        Sprite.width = 100;
        Sprite.speed = sp;
        Sprite.hBitmap = Load("enemy_right.bmp");
    }

    virtual void Move() {
        a = 7;
        //if (Sprite.hBitmap) {
        //    DeleteObject(Sprite.hBitmap);
        //}

        if (Moving) {
            Sprite.x += Sprite.speed;
            //Sprite.hBitmap = Load("enemy_right.bmp");

            if (Sprite.x >= Endpos) {
                Moving = false;
            }
        }
        else {
            Sprite.x -= Sprite.speed;
            //Sprite.hBitmap = Load("enemy_left.bmp");

            if (Sprite.x <= Startpos) {
                Moving = true;
            }
        }
    }
    ~Character() {}
};

//class Hero : Character {
//public:
//    int current_location;
//    vector<item_> player_items;
//
//    Hero() 
//    {
//
//    }
//
//    void Move() {
//
//    }
//};

class Wolf : public Character {
public:
    int b = 0;

    Wolf(int x, int y, float endpos, int sp, int height, int width) : Character(x, y, endpos, sp)
    {
        Sprite.x = x;
        Sprite.y = y;
        Startpos = x;
        Endpos = endpos;
        Sprite.speed = sp;
        //Sprite.hBitmap = Load("enemy_right.bmp");
        Sprite.height = height;
        Sprite.width = width;
    }
    void Move() override {
        b = 5;
        /* if (Sprite.hBitmap) {
             DeleteObject(Sprite.hBitmap);
         }*/

        if (Moving) {
            Sprite.x += Sprite.speed;
            //Sprite.hBitmap = Load("enemy_right.bmp");

            if (Sprite.x >= Endpos) {
                Moving = false;
            }
        }
        else {
            Sprite.x -= Sprite.speed;
            //Sprite.hBitmap = Load("enemy_left.bmp");

            if (Sprite.x <= Startpos) {
                Moving = true;
            }
        }
    }
};

class Hare : public Character {
public:
    Hare(int x, int y, float endpos, int sp, int height, int width) : Character(x, y, endpos, sp)
    {
        Sprite.x = x;
        Sprite.y = y;
        Startpos = x;
        Endpos = endpos;
        Sprite.speed = sp;
        //Sprite.hBitmap = Load("enemy_right.bmp");
        Sprite.height = height;
        Sprite.width = width;
    }
    void Move() override {
        a = 3;
        //if (Sprite.hBitmap) {
        //    DeleteObject(Sprite.hBitmap);
        //}

        if (Moving) {
            Sprite.x += Sprite.speed;
            //Sprite.hBitmap = Load("enemy_right.bmp");

            if (Sprite.x >= Endpos) {
                Moving = false;
            }
        }
        else {
            Sprite.x -= Sprite.speed;
            //Sprite.hBitmap = Load("enemy_left.bmp");

            if (Sprite.x <= Startpos) {
                Moving = true;
            }
        }
    }
};
//vector<Character*> chars;

class Enemy {
public:
    int life = 5;
    float startPos;
    float endPos;
    bool Moving;
    sprite en_sprite;


    Enemy(int en_x, int en_y, int EndPos, int sp) {

        en_sprite.x = en_x;
        en_sprite.y = en_y;
        startPos = en_x;
        endPos = EndPos;
        en_sprite.speed = sp;
        en_sprite.height = 100;
        en_sprite.width = 100;
        en_sprite.hBitmap = Load("enemy_right.bmp");
    }

    ~Enemy() {
        DeleteObject(en_sprite.hBitmap);
    }

    void EnemyMove() {
        if (en_sprite.hBitmap) {
            DeleteObject(en_sprite.hBitmap);
        }

        if (Moving) {
            en_sprite.x += en_sprite.speed;
            en_sprite.hBitmap = Load("enemy_right.bmp");

            if (en_sprite.x >= endPos) {
                Moving = false;
            }
        }
        else {
            en_sprite.x -= en_sprite.speed;
            en_sprite.hBitmap = Load("enemy_left.bmp");

            if (en_sprite.x <= startPos) {
                Moving = true;
            }
        }
    }
    void EnemyCollusion() {
        auto& pl_s = player.hero_sprite;

        if (pl_s.y <= en_sprite.y + en_sprite.height &&
            pl_s.y + pl_s.height >= en_sprite.y &&
            pl_s.x <= en_sprite.x + en_sprite.width &&
            pl_s.x + pl_s.width >= en_sprite.x)
        {
            int UP = abs(en_sprite.y - (pl_s.y + pl_s.height));
            int DOWN = abs((en_sprite.y + en_sprite.height) - pl_s.y);
            int over_y = min(UP, DOWN);

            int LEFT = abs(en_sprite.x - (pl_s.x + pl_s.width));
            int RIGHT = abs((en_sprite.x + en_sprite.width) - pl_s.x);
            int over_x = min(LEFT, RIGHT);

            if (over_x < over_y) {
                if (LEFT < RIGHT) {
                    pl_s.x = en_sprite.x - pl_s.width;
                    pl_s.x -= 100;
                }
                else {
                    pl_s.x = en_sprite.x + en_sprite.width;
                    pl_s.x += 100;
                }
            }
            else {
                if (UP < DOWN) {
                    pl_s.y = en_sprite.y - pl_s.height;
                    pl_s.x += 200;
                }
                else {
                    pl_s.y = en_sprite.y + en_sprite.height;
                }
            }
            player.life -= 1;
            pl_s.y -= 70;
        }
    }
};


struct location_ {
    HBITMAP hBitmap;
    int left_portal;
    int right_portal;
    vector<Platform> plats;
    vector <item_> items;
    vector <Enemy> enemies;
    vector <Character*> chars;
};

location_ loc[5];

struct {
    int score, balls;//количество набранных очков и оставшихся "жизней"
    bool action = false;//состояние - ожидание (игрок должен нажать пробел) или игра
} game;


struct {
    HWND hWnd;//хэндл окна
    HDC device_context, context;// два контекста устройства (для буферизации)
    int width, height;//сюда сохраним размеры окна которое создаст программа
} window;


HBITMAP hBack;// хэндл для фонового изображения
