//linker::system::subsystem  - Windows(/ SUBSYSTEM:WINDOWS)
//configuration::advanced::character set - not set
//linker::input::additional dependensies Msimg32.lib; Winmm.lib


#include "windows.h"
#include <vector>
#include <string>
#include <time.h>
#include <math.h>

using namespace std;

DWORD currentTime = timeGetTime();

// секция данных игры  
typedef struct {
    float x, y, width, height, rad, dx, dy, speed;
    HBITMAP hBitmap;//хэндл к спрайту шарика 
} sprite;

enum class itemID {
    axe,hemlet,sword
};

struct item_ {
    string name;
    sprite Sprite;
};

vector<item_> itemLib;

struct player_ {
    sprite hero_sprite;
    int life = 10;
    int current_location = 0;
    vector <item_> player_items;
};

player_ player;

struct platform_ {
    sprite plat_sprite;
};

auto Show(LPCSTR name) {
    return (HBITMAP)LoadImageA(NULL, name, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

class Platform
{
public:
    sprite pl_sprite;

    Platform( int plat_x, int plat_y, int plat_height, int plat_width )
    {
        pl_sprite.x = plat_x;
        pl_sprite.y = plat_y;
        pl_sprite.height = plat_height;
        pl_sprite.width = plat_width;
        pl_sprite.hBitmap = Show("platform.bmp");
    }
};

class Enemy {
public:
    int life = 5;
    float startPos;
    float endPos;
    sprite en_sprite;

    Enemy( int en_x, int en_y, int EndPos ) {

        en_sprite.x = en_x;
        en_sprite.y = en_y;
        startPos = en_x;
        endPos = EndPos;
        en_sprite.speed = 3;
        en_sprite.height = 100;
        en_sprite.width = 100;
        en_sprite.hBitmap = Show("enemy_right.bmp");
    }

    void EnemyMove();

};

//struct enemy_ {
//    sprite enemy_sprite;
//    int life = 5;
//    int currentloc = 0;
//    
//
//};
//enemy_ enemy;

struct location_ {
    HBITMAP hBitmap;
    int left_portal;
    int right_portal;
    vector<Platform> plats;
    vector <item_> items;
    vector <Enemy> enemies;
};

location_ loc[5];

struct {
    int score, balls;//количество набранных очков и оставшихся "жизней"
    bool action = false;//состояние - ожидание (игрок должен нажать пробел) или игра
} game;


//struct enemyWalck 
//{
//    float startPos;
//    float endPos;
//    float speed;
//    //float progress;
//    
//
//};
//
//enemyWalck Walck;


struct {
    HWND hWnd;//хэндл окна
    HDC device_context, context;// два контекста устройства (для буферизации)
    int width, height;//сюда сохраним размеры окна которое создаст программа
} window;

HBITMAP hBack;// хэндл для фонового изображения

//cекция кода

//void EnemyInfo(string name, int x, int y, LPCSTR en_bmp) 
//{
//    enemy_ i;
//    i.enemy_sprite.x = x;
//    i.enemy_sprite.y = y;
//    i.enemy_sprite.height = 100;
//    i.enemy_sprite.width = 100;
//    i.enemy_sprite.hBitmap = Show("enemy_left.bmp");
//}

void ItemInfo(string name, int x, int y, int height, int width, LPCSTR it_name)
{
    item_ i;
    i.name = name;
    i.Sprite.x = x;
    i.Sprite.y = y;
    i.Sprite.height = height;
    i.Sprite.width = width;
    i.Sprite.hBitmap = Show(it_name);
    itemLib.push_back(i);
}

void LocInfo(int numb, LPCSTR loc_name, int l_port, int r_port) 
{
    loc[numb].hBitmap = Show(loc_name);
    loc[numb].left_portal = l_port;
    loc[numb].right_portal = r_port;
}

void InitGame()
{
    ItemInfo("axe", window.width / 2 , window.height - 100, 100, 100, "axe.bmp");
    ItemInfo("hemlet", window.width / 3, window.height - 100, 100, 100, "hemlet.bmp");
    ItemInfo("sword", window.width / 7, window.height - 100, 100, 100, "sword.bmp");

    loc[0].items.push_back(itemLib[(int)itemID::axe]);
    loc[0].items.push_back(itemLib[(int)itemID::sword]);
    loc[2].items.push_back(itemLib[(int)itemID::hemlet]);

    loc[0].plats.emplace_back(window.width / 1.9, window.height - 200, 60, 600);
    //loc[0].plats.emplace_back(window.width / 6, window.height - 60, 60, 600);
    loc[0].plats.emplace_back(window.width / 6, window.height - 350, 60, 600);
    loc[1].plats.emplace_back(0, window.height - 200, 60, 900);
    loc[1].plats.emplace_back(window.width / 1.7, window.height - 200, 60, window.width - window.width / 1.7);
    loc[2].plats.emplace_back(100, window.height - 200, 60, 200);
    loc[2].plats.emplace_back(600, window.height - 200, 60, 200);
    loc[2].plats.emplace_back(1200, window.height - 200, 60, 200);

    loc[0].enemies.emplace_back(1500, window.height - 100, 2000);
    loc[0].enemies.emplace_back(1800, window.height - 100, 2100);
    loc[1].enemies.emplace_back(1800, window.height - 100, 2100);
    loc[1].enemies.emplace_back(1100, window.height - 100, 2100);

    LocInfo(0, "loc0.bmp", 2, 1);
    LocInfo(1, "loc1.bmp", 0, 2);
    LocInfo(2, "loc2.bmp", 1, 0);

    player.current_location = 0;
    player.hero_sprite.x = 100;
    player.hero_sprite.y = window.height - 100;
    player.hero_sprite.width = 100;
    player.hero_sprite.height = 100;
    player.hero_sprite.hBitmap = Show("hero_right.bmp");
    player.hero_sprite.speed = 25;

    //enemy.currentloc = 0;
    
    
    /*enemy.enemy_sprite.x = 1500;
    enemy.enemy_sprite.y = window.height - 100;
    enemy.enemy_sprite.width = 100;
    enemy.enemy_sprite.height = 100;
    enemy.enemy_sprite.hBitmap = Show("enemy_right.bmp");*/

    /*Walck.startPos = loc[0]enemies.x;
    Walck.endPos = 2000;
    Walck.speed = 5.;*/
    //Walck.progress = 0;
    

}

void ShowScore()
{
    //поиграем шрифтами и цветами
    SetTextColor(window.context, RGB(160, 160, 160));
    SetBkColor(window.context, RGB(0, 0, 0));
    SetBkMode(window.context, TRANSPARENT);
    auto hFont = CreateFont(70, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "CALIBRI");
    auto hTmp = (HFONT)SelectObject(window.context, hFont);

    char txt[32];//буфер для текста
    _itoa_s(player.life, txt, 10);//преобразование числовой переменной в текст. текст окажется в переменной txt
    TextOutA(window.context, 10, 10, "Health", 6);
    TextOutA(window.context, 200, 10, (LPCSTR)txt, strlen(txt));
    _itoa_s(game.balls, txt, 10);
    TextOutA(window.context, 10, 100, "ITEMS", 5);
}

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

int gravity = 15;
int jump = 0;
bool isJumping = false;

void ProcessInput()
{
    if (GetAsyncKeyState('A')) {
        player.hero_sprite.hBitmap = Show("hero_left.bmp");
        player.hero_sprite.x -= player.hero_sprite.speed;
    }
    if (GetAsyncKeyState('D')) {
        player.hero_sprite.hBitmap = Show("hero_right.bmp");
        player.hero_sprite.x += player.hero_sprite.speed;
    }
    auto pl_s = player.hero_sprite;

        
        if ( GetAsyncKeyState('W') && !isJumping) {
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
bool Moving = true;


void Enemy::EnemyMove() {
    
    
        for (int i = 0; i < loc[player.current_location].enemies.size(); i++) {
            auto enemy = loc[player.current_location].enemies[i].en_sprite;

            if (Moving) {
                loc[player.current_location].enemies[i].en_sprite.x += enemy.speed;
                loc[player.current_location].enemies[i].en_sprite.hBitmap = Show("enemy_right.bmp");

                if (enemy.x >= loc[player.current_location].enemies[i].endPos) {
                    Moving = false;
                }
            }
            else {
                loc[player.current_location].enemies[i].en_sprite.x -= enemy.speed;
                loc[player.current_location].enemies[i].en_sprite.hBitmap = Show("enemy_left.bmp");

                if (enemy.x <= loc[player.current_location].enemies[i].startPos) {
                    Moving = true;
                }
            }
        }
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
    for (int i = 0; i < loc[player.current_location].enemies.size(); i++) {
        auto enemy = loc[player.current_location].enemies[i].en_sprite;
        ShowBitmap(window.context, enemy.x, enemy.y, enemy.width, enemy.height, enemy.hBitmap);
    }
    //ShowBitmap(window.context, enemy.enemy_sprite.x, enemy.enemy_sprite.y, enemy.enemy_sprite.width, enemy.enemy_sprite.height, enemy.enemy_sprite.hBitmap);
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

void InitWindow()
{
    SetProcessDPIAware();
    window.hWnd = CreateWindow("edit", 0, WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0);

    RECT r;
    GetClientRect(window.hWnd, &r);
    window.device_context = GetDC(window.hWnd);//из хэндла окна достаем хэндл контекста устройства для рисования
    window.width = r.right - r.left;//определяем размеры и сохраняем
    window.height = r.bottom - r.top;
    window.context = CreateCompatibleDC(window.device_context);//второй буфер
    SelectObject(window.context, CreateCompatibleBitmap(window.device_context, window.width, window.height));//привязываем окно к контексту
    GetClientRect(window.hWnd, &r);

}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    
    InitWindow();//здесь инициализируем все что нужно для рисования в окне
    InitGame();//здесь инициализируем переменные игры

    //mciSendString(TEXT("play ..\\Debug\\music.mp3 repeat"), NULL, 0, NULL);
    ShowCursor(NULL);
    
    while (!GetAsyncKeyState(VK_ESCAPE))
    {
        ShowSprites();//рисуем фон, героя, предметы и платформы
        ProcessInput();//опрос клавиатуры
        for (auto p : loc[player.current_location].enemies) {
            p.EnemyMove();

        }
        ShowScore();//рисуем очик и жизни
        BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//копируем буфер в окно
        Sleep(16);//ждем 16 милисекунд (1/количество кадров в секунду)

        Collusion();//коллизия
        LimitHero();//проверяем, чтобы ракетка не убежала за экран
    }

}
