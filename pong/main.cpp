//linker::system::subsystem  - Windows(/ SUBSYSTEM:WINDOWS)
//configuration::advanced::character set - not set
//linker::input::additional dependensies Msimg32.lib; Winmm.lib

#include "windows.h"
#include <vector>
#include <string>
#include <time.h>
#include <math.h>
using namespace std;


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

struct location_ {
    HBITMAP hBitmap;
    int left_portal;
    int right_portal;
    platform_ platform;
    vector<platform_> plats;
    vector <item_> items;
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

//cекция кода

auto Show(LPCSTR name) {
    
    return (HBITMAP)LoadImageA(NULL, name, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

void InitGame()
{
    item_ i;

    i.name = "axe";
    i.Sprite.x = 1150;
    i.Sprite.y = window.height - 50;;
    i.Sprite.height = 50;
    i.Sprite.width = 50;
    i.Sprite.hBitmap = Show("axe.bmp");
    itemLib.push_back(i);

    i.name = "hemlet";
    i.Sprite.x = 150;
    i.Sprite.y = window.height - 50;;
    i.Sprite.height = 50;
    i.Sprite.width = 50;
    i.Sprite.hBitmap = Show("hemlet.bmp");
    itemLib.push_back(i);

    i.name = "sword";
    i.Sprite.x = 850;
    i.Sprite.y = window.height - 50;;
    i.Sprite.height = 50;
    i.Sprite.width = 50;
    i.Sprite.hBitmap = Show("sword.bmp");
    itemLib.push_back(i);

    loc[0].items.push_back(itemLib[(int)itemID::axe]);
    loc[0].items.push_back(itemLib[(int)itemID::sword]);
    loc[2].items.push_back(itemLib[(int)itemID::hemlet]);

    loc[0].platform.plat_sprite.x = 1200;
    loc[0].platform.plat_sprite.y = window.height - 250;
    loc[0].platform.plat_sprite.height = 50;
    loc[0].platform.plat_sprite.width = 500;
    loc[0].platform.plat_sprite.hBitmap = Show("racket.bmp");

    loc[1].platform.plat_sprite.x = 100;
    loc[1].platform.plat_sprite.y = window.height - 250;
    loc[1].platform.plat_sprite.height = 50;
    loc[1].platform.plat_sprite.width = 500;
    loc[1].platform.plat_sprite.hBitmap = Show("racket.bmp");

    loc[2].platform.plat_sprite.x = 700;
    loc[2].platform.plat_sprite.y = window.height - 250;
    loc[2].platform.plat_sprite.height = 50;
    loc[2].platform.plat_sprite.width = 500;
    loc[2].platform.plat_sprite.hBitmap = Show("racket.bmp");

    loc[0].hBitmap = Show("loc0.bmp");
    loc[0].left_portal = 2;
    loc[0].right_portal = 1;

    loc[1].hBitmap = Show("loc1.bmp");
    loc[1].left_portal = 0;
    loc[1].right_portal = 2;

    loc[2].hBitmap = Show("loc2.bmp");
    loc[2].left_portal = 1;
    loc[2].right_portal = 0;

    player.current_location = 0;
    player.hero_sprite.x = 100;
    player.hero_sprite.y = window.height - 50;
    player.hero_sprite.width = 50;
    player.hero_sprite.height = 50;
    player.hero_sprite.hBitmap = Show("racket.bmp");
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

void ProcessInput()
{
    if (GetAsyncKeyState(VK_LEFT)) player.hero_sprite.x -= 15;
    if (GetAsyncKeyState(VK_RIGHT)) player.hero_sprite.x += 15;
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

void ShowSprites()
{
    ShowBitmap(window.context, 0, 0, window.width, window.height, loc[player.current_location].hBitmap);//задний фон

    for (int i = loc[player.current_location].items.size() - 1; i >= 0; i--) {
        auto item = loc[player.current_location].items[i].Sprite;
        //ShowBitmap(window.context, item.x, item.y, item.width, item.height, item.hBitmap);
        ShowBitmap(window.context, item.x, item.y, item.width, item.height, item.hBitmap);
        if (player.hero_sprite.x + player.hero_sprite.width >= item.x  && player.hero_sprite.x <= item.x + item.width &&
            player.hero_sprite.y + player.hero_sprite.height >= item.y){

            player.player_items.push_back(loc[player.current_location].items[i]);
            loc[player.current_location].items.erase(loc[player.current_location].items.begin() + i);
        }
    }

    if (!player.player_items.empty()) {
       for (int i = 0; i < player.player_items.size(); i++) {
           ShowBitmap(window.context, 200 + i * 50, 100, player.player_items[i].Sprite.width, player.player_items[i].Sprite.height, player.player_items[i].Sprite.hBitmap);
        }
    }

    ShowBitmap(window.context, player.hero_sprite.x, player.hero_sprite.y, player.hero_sprite.width, player.hero_sprite.height, player.hero_sprite.hBitmap );// ракетка игрока
    ShowBitmap(window.context, loc[player.current_location].platform.plat_sprite.x, loc[player.current_location].platform.plat_sprite.y,
        loc[player.current_location].platform.plat_sprite.width, loc[player.current_location].platform.plat_sprite.height, 
        loc[player.current_location].platform.plat_sprite.hBitmap);


    if (player.hero_sprite.y == loc[player.current_location].platform.plat_sprite.y + loc[player.current_location].platform.plat_sprite.height &&
        player.hero_sprite.x + player.hero_sprite.width >= loc[player.current_location].platform.plat_sprite.x &&
        player.hero_sprite.x <= loc[player.current_location].platform.plat_sprite.x + loc[player.current_location].platform.plat_sprite.width) {
        player.hero_sprite.y > loc[player.current_location].platform.plat_sprite.y - player.hero_sprite.height;
    }
    else if (player.hero_sprite.y + player.hero_sprite.height <= loc[player.current_location].platform.plat_sprite.y &&
        player.hero_sprite.x + player.hero_sprite.width >= loc[player.current_location].platform.plat_sprite.x &&
        player.hero_sprite.x <= loc[player.current_location].platform.plat_sprite.x + loc[player.current_location].platform.plat_sprite.width) {
        player.hero_sprite.y = loc[player.current_location].platform.plat_sprite.y - player.hero_sprite.height;
        gravity = 0;
    }
    else {
        gravity = 15;
    }
}
int jump = 0;

void Jump() {
    if (GetAsyncKeyState(VK_SPACE) && player.hero_sprite.y == window.height - player.hero_sprite.height)
         jump+=70;
    /*if (GetAsyncKeyState(VK_SPACE) && player.hero_sprite.y == loc[player.current_location].platform.plat_sprite.y)
        jump += 70;*/
    player.hero_sprite.y +=  gravity - jump;

    player.hero_sprite.y = min(window.height - player.hero_sprite.height, player.hero_sprite.y);
    jump *= 0.9;
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
        ShowScore();//рисуем очик и жизни
        BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//копируем буфер в окно
        Sleep(16);//ждем 16 милисекунд (1/количество кадров в секунду)

        ProcessInput();//опрос клавиатуры
        Jump();
        LimitHero();//проверяем, чтобы ракетка не убежала за экран
    }

}
