//linker::system::subsystem  - Windows(/ SUBSYSTEM:WINDOWS)
//configuration::advanced::character set - not set
//linker::input::additional dependensies Msimg32.lib; Winmm.lib




#include"PornHub.h"
#include "Bitmaps.h"    
#include "functions.h"



//cекция кода


void ItemInfo(string name, int x, int y, int height, int width, LPCSTR it_name)
{
    item_ i;
    i.name = name;
    i.Sprite.x = x;
    i.Sprite.y = y;
    i.Sprite.height = height;
    i.Sprite.width = width;
    i.Sprite.hBitmap = Load(it_name);
    itemLib.push_back(i);
}

void LocInfo(int numb, LPCSTR loc_name, int l_port, int r_port) 
{
    loc[numb].hBitmap = Load(loc_name);
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
    loc[0].plats.emplace_back(window.width / 1.9, window.height - 200, 600, 60);
    loc[0].plats.emplace_back(window.width / 6, window.height - 350, 60, 600);
    loc[1].plats.emplace_back(0, window.height - 200, 60, 900);
    loc[1].plats.emplace_back(window.width / 1.7, window.height - 200, 60, window.width - window.width / 1.7);
    loc[2].plats.emplace_back(window.width / 23, window.height - 400, 60, 600);
    loc[2].plats.emplace_back(window.width / 6, window.height - 200, 60, 1000);
    loc[2].plats.emplace_back(window.width / 1.4, window.height - 500, 60, 600);

    //loc[0].enemies.emplace_back(window.width / 1.9, window.height - 300, window.width / 1.9 + 500, 3);
    //loc[0].enemies.emplace_back(window.width / 6, window.height - 450, window.width / 6 + 500, 3);
    //loc[0].enemies.emplace_back(window.width / 4, window.height - 100, window.width / 4 + 500, 3);
    loc[1].enemies.emplace_back(window.width / 4, window.height - 100, window.width / 4 + 500, 3);
    loc[1].enemies.emplace_back(window.width / 1.4, window.height - 300, window.width / 1.1, 3);

    LocInfo(0, "loc0.bmp", 2, 1);
    LocInfo(1, "loc1.bmp", 0, 2);
    LocInfo(2, "loc2.bmp", 1, 0);

    player.current_location = 0;
    player.life = 10;
    player.hero_sprite.x = 100;
    player.hero_sprite.y = window.height - 100;
    player.hero_sprite.width = 100;
    player.hero_sprite.height = 100;
    player.hero_sprite.speed = 25;
    player.hBitmapRight = Load("hero_right.bmp");
    player.hBitmapLeft = Load("hero_left.bmp");
    player.hero_sprite.hBitmap = player.hBitmapRight;

    //Character character(100, window.height,300, 15);
    Character* a1 = new Character(1000, 1100, 1500, 15);
    Wolf* a2 = new Wolf(500, window.height - 300, 1000, 25, 150, 150);
    Hare* a3 = new Hare(1500, 500, 1700, 5, 50, 50);
    
    loc[0].chars.push_back(a1);
    loc[0].chars.push_back(a2);
    loc[0].chars.push_back(a3);

}

HFONT hFont = NULL;

void ShowScore()
{
    //поиграем шрифтами и цветами
    SetTextColor(window.context, RGB(160, 160, 160));
    SetBkColor(window.context, RGB(0, 0, 0));
    SetBkMode(window.context, TRANSPARENT);
    if (!hFont)
    {
        hFont = CreateFont(70, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "CALIBRI");
    }

    auto hTmp = (HFONT)SelectObject(window.context, hFont);

    char txt[32];//буфер для текста
    _itoa_s(player.life, txt, 10);//преобразование числовой переменной в текст. текст окажется в переменной txt
    TextOutA(window.context, 10, 10, "Health", 6);
    TextOutA(window.context, 200, 10, (LPCSTR)txt, strlen(txt));
    _itoa_s(game.balls, txt, 10);
    TextOutA(window.context, 10, 100, "ITEMS", 5);
}

void GameOver() {
    if (player.life == 0) {
        MessageBox(window.hWnd, "Вы погибли", "PORNO", MB_OK);
        ClearVectors();
        InitGame();
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
        GameOver();
        
        ShowSprites();//рисуем фон, героя, предметы и платформы
        
        ProcessInput();//опрос клавиатуры

        Trace();
        ShowScore();//рисуем очик и жизни
        BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//копируем буфер в окно
        Sleep(16);//ждем 16 милисекунд (1/количество кадров в секунду)

        Collusion();//коллизия
        for (auto& p : loc[player.current_location].enemies) {
            p.EnemyMove();
            p.EnemyCollusion();

        }
        for (Character* charact : loc[player.current_location].chars) {
            charact->Move();
        }
        LimitHero();//проверяем, чтобы ракетка не убежала за экран
    }

}
