// g++ -Llib -mwindows main.cpp resources.o -lSDL2 -lSDL2_image -lSDL2_ttf -o Backgammon.exe 
// g++ -Llib -static-libstdc++ -mwindows main.cpp resources.o -lSDL2 -lSDL2_image -lSDL2_ttf -I./dependencies/include/GLFW -L./dependencies/lib -lglfw3  -lm -o Backgammon.exe
// ^ compile settings
// ./Backgammon
#define SDL_MAIN_HANDLED
#include "src\SDL.h"
#include "src\SDL_image.h"
#include "src\SDL_ttf.h"
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
using namespace std;
int alb[25], ngr[25];
int k = 0, x = 1, y = 1;
int bar_negru = 0;
int bar_alb = 0;
int punct_negru, punct_alb;
const int SCREEN_WIDTH = 576;
const int SCREEN_HEIGHT = 522;
int mouseX, mouseY;
char a[14][14];
SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Texture *backgroundTexture = nullptr;
enum GameState { MENU, NAME_INPUT, GAME , REPLAY , HELP};
enum class GamePhase
{
    RollDice,
    MovePiece
};
void afisaretablainitiala()
{
    int i, j;
    for (i = 0; i <= 13; i++)
        for (j = 0; j <= 13; j++)
        {
            if (i == 0 || j == 0 || j == 13 || i == 13)
                a[i][j] = '#';
            else if (i > 0 && i < 6 && j == 1)
                a[i][j] = '1';
            else if (i > 0 && i < 3 && j == 12)
                a[i][j] = '1';
            else if (i > 9 && i < 13 && j == 5)
                a[i][j] = '1';
            else if (i > 7 && i < 13 && j == 7)
                a[i][j] = '1';
            else if (i > 7 && i < 13 && j == 1)
                a[i][j] = '2';
            else if (i > 10 && i < 13 && j == 12)
                a[i][j] = '2';
            else if (i > 0 && i < 4 && j == 5)
                a[i][j] = '2';
            else if (i > 0 && i < 6 && j == 7)
                a[i][j] = '2';
            else
                a[i][j] = '0';
        }
}
SDL_Texture* renderText(SDL_Renderer* renderer, TTF_Font* font, const string& text, SDL_Color color, int& text_width, int& text_height) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (surface == NULL) {
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL) {
        cerr << "Unable to create texture from rendered text! SDL_Error: " << SDL_GetError() << endl;
    }
    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);
    return texture;
}
void renderMenu(SDL_Renderer* renderer, TTF_Font* font, const vector<string>& menuItems, int selectedItem) 
{
    SDL_Color normalColor = { 255, 255, 255, 255 };
    SDL_Color selectedColor = { 255, 0, 0, 255 };
    int text_width, text_height;
    for (int i = 0; i < menuItems.size(); ++i) {
        SDL_Color color = (i == selectedItem) ? selectedColor : normalColor;
        SDL_Texture* texture = renderText(renderer, font, menuItems[i], color, text_width, text_height);
        SDL_Rect renderQuad = { 200, 300 + i * 50, text_width, text_height };
        SDL_RenderCopy(renderer, texture, NULL, &renderQuad);
        SDL_DestroyTexture(texture);
    }
}
void renderNameInput(SDL_Renderer* renderer, TTF_Font* font, const string& prompt, const string& inputText) {
    SDL_Color whiteColor = { 255, 255, 255, 255 };
    int text_width, text_height;

    SDL_Texture* promptTexture = renderText(renderer, font, prompt, whiteColor, text_width, text_height);
    SDL_Rect promptQuad = { 100, 150, text_width, text_height };
    SDL_RenderCopy(renderer, promptTexture, NULL, &promptQuad);
    SDL_DestroyTexture(promptTexture);

    SDL_Texture* inputTexture = renderText(renderer, font, inputText, whiteColor, text_width, text_height);
    SDL_Rect inputQuad = { 100, 200, text_width, text_height };
    SDL_RenderCopy(renderer, inputTexture, NULL, &inputQuad);
    SDL_DestroyTexture(inputTexture);
}
int roll()
{
    srand(time(0));
    return (1 + (rand() % 6));
}
int roll2()
{
    srand(time(0) + 1);
    return (1 + (rand() % 6));
}
bool initSDL(SDL_Window *&window, SDL_Renderer *&renderer)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    window = SDL_CreateWindow("Backgammon", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << endl;
        return false;
    }

    return true;
}
SDL_Texture *loadTexture(const string &path, SDL_Renderer *renderer)
{
    SDL_Texture *texture = nullptr;

    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr)
    {
        cerr << "Unable to load image " << path.c_str() << "! SDL_image Error: " << IMG_GetError() << endl;
    }
    else
    {
        texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (texture == nullptr)
        {
            cerr << "Unable to create texture from " << path.c_str() << "! SDL Error: " << SDL_GetError() << endl;
        }
        SDL_FreeSurface(loadedSurface);
    }

    return texture;
}
void afiseazatabla(SDL_Texture *t1, SDL_Texture *t2)
{
    int x = 0;
    int y = 0;
    for (int i = 0; i <= 13; i++)
    {
        for (int j = 0; j <= 13; j++)
        {
            x = j * 40;
            y = i * 38;
            if (j >= 7)
            {
                x = x + 40;
                if (i >= 8)
                    y = y + 6;
                else
                    y = y - 8;
            }
            else
                x = x - 15;
            SDL_Rect destination_rect = {x, y, 40, 40};
            if (a[i][j] == '0')
                SDL_RenderCopy(renderer, NULL, NULL, &destination_rect);
            if (a[i][j] == '1')
                SDL_RenderCopy(renderer, t1, NULL, &destination_rect);
            else if (a[i][j] == '2')
                SDL_RenderCopy(renderer, t2, NULL, &destination_rect);
        }
    }
}
void closeSDL(SDL_Window *window, SDL_Renderer *renderer)
{   TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    
}
void MouseClickCoordinates(SDL_Event &e)
{
    if (e.type == SDL_MOUSEBUTTONDOWN)
    {

        SDL_GetMouseState(&mouseX, &mouseY);
        cout << mouseX << " " << mouseY << endl;
    }
}
int GetCol(SDL_Event &e)
{
    if (e.type == SDL_MOUSEBUTTONDOWN)
    {
        SDL_GetMouseState(&mouseX, &mouseY);
        if (mouseY < 260)
        {
            if (mouseX > 25 && mouseX < 65)
                return 1;
            else if (mouseX >= 65 && mouseX < 105)
                return 2;
            else if (mouseX >= 105 && mouseX < 145)
                return 3;
            else if (mouseX >= 145 && mouseX < 185)
                return 4;
            else if (mouseX >= 185 && mouseX < 225)
                return 5;
            else if (mouseX >= 225 && mouseX < 265)
                return 6;
            else if (mouseX >= 315 && mouseX < 355)
                return 7;
            else if (mouseX >= 355 && mouseX < 395)
                return 8;
            else if (mouseX >= 395 && mouseX < 435)
                return 9;
            else if (mouseX >= 435 && mouseX < 475)
                return 10;
            else if (mouseX >= 475 && mouseX < 515)
                return 11;
            else if (mouseX >= 515 && mouseX < 555)
                return 12;
            else
                return 0;
        }
        else
        {
            if (mouseX > 25 && mouseX < 65)
                return 13;
            else if (mouseX >= 65 && mouseX < 105)
                return 14;
            else if (mouseX >= 105 && mouseX < 145)
                return 15;
            else if (mouseX >= 145 && mouseX < 185)
                return 16;
            else if (mouseX >= 185 && mouseX < 225)
                return 17;
            else if (mouseX >= 225 && mouseX < 265)
                return 18;
            else if (mouseX >= 315 && mouseX < 355)
                return 19;
            else if (mouseX >= 355 && mouseX < 395)
                return 20;
            else if (mouseX >= 395 && mouseX < 435)
                return 21;
            else if (mouseX >= 435 && mouseX < 475)
                return 22;
            else if (mouseX >= 475 && mouseX < 515)
                return 23;
            else if (mouseX >= 515 && mouseX < 555)
                return 24;
            else
                return 0;
        }
    }
    return 0; // invalid
}
int GetZar(SDL_Event &e)
{
    if (e.type == SDL_MOUSEBUTTONDOWN)
    {
        SDL_GetMouseState(&mouseX, &mouseY);
        if (mouseX >= 268 && mouseX <= 304)
        {
            if (mouseY >= 173 && mouseY <= 208)
                return 1;
            else if (mouseY >= 229 && mouseY <= 259)
                return 2;
        }
    }
    return 0; // invalid
}
void numarare_piese_albe()
{
    for (int i = 1; i <= 24; i++)
        alb[i] = 0;
    for (int i = 1; i <= 6; i++)
    {
        for (int j = 1; j <= 12; j++)
            if (a[i][j] == '1')
                alb[j]++;
    }
    for (int i = 7; i <= 12; i++)
        for (int j = 1; j <= 12; j++)
            if (a[i][j] == '1')
                alb[12 + j]++;
}
void numarare_piese_negre()
{
    for (int i = 1; i <= 24; i++)
        ngr[i] = 0;
    for (int i = 1; i <= 6; i++)
    {
        for (int j = 1; j <= 12; j++)
            if (a[i][j] == '2')
                ngr[j]++;
    }
    for (int i = 7; i <= 12; i++)
        for (int j = 1; j <= 12; j++)
            if (a[i][j] == '2')
                ngr[12 + j]++;
}
int Check_pul_casa_alb()
{
    int s = 0;
    numarare_piese_albe();
    for (int i = 1; i <= 18; i++)
        s += alb[i];
    return s;
}
int Check_pul_casa_negru()
{
    int s = 0;
    numarare_piese_negre();
    for (int i = 1; i <= 6; i++)
        s += ngr[i];
    for (int i = 13; i <= 24; i++)
        s += ngr[i];
    return s;
}
int check_win_cond_alb()
{
    numarare_piese_albe();
    int s = 0;
    for (int i = 1; i <= 24; i++)
        s += alb[i];
    return s;
}
int check_win_cond_negru()
{
    numarare_piese_negre();
    int s = 0;
    for (int i = 1; i <= 24; i++)
        s += ngr[i];
    return s;
}
int penalty_alb(int zarr)
{
    numarare_piese_negre();
    numarare_piese_albe();
    if (ngr[12 - zarr + 1] == 1)
    {
        bar_negru++;
        bar_alb--;
        a[1][12 - zarr + 1] = '1';
        return 1;
    }
    else
    {
        a[alb[12 - zarr + 1] + 1][12 - zarr + 1] = '1';
        bar_alb--;
        return 1;
    }
}
int penalty_negru(int zarr)
{
    numarare_piese_albe();
    numarare_piese_negre();
    if (alb[24 - zarr + 1] >= 2)
    {
        return 0;
    }
    else if (alb[24 - zarr + 1] == 1)
    {
        bar_negru--;
        bar_alb++;
        a[12][12 - zarr + 1] = '2';

        return 1;
    }
    else
    {
        a[12 - ngr[24 - zarr + 1]][12 - zarr + 1] = '2';
        bar_negru--;

        return 1;
    }
}
void scoaterepiesa_alba(int p, int zarr)
{
    if (x % 4 == 0)
    {
        cout << " 3 mutari invalide, vom trece peste acest zar" << endl;

        x++;
        return;
    }
    numarare_piese_albe();
    if (alb[p] == 0)
    {
        cout << "Mutare invalida,alege alta coloana" << endl;

        cin >> p;
        x++;
        scoaterepiesa_alba(p, zarr);
        return;
    }
    else if (zarr > 25 - p && alb[25 - zarr] != 0)
    {
        cout << "Mutare invalida,alege alta coloana" << endl;

        cin >> p;
        x++;
        scoaterepiesa_alba(p, zarr);
        return;
    }
    if (p + zarr >= 25)
        a[13 - alb[p]][p - 12] = 0;

    else if (p > 12) // muta din stanga in dr partea jos
    {

        numarare_piese_negre();
        if (ngr[p + zarr] >= 2) // CONDITIA DE POARTA
        {
            cout << "Mutare invalida,alege alta coloana" << endl;

            cin >> p;
            x++;
            scoaterepiesa_alba(p, zarr);
            return;
        }
        else if (ngr[p + zarr] == 1)
        {
            a[13 - alb[p]][p - 12] = 0;
            a[12 - alb[p + zarr]][p + zarr - 12] = '1';
            bar_negru++;
        }
        else
        {
            a[12 - alb[p + zarr]][p + zarr - 12] = '1';
            a[13 - alb[p]][p - 12] = 0;
        }
    }
}
void scoaterepiesa_neagra(int p, int zarr)
{
    if (y % 4 == 0)
    {
        cout << "3 mutari invalide, vom trece peste acest zar" << endl;

        y++;
        return;
    }
    numarare_piese_negre();
    if (ngr[p] == 0)
    {
        cout << "Mutare invalida,alege alta coloana" << endl;

        cin >> p;
        y++;
        scoaterepiesa_neagra(p, zarr);
    }
    else if (zarr > 13 - p && ngr[13 - zarr] != 0)
    {
        cout << "Mutare invalida,alege alta coloana" << endl;

        cin >> p;
        y++;
        scoaterepiesa_neagra(p, zarr);
        return;
    }
    if (zarr + p >= 13)
    {
        a[ngr[p]][p] = 0;
    }
    else // st->dr
    {
        numarare_piese_albe();
        if (alb[p + zarr] >= 2)
        {
            cout << "Mutare invalida,alege alta coloana" << endl;

            cin >> p;
            y++;
            scoaterepiesa_neagra(p, zarr);
            return;
        }
        else if (alb[p + zarr] == 1)
        {
            a[ngr[p]][p] = 0;
            a[1 + ngr[p + zarr]][p + zarr] = '2';
            bar_alb++;
        }
        else
        {
            a[1 + ngr[p + zarr]][p + zarr] = '2';
            a[ngr[p]][p] = 0;
        }
    }
}
int validAlb_penalty(int zarr)
{
    numarare_piese_albe();
    numarare_piese_negre();
    if (ngr[12 - zarr + 1] >= 2 || alb[12-zarr+1]==6)
        return 0;
    else
        return 1;
}
int validAlb_scoatere(int p, int zarr)
{
    numarare_piese_albe();
    numarare_piese_negre();
    if (alb[p] == 0 || (zarr > 25 - p && alb[25 - zarr] != 0) || ngr[p + zarr] >= 2 || alb[p+zarr]==6) 
        return 0;
    return 1;
}
int validAlb(int p, int zarr)
{
    numarare_piese_albe();
    numarare_piese_negre();
    if (alb[p] == 0 || zarr + p > 24 || (zarr - p >= 0 && (ngr[13 + zarr - p] >= 2 || alb[13+zarr-p]==6)) || (p <= 12 && (ngr[p - zarr] >= 2 || alb[p-zarr]==6)) || (p > 12 && (ngr[p + zarr] >= 2 || alb[p+zarr]==6)))
        return 0;
    return 1;
}
int validNegru_penalty(int zarr)
{
    numarare_piese_albe();
    numarare_piese_negre();
    if (alb[24 - zarr + 1] >= 2 || ngr[24-zarr+1]==6)
    {
        return 0;
    }
    return 1;
}
int validNegru_scoatere(int p, int zarr)
{
    numarare_piese_albe();
    numarare_piese_negre();
    if (ngr[p] == 0 || (zarr > 13 - p && ngr[13 - zarr] != 0) || alb[p + zarr] >= 2 || ngr[p+zarr]==6)
        return 0;
    return 1;
}
int validNegru(int p, int zarr)
{
    numarare_piese_albe();
    numarare_piese_negre();
    if (ngr[p] == 0 || (p < 13 && p > 6 && p + zarr >= 13) || (p - zarr >= 7 && p - zarr < 13 && p>=13 && p<=18 && (alb[13 - p + zarr] >= 2 || ngr[13-p+zarr]==6)) || (p > 13 && (alb[p - zarr] >= 2 || ngr[p-zarr]==6)) || (p < 13 && (alb[p + zarr] >= 2 || ngr[p+zarr]==6)))
        return 0;
    return 1;
}
void mutarepiesa_alba(int p, int zarr)
{
    if (x % 4 == 0)
    {
        cout << " 3 mutari invalide, vom trece peste acest zar" << endl;

        x++;
        return;
    }
    numarare_piese_albe();
    if (alb[p] == 0)
    {
        cout << "Mutare invalida,alege alta coloana" << endl;

        cin >> p;
        x++;
        mutarepiesa_alba(p, zarr);
        return;
    }
    else if (zarr + p > 24)
    { // daca scoate din casa;

        cout << "Mutare invalida,alege alta coloana" << endl;

        cin >> p;
        x++;
        mutarepiesa_alba(p, zarr);
        return;
    }
    else if (zarr - p >= 0) // muta de sus in jos gen
    {

        numarare_piese_negre();
        if (ngr[13 + zarr - p] >= 2) // CONDITIA DE POARTA
        {
            cout << "Mutare invalida,alege alta coloana" << endl; // ceva gresit ai uitat sa adaugi cin>>p; (la fiecare mutare invalida, alge alta coloana)

            cin >> p;
            x++;
            mutarepiesa_alba(p, zarr);
            return;
        }
        else if (ngr[13 + zarr - p] == 1)
        {
            a[alb[p]][p] = 0; // PROBABIL ARE TREABA CU ACEST LUCRU CE AM SCRIS MAI SUS.
            a[12 - alb[13 + (zarr - p)]][1 + (zarr - p)] = '1';
            bar_negru++;
        }
        else
        {
            a[alb[p]][p] = 0; // PROBABIL ARE TREABA CU ACEST LUCRU CE AM SCRIS MAI SUS.
            a[12 - alb[13 + (zarr - p)]][1 + (zarr - p)] = '1';
        }
    }
    else if (p <= 12) // muta din dr in stanga partea sus
    {

        numarare_piese_negre();
        if (ngr[p - zarr] >= 2) // CONDITIA DE POARTA
        {
            cout << "Mutare invalida,alege alta coloana" << endl;

            cin >> p;
            x++;
            mutarepiesa_alba(p, zarr);
            return;
        }
        else if (ngr[p - zarr] == 1)
        {
            a[alb[p]][p] = 0;
            a[alb[p - zarr] + 1][p - zarr] = '1';
            bar_negru++;
        }
        else
        {
            a[alb[p]][p] = 0;
            a[alb[p - zarr] + 1][p - zarr] = '1';
        }
    }
    else if (p > 12) // muta din stanga in dr partea jos
    {

        numarare_piese_negre();
        if (ngr[p + zarr] >= 2) // CONDITIA DE POARTA
        {
            cout << "Mutare invalida,alege alta coloana" << endl;

            cin >> p;
            x++;
            mutarepiesa_alba(p, zarr);
            return;
        }
        else if (ngr[p + zarr] == 1)
        {
            a[13 - alb[p]][p - 12] = 0;
            a[12 - alb[p + zarr]][p + zarr - 12] = '1';
            bar_negru++;
        }
        else
        {
            a[13 - alb[p]][p - 12] = 0;
            a[12 - alb[p + zarr]][p + zarr - 12] = '1';
        }
    }
}
void mutarepiesa_neagra(int p, int zarr) 
{
    if (y % 4 == 0)
    {
        cout << "3 mutari invalide, vom trece peste acest zar" << endl;

        y++;
        return;
    }
    numarare_piese_negre();
    if (ngr[p] == 0)
    {
        cout << "Mutare invalida,alege alta coloana" << endl;
        
        cin >> p;
        y++;
        mutarepiesa_neagra(p, zarr);
        return;
    }
    else if (p < 13 && p > 6 && p + zarr >= 13)
    {
        cout << "Mutare invalida,alege alta coloana" << endl;
        cout <<"2";
        cin >> p;
        y++;
        mutarepiesa_neagra(p, zarr);
        return;
    }
    else if (p - zarr >= 7 && p - zarr < 13 && p>=13 && p<=18) // jos in sus
    {
        numarare_piese_albe();
        if (alb[13 - p + zarr] >= 2)
        {
            cout << "Mutare invalida,alege alta coloana" << endl;
            cout <<"3";
            cin >> p;
            y++;
            mutarepiesa_neagra(p, zarr);
            return;
        }
        else if (alb[13 - p + zarr] == 1)
        {   cout <<"4";
            a[13 - ngr[p]][p - 12] = 0;
            a[1 + ngr[13 - p + zarr]][13 - p + zarr] = '2';
            bar_alb++;
        }
        else
        {
            cout << "5";
            a[1 + ngr[13 - p + zarr]][13 - p + zarr] = '2';
            a[13 - ngr[p]][p - 12] = 0;
        }
    }
    else if (p > 13) // dr->st
    {

        numarare_piese_albe();
        if (alb[p - zarr] >= 2)
        {
            cout << "Mutare invalida,alege alta coloana" << endl;
            cout << "6";
            cin >> p;
            y++;
            mutarepiesa_neagra(p, zarr);
            return;
        }
        else if (alb[p - zarr] == 1)
        {
            cout << "7";
            a[13 - ngr[p]][p - 12] = 0;
            a[12 - ngr[p - zarr]][p - zarr - 12] = '2';
            bar_alb++;
        }
        else
        {
            cout << "8";
        
            a[12 - ngr[p - zarr]][p - zarr - 12] = '2';
            a[13 - ngr[p]][p - 12] = 0;
        }
    }
    else if (p < 13) // st->dr
    {

        numarare_piese_albe();
        if (alb[p + zarr] >= 2)
        {
            cout << "Mutare invalida,alege alta coloana" << endl;
            cout <<"9";
            cin >> p;
            y++;
            mutarepiesa_neagra(p, zarr);
            return;
        }
        else if (alb[p + zarr] == 1)
        {   cout <<"10";
            a[ngr[p]][p] = 0;
            a[1 + ngr[p + zarr]][p + zarr] = '2';
            bar_alb++;
        }
        else
        {   cout <<"11";
            a[ngr[p]][p] = 0;
            a[1 + ngr[p + zarr]][p + zarr] = '2';
        }
    }
}

#undef main
int main(int argc, char *argv[])
{

    if (!initSDL(window, renderer))
    {
        cerr << "Failed to initialize SDL." << endl;
        return -1;
    }
    SDL_Texture *backgroundTexture2= loadTexture("res/background2.png", renderer);
    backgroundTexture = loadTexture("res/background.png", renderer);
    SDL_Texture *backgroundTexture3 = loadTexture("res/background3.png", renderer);
    if (backgroundTexture == nullptr)
    {
        cerr << "Failed to load background image." << endl;
        closeSDL(window, renderer);
        return -1;
    }
    if (TTF_Init() == -1) {
        cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << endl;
        SDL_Quit();
        return 1;
    }
        TTF_Font *font = TTF_OpenFont("fonts/papyrus.ttf", 24);
        SDL_Color textColor = {255,255,255,0};
        SDL_Color redColor = {255, 0, 0, 255};
        SDL_Texture *texture = loadTexture("res/piesa_1.png", renderer);
        SDL_Texture *texture2 = loadTexture("res/piesa_2.png", renderer);
        SDL_Texture *zar1 = loadTexture("res/zar_1.png", renderer);
        SDL_Texture *zar2 = loadTexture("res/zar_2.png", renderer);
        SDL_Texture *zar3 = loadTexture("res/zar_3.png", renderer);
        SDL_Texture *zar4 = loadTexture("res/zar_4.png", renderer);
        SDL_Texture *zar5 = loadTexture("res/zar_5.png", renderer);
        SDL_Texture *zar6 = loadTexture("res/zar_6.png", renderer);
    
    bool quit = false;
    SDL_Event e;
    afisaretablainitiala();
    GameState currentState = MENU;
    vector<string> menuItems = { "Start Game", "Exit", "How to play" };
    vector<string> playAgainMenuItems = {"Play Again", "Exit"};
    int selectedItem = 0;
    string player1Name = "";
    string player2Name = "";
    bool enteringPlayer1 = true;
    SDL_StartTextInput();
    int tura = 1;
    int alegere = 0, coloana;
    bool castiga_alb = false;
    bool castiga_negru = false;
    int punct_alb = 0;
    int punct_negru = 0;
    int zarr1, zarr2;
    GamePhase phase = GamePhase::RollDice;
    int z1, z2, ap = 0;
    zarr1 = roll();
    zarr2 = roll2();
    int resetZar = 0;
    while (quit != true)
    {   
        if(check_win_cond_alb() == 0)
        {
            
            afisaretablainitiala();
            punct_alb++;
            castiga_alb = false;
            alegere = 0;
            ap = 0;
            tura = 1;
            zarr1 = roll();
            zarr2 = roll2();
            currentState = REPLAY;
            selectedItem = 0;
        }
        if(check_win_cond_negru() == 0)
        {
            afisaretablainitiala();
            punct_negru++;
            alegere = 0;
            ap = 0;
            tura = 2;
            zarr1 = roll();
            zarr2 = roll2();
            currentState = REPLAY;
            selectedItem = 0;
        }
        while (SDL_PollEvent(&e) != 0)
        {

            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN && currentState==GAME)
            {
                MouseClickCoordinates(e);
                if (phase == GamePhase::RollDice)
                {
                    cout << "dice phase";
                    alegere = GetZar(e);
                    z1 = zarr1;
                    z2 = zarr2;
                    if (alegere != 0)
                        phase = GamePhase::MovePiece;
                }
                else if (phase == GamePhase::MovePiece)
                {
                    if (tura % 2 == 1)
                    {
                        if (bar_alb != 0 && ap != 2)
                        {
                            if (alegere == 1)
                            {
                                if (validAlb_penalty(z1))
                                {
                                    penalty_alb(z1);
                                    alegere = 2;
                                    ap++;
                                }
                                else if (validAlb_penalty(z2))
                                {
                                    penalty_alb(z2);
                                    alegere = 1;
                                    ap++;
                                }
                                else
                                {
                                    cout << "nu ai mutari valide" << endl;
                                    ap = 2;
                                }
                            }
                            else if (alegere == 2)
                            {
                                if (validAlb_penalty(z2))
                                {
                                    penalty_alb(z2);
                                    alegere = 1;
                                    ap++;
                                }
                                else if (validAlb_penalty(z1))
                                {
                                    penalty_alb(z1);
                                    alegere = 2;
                                    ap++;
                                }
                                else
                                {
                                    cout << "nu ai mutari valide" << endl;
                                    ap = 2;
                                }
                            }
                        }
                        if (Check_pul_casa_alb() == 0 && ap != 2)
                        {
                            if (alegere == 1 && ap != 2)
                            {
                                coloana = GetCol(e);
                                if (validAlb_scoatere(coloana, z1))
                                {
                                    numarare_piese_albe();
                                    scoaterepiesa_alba(coloana, z1);
                                    alegere = 2;
                                    ap++;
                                    resetZar = 0;
                                }
                                else
                                {
                                    resetZar++;
                                    if(resetZar%6==0)
                                        {
                                            alegere = 2;
                                            ap++;
                                        }
                                }
                            
                            }
                            else if (alegere == 2 && ap != 2)
                            {
                                coloana = GetCol(e);
                                if (validAlb_scoatere(coloana, z2))
                                {
                                    numarare_piese_albe();
                                    scoaterepiesa_alba(coloana, z2);
                                    alegere = 1;
                                    ap++;
                                    resetZar = 0;
                                }
                                else
                                {
                                    resetZar++;
                                    if(resetZar%6==0)
                                        {
                                            alegere = 1;
                                            ap++;
                                        }
                                }
                            }
                        }
                        else
                        {
                            if (alegere == 1 && ap != 2)
                            {
                                cout << "Ai ales primul zar selecteaza coloana de la care vrei sa muti piesa" << endl;
                                coloana = GetCol(e);
                                if (validAlb(coloana, z1))
                                {
                                
                                    numarare_piese_albe();
                                    mutarepiesa_alba(coloana, z1);
                                    alegere = 2;
                                    ap++;
                                }
                                else
                                {
                                    resetZar++;
                                    if(resetZar%6==0)
                                        {
                                            alegere = 2;
                                            ap++;
                                        }
                                }
                            }
                            else if (alegere == 2 && ap != 2)
                            {
                                cout << "Ai ales al doilea zar selecteaza coloana de la care vrei sa muti piesa" << endl;
                                coloana = GetCol(e);
                                if (validAlb(coloana, z2))
                                { 
                                    numarare_piese_albe();
                                    mutarepiesa_alba(coloana, z2);
                                    alegere = 1;
                                    ap++;
                                }
                                else
                                {
                                    resetZar++;
                                    if(resetZar%6==0)
                                        {
                                            alegere = 1;
                                            ap++;
                                        }
                                }
                            }
                        }
                    }
                    else
                    {
                        if (bar_negru != 0 && ap != 2)
                        {
                            if (alegere == 1)
                            {
                                if (validNegru_penalty(z1))
                                {
                                    penalty_negru(z1);
                                    alegere = 2;
                                    ap++;
                                }
                                else if (validNegru_penalty(z2))
                                {
                                    penalty_negru(z2);
                                    alegere = 1;
                                    ap++;
                                }
                                else
                                {
                                    cout << "nu ai mutari valide" << endl;
                                    ap = 2;
                                }
                            }
                            else if (alegere == 2)
                            {
                                if (validNegru_penalty(z2))
                                {
                                    penalty_negru(z2);
                                    alegere = 1;
                                    ap++;
                                }
                                else if (validNegru_penalty(z1))
                                {
                                    penalty_negru(z1);
                                    alegere = 2;
                                    ap++;
                                }
                                else
                                {
                                    cout << "nu ai mutari valide" << endl;
                                    ap = 2;
                                }
                            }
                        }
                        if (Check_pul_casa_negru() == 0 && ap != 2)
                        {
                            if (alegere == 1 && ap != 2)
                            {
                                coloana = GetCol(e);
                                if (validNegru_scoatere(coloana, z1))
                                {
                                    numarare_piese_negre();
                                    scoaterepiesa_neagra(coloana, z1);
                                    alegere = 2;
                                    ap++;
                                }
                                else
                                {
                                    resetZar++;
                                    if(resetZar%6==0)
                                        {
                                            alegere = 2;
                                            ap++;
                                        }
                                }
                            }
                            else if (alegere == 2 && ap != 2)
                            {
                                coloana = GetCol(e);
                                if (validNegru_scoatere(coloana, z2))
                                {
                                    numarare_piese_negre();
                                    scoaterepiesa_neagra(coloana, z2);
                                    alegere = 1;
                                    ap++;
                                }
                                else
                                {
                                    resetZar++;
                                    if(resetZar%6==0)
                                        {
                                            alegere = 1;
                                            ap++;
                                        }
                                }
                            }
                        }
                        else
                        {
                            if (alegere == 1 && ap != 2)
                            {
                                coloana = GetCol(e);
                                if (validNegru(coloana, z1))
                                {
                                    numarare_piese_negre();
                                    mutarepiesa_neagra(coloana, z1);
                                    alegere = 2;
                                    ap++;
                                }
                                else
                                {
                                    resetZar++;
                                    if(resetZar%6==0)
                                        {
                                            alegere = 2;
                                            ap++;
                                        }
                                }
                            }
                            else if (alegere == 2 && ap != 2)
                            {
                                coloana = GetCol(e);
                                if (validNegru(coloana, z2))
                                {
                                    numarare_piese_negre();
                                    mutarepiesa_neagra(coloana, z2);
                                    alegere = 1;
                                    ap++;
                                }
                                else
                                {
                                    resetZar++;
                                    if(resetZar%6==0)
                                        {
                                            alegere = 1;
                                            ap++;
                                        }
                                }
                            }
                        }
                    }
                    if (ap == 2)
                    {
                        alegere = 0;
                        ap = 0;
                        phase = GamePhase::RollDice;
                        zarr1 = roll();
                        zarr2 = roll2();
                        tura++;
                        
                    }
                }
            }
            else if (e.type==SDL_KEYDOWN)
            {
                switch(e.key.keysym.sym)
                {
                    case SDLK_ESCAPE :
                    {
                        if(ap==0 && currentState==GAME)
                            {
                                phase = GamePhase::RollDice;
                                alegere = 0;
                            }
                            break;
                    }
                    case SDLK_UP:
                        if (currentState == MENU) 
                        {
                            selectedItem = (selectedItem > 0) ? selectedItem - 1 : menuItems.size() - 1;
                        }
                        else if(currentState == REPLAY)
                        {
                            selectedItem = (selectedItem > 0) ? selectedItem - 1 : playAgainMenuItems.size() - 1;
                        }
                        break;
                    case SDLK_DOWN:
                        if (currentState == MENU)
                         {
                            selectedItem = (selectedItem < menuItems.size() - 1) ? selectedItem + 1 : 0;
                        }
                        else if(currentState == REPLAY)
                        {
                            selectedItem = (selectedItem < playAgainMenuItems.size() - 1) ? selectedItem + 1 : 0;
                        }
                        break;
                    case SDLK_RETURN:
                        if (currentState == MENU) 
                        {
                            if (selectedItem == 0) 
                            { 
                                currentState = NAME_INPUT;
                            } else if (selectedItem == 1) 
                            {
                                quit = true;
                            }
                            else if (selectedItem == 2)
                            {
                                currentState = HELP;
                            }
                        } else if (currentState == NAME_INPUT) 
                        {
                            if (enteringPlayer1 && !player1Name.empty()) 
                            {
                                enteringPlayer1 = false;
                            } else if (!enteringPlayer1 && !player2Name.empty()) 
                            {
                                currentState = GAME;
                            }
                        }
                        else if (currentState == REPLAY)
                        {
                            if (selectedItem == 0)
                            {
                                currentState = GAME;
                            }
                            else if(selectedItem==1)
                            {
                                quit = true;
                            }
                        }
                        else if (currentState == HELP)
                        {
                            currentState = MENU;
                        }
                        break;
                    case SDLK_BACKSPACE:
                        if (currentState == NAME_INPUT) 
                        {
                            if (enteringPlayer1) 
                            {
                                if (!player1Name.empty()) player1Name.pop_back();
                            } else 
                            {
                                if (!player2Name.empty()) player2Name.pop_back();
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
            else if (e.type == SDL_TEXTINPUT) {
                if (currentState == NAME_INPUT) {
                    if (enteringPlayer1) {
                        player1Name += e.text.text;
                    } else {
                        player2Name += e.text.text;
                    }
                }
            }
        }
        
        SDL_RenderClear(renderer);
         if (currentState == MENU) {
            
                SDL_RenderCopy(renderer, backgroundTexture2, NULL, NULL);
                renderMenu(renderer, font, menuItems, selectedItem);
            
        } else if (currentState == NAME_INPUT) {
            string prompt = enteringPlayer1 ? "Enter Player 1 Name:" : "Enter Player 2 Name:";
            string inputText = enteringPlayer1 ? player1Name : player2Name;
            renderNameInput(renderer, font, prompt, inputText);
        }
        else if(currentState==REPLAY)
        {
            renderMenu(renderer, font, playAgainMenuItems, selectedItem);
        }
        else if(currentState==GAME)
        {SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
        afiseazatabla(texture, texture2);
        SDL_Rect destination_rect1 = {265, 170, 40, 40}, destination_rect2 = {265, 220, 40, 40}, destination_rect3{265, 300, 40, 40}, destination_rect4{265, 360, 40, 40};
        string j1 = player1Name, j2 = player2Name;
        string p_alb = to_string(punct_alb);
        string p_negru = to_string(punct_negru);
        int text_width,text_width2,text_width3;
        int text_height,text_height2,text_height3;
        SDL_Texture *scor_alb = renderText(renderer, font, p_alb, textColor, text_width3, text_height3);
        SDL_Texture *scor_negru = renderText(renderer, font, p_negru, textColor, text_width3, text_height3);
        SDL_Texture *j1Texture = renderText(renderer, font, j1, textColor, text_width, text_height);
        SDL_Texture *j2Texture = renderText(renderer, font, j2, textColor, text_width2, text_height2);
        SDL_Texture *j1Texture_rosu = renderText(renderer, font, j1, redColor, text_width, text_height);
        SDL_Texture *j2Texture_rosu = renderText(renderer, font, j2, redColor, text_width2, text_height2);
        SDL_Rect renderQuad = { 20, 490,text_width ,text_height },renderQuad2={480,490,text_width2 ,text_height2};
        SDL_Rect renderQuad3 = {250, 490, text_width3, text_height3};
        SDL_Rect renderQuad4 = {315, 490, text_width3, text_height3};
        SDL_RenderCopy(renderer, scor_alb, NULL, &renderQuad3);
        SDL_RenderCopy(renderer, scor_negru, NULL, &renderQuad4);
        if(tura%2==1)
        {SDL_RenderCopy(renderer, j1Texture_rosu, NULL, &renderQuad);
        SDL_RenderCopy(renderer, j2Texture, NULL, &renderQuad2);
        }
        else 
        {
            SDL_RenderCopy(renderer, j1Texture, NULL, &renderQuad);
        SDL_RenderCopy(renderer, j2Texture_rosu, NULL, &renderQuad2);
        }
        switch (zarr1)
        {
        case 1:
            SDL_RenderCopy(renderer, zar1, NULL, &destination_rect1);
            break;
        case 2:
            SDL_RenderCopy(renderer, zar2, NULL, &destination_rect1);
            break;
        case 3:
            SDL_RenderCopy(renderer, zar3, NULL, &destination_rect1);
            break;
        case 4:
            SDL_RenderCopy(renderer, zar4, NULL, &destination_rect1);
            break;
        case 5:
            SDL_RenderCopy(renderer, zar5, NULL, &destination_rect1);
            break;
        case 6:
            SDL_RenderCopy(renderer, zar6, NULL, &destination_rect1);
            break;
        }
        switch (zarr2)
        {
        case 1:
            SDL_RenderCopy(renderer, zar1, NULL, &destination_rect2);
            break;
        case 2:
            SDL_RenderCopy(renderer, zar2, NULL, &destination_rect2);
            break;
        case 3:
            SDL_RenderCopy(renderer, zar3, NULL, &destination_rect2);
            break;
        case 4:
            SDL_RenderCopy(renderer, zar4, NULL, &destination_rect2);
            break;
        case 5:
            SDL_RenderCopy(renderer, zar5, NULL, &destination_rect2);
            break;
        case 6:
            SDL_RenderCopy(renderer, zar6, NULL, &destination_rect2);
            break;
        }
        if (bar_alb != 0)
            SDL_RenderCopy(renderer, texture, NULL, &destination_rect3);
        if (bar_negru != 0)
            SDL_RenderCopy(renderer, texture2, NULL, &destination_rect4);

     SDL_DestroyTexture(scor_alb);
    SDL_DestroyTexture(scor_negru);
    SDL_DestroyTexture(j1Texture);
    SDL_DestroyTexture(j2Texture);
    SDL_DestroyTexture(j1Texture_rosu);
    SDL_DestroyTexture(j2Texture_rosu);
        }
        else if(currentState==HELP)
        {
            SDL_RenderCopy(renderer, backgroundTexture3, NULL, NULL);
        }

        SDL_RenderPresent(renderer);
            }
    TTF_CloseFont(font);
    font = NULL;
    closeSDL(window, renderer);
    
    return 0;
}
