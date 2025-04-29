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

/// <summary>
/// Метод, выводящий спрайт
/// </summary>
/// <param name="name">имя файла спрайта</param>
/// <returns></returns>
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

struct location_ {
    HBITMAP hBitmap;
    int left_portal;
    int right_portal;
    platform_ platform;
    vector<Platform> plats;
    vector <item_> items;
};

location_ loc[5];

//void PlatformImage(int numb, int plat_x, int plat_y, int plat_height, int plat_width)
//{
//    Platform pl( plat_x, plat_y, plat_height, plat_width );
//    loc[numb].plats.emplace_back(pl);
//}

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

/// <summary>
/// Метод, выводящий спрайты предметов
/// </summary>
/// <param name="name">имя предмета</param>
/// <param name="x">координата х</param>
/// <param name="y">координата у</param>
/// <param name="height">высота</param>
/// <param name="width">ширина</param>
/// <param name="it_name">имя файла спрайта</param>
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
/// <summary>
/// Метод, выводящий спрайты платформ
/// </summary>
/// <param name="numb">номер локации</param>
/// <param name="x">координата х</param>
/// <param name="y">координата у</param>
/// <param name="height">высота</param>
/// <param name="width">ширина</param>
//void PlatformInfo(int numb, int x, int y)
//{
//    loc[numb].platform.plat_sprite.x = x;
//    loc[numb].platform.plat_sprite.y = y;
//    loc[numb].platform.plat_sprite.height = 60;
//    loc[numb].platform.plat_sprite.width = 600;
//    loc[numb].platform.plat_sprite.hBitmap = Show("platform.bmp");
//}
/// <summary>
/// Метод, выводящий локацию с порталами
/// </summary>
/// <param name="numb">номер локации</param>
/// <param name="loc_name">имя файла спрайта</param>
/// <param name="l_port">номер локации слева</param>
/// <param name="r_port">номер локации справа</param>
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

    loc[0].plats.emplace_back(window.width / 1.9, window.height - 250, 60, 600);
    //loc[0].plats.emplace_back(window.width / 6, window.height - 350, 60, 600);
    loc[1].plats.emplace_back(0, window.height - 250, 60, 900);
    loc[1].plats.emplace_back(window.width / 1.7, window.height - 450, 60, window.width - window.width / 1.7);
    loc[2].plats.emplace_back(window.width / 6, window.height - 250, 60, 600);
    loc[2].plats.emplace_back(window.width / 1.8, window.height - 450, 60, 600);
    loc[2].plats.emplace_back(window.width / 8, window.height - 650, 60, 600);

    LocInfo(0, "loc0.bmp", 2, 1);
    LocInfo(1, "loc1.bmp", 0, 2);
    LocInfo(2, "loc2.bmp", 1, 0);

    player.current_location = 0;
    player.hero_sprite.x = 2000;
    player.hero_sprite.y = window.height - 100;
    player.hero_sprite.width = 100;
    player.hero_sprite.height = 100;
    player.hero_sprite.hBitmap = Show("hero_right.bmp");
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

void Collusion() 
{
    auto pl_s = player.hero_sprite;
    
    for (int i = 0; i < loc[player.current_location].plats.size(); i++)
    {
        auto platform = loc[player.current_location].plats[i].pl_sprite;
        if ((pl_s.y + pl_s.height >= platform.y && 
            pl_s.y <= platform.y) && 
            (pl_s.x + pl_s.width >= platform.x && //над платформой
            pl_s.x <= platform.x + platform.width)) {

            player.hero_sprite.y = platform.y - player.hero_sprite.height;
            gravity = 0;
        }
        else if (pl_s.y <= platform.y + platform.height && pl_s.y + pl_s.height >= platform.y + platform.height && pl_s.x + pl_s.width >= platform.x &&//под платформой
            pl_s.x <= platform.x + platform.width) {

            player.hero_sprite.y = platform.y + platform.height;
        }
        else {
            gravity = 15;
        }
    }
}

void ProcessInput()
{
    if (GetAsyncKeyState('A')) {
        player.hero_sprite.hBitmap = Show("hero_left.bmp");
        player.hero_sprite.x -= 25;
    }
    if (GetAsyncKeyState('D')) {
        player.hero_sprite.hBitmap = Show("hero_right.bmp");
        player.hero_sprite.x += 25;
    }
    auto pl_s = player.hero_sprite;

    for (int i = 0; i < loc[player.current_location].plats.size(); i++)
    {
        auto platform = loc[player.current_location].plats[i].pl_sprite;

        bool isOnGround = (pl_s.y == window.height - pl_s.height);
        bool isOnPlatform = (pl_s.y + pl_s.height == platform.y);

        if (!isJumping && GetAsyncKeyState('W')) {
            jump += 40;
            isJumping = true;
        }
        if (isOnGround) {
            isJumping = false;
        }
        if (isOnPlatform) {
            isJumping = false;
        }
        player.hero_sprite.y += gravity - jump;
        //Sleep(1);
        player.hero_sprite.y = min(window.height - player.hero_sprite.height, player.hero_sprite.y);
        jump *= 0.91;
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
        LimitHero();//проверяем, чтобы ракетка не убежала за экран
        Collusion();//коллизия
    }

}
