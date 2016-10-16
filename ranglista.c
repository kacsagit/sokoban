#ifndef ranglista_INCLUDED
#define ranglista_INCLUDED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_gfxPrimitives.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL_ttf.h>
//#include "jatek.c"
#include "main.c"
#if defined(WIN32) || defined(_WIN32)
    #include <windows.h>
#endif

 typedef struct rang{
    unsigned char nev[20];
    int db;
}rang;

typedef unsigned char Bajt;
typedef unsigned short UniKar;

// unicodeból utf8 ba konvertál fájlba írásnál fontos
static void unicode_2_utf8(UniKar const *be, Bajt *ki) { // infoc kód
    int pb, pk;

    pk=0;
    for (pb=0; be[pb]!=0x0000; ++pb) {
        /* 00000000 0xxxxxxx    0x0000-0x007F   0xxxxxxx */
        if (be[pb]<=0x007F)
            ki[pk++] = be[pb];
        else
        /* 00000yyy yyxxxxxx    0x0080-0x07FF   110yyyyy 10xxxxxx */
        if (be[pb]<=0x07FF) {
            ki[pk++] = 0xC0 | be[pb]>>6;     /* 0xC0 = 11000000 */
            ki[pk++] = 0x80 | (be[pb]&0x3F); /* 0x80 = 10000000, 0x3F = 00111111 */
        }
        /* zzzzyyyy yyxxxxxx    0x0800-0xFFFF   1110zzzz 10yyyyyy 10xxxxxx */
        else {
            ki[pk++] = 0xE0 | be[pb]>>12;    /* 0xE0 = 11100000 */
            ki[pk++] = 0x80 | ((be[pb]>>6)&0x3F);
            ki[pk++] = 0x80 | (be[pb]&0x3F);
        }
    }

    ki[pk]=0;
}

//beolvassa az új ranglistára felkerülő nevét   infoc
static int input_text(UniKar *dest, int x, int y, int sz, int m,
               SDL_Color hatter, SDL_Color szin, TTF_Font *font, SDL_Surface *screen) {
    SDL_Rect forras = { 0, 0, sz, m}, cel = { x+10, y, sz, m };
    SDL_Surface *felirat;
    SDL_Event event;
    int hossz, kilep, enter;

    hossz = 0;
    dest[hossz] = 0x0000;   /* lezaro 0 */
    SDL_EnableUNICODE(1);
    enter = 0;
    kilep = 0;
    while (!kilep && !enter) {
        /* szoveg kirajzolasa */
        boxRGBA(screen, x, y, x+sz-1, y+m-1, hatter.r, hatter.g, hatter.b, 255);
        felirat = TTF_RenderUNICODE_Blended(font, dest, szin);
        SDL_BlitSurface(felirat, &forras, screen, &cel);
        SDL_FreeSurface(felirat);
      //  rectangleRGBA(screen, x, y, x+sz-1, y+m-1, 0, 0, 0, 255);
        /* updaterect: mint az sdl_flip, de csak a kepernyo egy darabjat */
        SDL_UpdateRect(screen, x, y, sz, m);

        SDL_WaitEvent(&event);
        switch (event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.unicode) {
                    case 0x0000:
                        /* nincs neki megfelelo karakter (pl. shift gomb) */
                        break;
                    case '\r':
                    case '\n':
                        /* enter: bevitel vege */
                       enter = 1;
                        break;
                    case '\b':
                        /* backspace: torles visszafele, ha van karakter */
                        if (hossz>0)
                            dest[--hossz] = 0x0000;
                        break;
                    default:
                        /* karakter: tombbe vele, plusz lezaro nulla */
                        dest[hossz++] = event.key.keysym.unicode;
                        dest[hossz] = 0x0000;
                        break;
                }
                break;
            case SDL_QUIT:
                /* visszatesszuk a sorba ezt az eventet, mert
                 * sok mindent nem tudunk vele kezdeni */
                SDL_PushEvent(&event);
                kilep = 1;
                break;
        }
    }

    /* 1 jelzi a helyes beolvasast; = ha enter miatt allt meg a ciklus */
    return enter;
}

//kirajzolja a ranglista elemeit
static void kirang(SDL_Surface *screen,SDL_Surface *felirat,SDL_Rect hova, TTF_Font *font,rang tomb,int i,int hanyadik){
    SDL_Color feher = {255, 255, 255}, piros = {207, 89, 54};
    font = TTF_OpenFont("LiberationSerif-Regular.ttf", 25);
    hova.y=100+i*20;
    char konkat[20];
    sprintf(konkat,"%d.",i+1);
    if (hanyadik==i)
        felirat = TTF_RenderUTF8_Blended(font,konkat, feher);
    else
        felirat = TTF_RenderUTF8_Blended(font,konkat, piros);
    hova.x=(screen->w-felirat->w)-350;
    SDL_BlitSurface(felirat, NULL, screen,&hova);
    SDL_FreeSurface(felirat);
    hova.x=60;
    sprintf(konkat,"%s",tomb.nev);
    if (hanyadik==i)
        felirat = TTF_RenderUTF8_Blended(font,konkat, feher);
    else
         felirat = TTF_RenderUTF8_Blended(font,konkat, piros);
    SDL_BlitSurface(felirat, NULL, screen,&hova);
    SDL_FreeSurface(felirat);
    sprintf(konkat,"%d",tomb.db);
    if (hanyadik==i)
        felirat = TTF_RenderUTF8_Blended(font,konkat, feher);
    else
        felirat = TTF_RenderUTF8_Blended(font,konkat, piros);
    hova.x=(screen->w-felirat->w)-30;
    SDL_BlitSurface(felirat, NULL, screen,&hova);
    SDL_FreeSurface(felirat);

}

//kirajzol a képernyõre írt dolgokat
static void rajz(SDL_Surface *screen,SDL_Surface *felirat,SDL_Rect hova, TTF_Font *font,SDL_Color szin,rang *rtomb,int rdb,int *uj,UniKar *szoveg,int hanyadik){
     SDL_Color feher = {255, 255, 255}, piros = {207, 89, 54};
    int i=0;
    SDL_Event event;


    //ha rekordot döntöttek
    if (*uj==2){

        font = TTF_OpenFont("LiberationSerif-Regular.ttf", 40);
        boxColor(screen, 0, 0, screen->w-1, screen->h-1, 0xe9b25bFF);
        felirat = TTF_RenderUTF8_Blended(font, "Ranglista", piros);
        SDL_BlitSurface(felirat, NULL, screen,&hova);
        SDL_FreeSurface(felirat);

        font = TTF_OpenFont("LiberationSerif-Regular.ttf", 25);
        hova.y=350;
        felirat = TTF_RenderUTF8_Blended(font, "Vissza a menübe", szin);
        SDL_BlitSurface(felirat, NULL, screen,&hova);
        SDL_FreeSurface(felirat);

        SDL_Flip(screen);
        font = TTF_OpenFont("LiberationSerif-Regular.ttf", 25);
        felirat = TTF_RenderUTF8_Blended(font, "Gratulálok rekordot döntöttel.", piros);
        hova.y=120;
        hova.x=(screen->w-felirat->w)/2;
        SDL_BlitSurface(felirat, NULL, screen,&hova);
        SDL_FreeSurface(felirat);
        felirat = TTF_RenderUTF8_Blended(font, "Írd be a neved:", piros);
        hova.y=150;
        hova.x=(screen->w-felirat->w)/2;
        SDL_BlitSurface(felirat, NULL, screen,&hova);
        SDL_FreeSurface(felirat);
        SDL_Flip(screen);
        input_text(szoveg, 100, 180, 200, 40, piros, feher, font, screen);
        SDL_Flip(screen);


       /* if (szoveg[0]!=0x0000) {
        felirat = TTF_RenderUNICODE_Blended(font, szoveg, feher);
        hova.x = (screen->w - felirat->w)/2 + 2;
        hova.y = (screen->h - felirat->h)/2 + 2;
        SDL_BlitSurface(felirat, NULL, screen, &hova);
        SDL_FreeSurface(felirat);}

        SDL_Flip(screen);
        while (SDL_WaitEvent(&event) && event.type!=SDL_QUIT)
            ;*/
        }





    // ha nem döntöttek rekordot
    if (*uj==1)   {
         font = TTF_OpenFont("LiberationSerif-Regular.ttf", 40);
        boxColor(screen, 0, 0, screen->w-1, screen->h-1, 0xe9b25bFF);
        felirat = TTF_RenderUTF8_Blended(font, "Ranglista", piros);
        SDL_BlitSurface(felirat, NULL, screen,&hova);
        SDL_FreeSurface(felirat);
        SDL_Flip(screen);
        font = TTF_OpenFont("LiberationSerif-Regular.ttf", 25);
        felirat = TTF_RenderUTF8_Blended(font, "Sajnos nem döntöttél rekordot", piros);
        hova.y=120;
        hova.x=(screen->w-felirat->w)/2;
        SDL_BlitSurface(felirat, NULL, screen,&hova);
        SDL_FreeSurface(felirat);
        SDL_Flip(screen);
        int kilep = 0;
        while (!kilep && *uj) {
            SDL_WaitEvent(&event);
            switch (event.type) {
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_KP_ENTER:
                        case SDLK_RETURN:
                            /* enter: bevitel vege */
                            *uj=0;
                            break;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    *uj=0; break;
                case SDL_QUIT:
                /* visszatesszuk a sorba ezt az eventet, mert
                 * sok mindent nem tudunk vele kezdeni */
                    SDL_PushEvent(&event);
                    kilep = 1;
                break;
        }
    }
    }
    // csak kiírja a ranglistát
    if (*uj==0){
            hova.x=20;
            hova.y=20;
            SDL_Color feher = {255, 255, 255}, piros = {207, 89, 54};
            font = TTF_OpenFont("LiberationSerif-Regular.ttf", 40);
            boxColor(screen, 0, 0, screen->w-1, screen->h-1, 0xe9b25bFF);
            felirat = TTF_RenderUTF8_Blended(font, "Ranglista", piros);
            SDL_BlitSurface(felirat, NULL, screen,&hova);
            SDL_FreeSurface(felirat);


            font = TTF_OpenFont("LiberationSerif-Regular.ttf", 25);
            hova.y=350;
            felirat = TTF_RenderUTF8_Blended(font, "Vissza a menübe", szin);
            SDL_BlitSurface(felirat, NULL, screen,&hova);
            SDL_FreeSurface(felirat);


        for (i=0;i<rdb;i++)
            kirang(screen,felirat,hova,font,rtomb[i],i,hanyadik);
        SDL_Flip(screen);

            }
    }


// beolvassa a ranglistát ha nincs akkor készít egy fájlt
static void beolv(rang *rtomb,int *rdb){
 FILE *fajl;
    char fajlneve[]="rang.txt";
    int c;
    fajl=fopen(fajlneve,"at+");
    while((c = fgetc(fajl)) != EOF){
        if(c == '\n')
            (*rdb)++;
    }
    rewind(fajl);
    int i;
    for (i=0;i<*rdb;i++){
        fscanf(fajl,"%s",&rtomb[i].nev);
        fscanf(fajl,"%d",&rtomb[i].db);
    }
    fclose(fajl);
}





// fõ
void ranglista(int argc, char *argv[]){
    #if defined(WIN32) || defined(_WIN32)
        SetConsoleCP(1250);
        SetConsoleOutputCP(1250);
    #endif

    SDL_Event event;
    SDL_Surface *screen;
    TTF_Font *font;
    SDL_Surface *felirat;
    SDL_TimerID id;
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    screen=SDL_SetVideoMode(400, 400, 0, SDL_ANYFORMAT);
    rang rtomb[10];
    int rdb=0;
    int uj;
    extern int darab;

    if (!screen) {
        fprintf(stderr, "Nem sikerult megnyitni az ablakot!\n");
        exit(1);
    }
    SDL_WM_SetCaption("Ranglista", "Ranglista");

    TTF_Init();
    font = TTF_OpenFont("LiberationSerif-Regular.ttf", 40);
    if (!font) {
        fprintf(stderr, "Nem sikerult megnyitni a fontot! %s\n", TTF_GetError());
        exit(2);
    }
//háttérszín
    boxColor(screen, 0, 0, screen->w-1, screen->h-1, 0xe9b25bFF);
    SDL_Color feher = {255, 255, 255}, piros = {207, 89, 54};
    int hanyadik=-1;
    SDL_Rect hova = { 20, 20, 0, 0};
    UniKar szoveg[20];


    int i,rekord=0;
    beolv(rtomb,&rdb);
    // megnézi rekord e
    if (darab>0)
        for (i=0;i<rdb;i++)
            if (darab<rtomb[i].db)
                rekord++;

    if ((rekord)||((darab)&&(rdb<10))){
        uj=2;
        hanyadik=0;
        }
    else
        if (darab)
            uj=1;

    if (darab==0)
        uj=0;

rajz(screen,felirat,hova,font,piros,rtomb,rdb,&uj,szoveg,hanyadik);

Bajt szovegutf[20];

//rekord esetén
if ((uj==2)&&(darab)){

 FILE *fajl;
    char fajlneve[]="rang.txt";
    int c;
    fajl=fopen(fajlneve,"wt");
    int i;

    if (rdb==10)
        rdb--;
    for (i=0;i<rdb;i++){
        fprintf(fajl,"%s ",rtomb[i].nev);
        fprintf(fajl,"%d\n",rtomb[i].db);
    }





    unicode_2_utf8(szoveg,szovegutf);
    fprintf(fajl,"%s ",szovegutf);

    fprintf(fajl,"%d\n",darab);
    fclose(fajl);

     int  j;
     rdb=0;
    beolv(rtomb,&rdb);
    int k=0,egyezik=0;
   for (i = rdb-1; i > 0; --i)
      for (j = 0; j < i; ++j)
         if (rtomb[j+1].db < rtomb[j].db) {
            rang temp = rtomb[j];
            rtomb[j] = rtomb[j+1];
            rtomb[j+1] = temp;
            }
    // fehérrel írja ki az aktuálisat teszt
        for (j = 0; j < rdb; j++){
            if (rtomb[j].db==darab){
                egyezik=1;
                do{
                    if (rtomb[j].nev[k]!=szovegutf[k])
                        egyezik=0;
                }while (rtomb[j].nev[k++]!='\0');
               if (egyezik) hanyadik=j;
            }
        }

    fajl=fopen(fajlneve,"wt");

    for (i=0;i<rdb;i++){
        fprintf(fajl,"%s ",rtomb[i].nev);
        fprintf(fajl,"%d\n",rtomb[i].db);}
    fclose(fajl);

    darab=0;
    rdb=0;
    uj=0;
    beolv(rtomb,&rdb);

    }


    rajz(screen,felirat,hova,font,piros,rtomb,rdb,&uj,szoveg,hanyadik);
    SDL_Flip(screen);

    int quit=0;

    //gomb kezelése
    while (!quit)
        while( SDL_PollEvent( &event ) ){
            switch( event.type ){
                case SDL_MOUSEMOTION:
                            if ((event.motion.x>20)&&(event.motion.y>350)&&(event.motion.x<250)&&(event.motion.y<380))
                                   rajz(screen,felirat,hova,font,feher,rtomb,rdb,&uj,szoveg,hanyadik);
                            else
                                    rajz(screen,felirat,hova,font,piros,rtomb,rdb,&uj,szoveg,hanyadik);
                            break;
                case SDL_MOUSEBUTTONDOWN:{
                        if (event.button.button == SDL_BUTTON_LEFT) {
                                if ((event.button.x>20)&&(event.button.y>350)&&(event.button.x<250)&&(event.button.y<380))
                                    {main(&argc, argv); return 0;}}} break;
                case SDL_KEYDOWN:
                        switch( event.key.keysym.sym ){
                            case SDLK_BACKSPACE: {main(&argc, argv); return 0;} break;
                            case SDLK_ESCAPE: {main(&argc, argv); return 0;} break;
                            }break;
                case SDL_QUIT: quit = 1; break;

            }
        SDL_Flip(screen);
        }

    TTF_CloseFont(font);

    SDL_Quit();
}
#endif


