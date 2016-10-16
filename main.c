#ifndef main_INCLUDED
#define main_INCLUDED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_gfxPrimitives.h>
#include <math.h>
#include <stdio.h>


#include <SDL_ttf.h>

//#include "jatek.c"
#include "ranglista.c"
#if defined(WIN32) || defined(_WIN32)
    #include <windows.h>
#endif
// Jóna Kata IP1LXW sokoban

////////////////////////////////////////////////////////////////////////
// A játékban dobozokat lehet tolni. A jaték célja, hogy a dobozokat  //
// betoljuk a megfelelő helyre a lehető legkevesebb lépésből.         //
// A figura mozgatása a nyíl billentyűkkel lehetséges.                //
////////////////////////////////////////////////////////////////////////


 enum { MERET2 = 60, KERET2 = 71 };

/* mezon allo figura. ugyanolyan sorrendben vannak, mint a kepen,
 * igy a kapott egesz szamok megegyeznek a png-beli indexekkel */
int doboz2=1;
int tabla2[3][1];

/* uj allassal tolti ki a parameterkent kapott tabla2t */
static void uj_allas2(int *tabla2[][1]) {
    int x, y;

    for (y=0; y<3; y++)
        for (x=0; x<1; x++)
            tabla2[y][x]=0;
    tabla2[0][0]=doboz2;
}

//doboz mozdulását irányítja
static void mozdul2(int *tabla2[][1],int *x,int *y,int *le,int x1,int y1){
        tabla2[(3+*x-1)%3][*y]=-1;
        tabla2[*x%3][*y]=-1;
        *x=(3+x1+*x)%3;
        *y=(3+y1+*y)%3;
        tabla2[*x%3][*y]=doboz2;
        *le=1;
}

//kirajzolja a mezőt
static void mezo_rajzol2(SDL_Surface *forraskep, SDL_Surface *celkep,int elem, int x, int y,int *tabla2[][8]) {
    /* a forras kepbol ezekrol a koordinatakrol, ilyen MERET2u reszletet masolunk. */

    SDL_Rect src = { 0, 0, MERET2, MERET2 };
    /* a cel kepre, ezekre a koordinatakra masoljuk. (0, 0 lenne a MERET2, de az nem szamit,
     * a masolando kepreszlet MERET2et az elozo struct adja meg. */

    SDL_Rect dest = { x*MERET2 + KERET2, y*MERET2 + KERET2, 0, 0 };

    if (elem!=doboz2)
        return;

    /* kepreszlet masolasa */
    SDL_BlitSurface(forraskep, &src, celkep, &dest);
}

/* kirajzolja az egesz tabla2t. forraskep a betoltott png, celkep ahova rajzol. */
static void tabla2_rajzol(int *tabla2[][1], SDL_Surface *forraskep, SDL_Surface *celkep,SDL_Surface *felirat,SDL_Rect hova, TTF_Font *font) {
    int x, y;
    SDL_Color feher = {255, 255, 255}, piros = {207, 89, 54};
    boxColor(celkep, 0, 0, celkep->w-1, celkep->h-1, 0xe9b25bFF);
   //  boxColor(screen, 0, 0, 400, 400  , 0xe9b25bFF);
   //ha ráállok más színűlesz a menüpont
    if (tabla2[0][0]==doboz2){
        /* felirat megrajzolasa */
        felirat = TTF_RenderUTF8_Blended(font, "Játék", feher);
        }
    else{
       /* felirat megrajzolasa */
        felirat = TTF_RenderUTF8_Blended(font, "Játék", piros);
        }
    /* felirat kep masolasa a kepernyore */
    hova.x = (celkep->w-felirat->w)/2;
    hova.y = 60;
    SDL_BlitSurface(felirat, NULL, celkep, &hova);
    /* a feliratot tartalmazo kepre nincs mar szukseg */
    SDL_FreeSurface(felirat);

    if (tabla2[1][0]==doboz2){
        felirat = TTF_RenderUTF8_Blended(font, "Ranglista", feher);
        }
    else
        {
        felirat = TTF_RenderUTF8_Blended(font, "Ranglista", piros);
        }
    hova.x = (celkep->w-felirat->w)/2;
    hova.y += 60;
    SDL_BlitSurface(felirat, NULL, celkep, &hova);
    SDL_FreeSurface(felirat);

    if (tabla2[2][0]==doboz2){
        felirat = TTF_RenderUTF8_Blended(font, "Kilépés", feher);
        }
    else {
        felirat = TTF_RenderUTF8_Blended(font, "Kilépés", piros);
    }
    hova.x = (celkep->w-felirat->w)/2;
    hova.y += 60;
    SDL_BlitSurface(felirat, NULL, celkep, &hova);
    SDL_FreeSurface(felirat);


    /* kirajzolja a mezoket */
    for (y=0; y<3; y++)
        for (x=0; x<1; x++)
            mezo_rajzol2(forraskep, celkep, tabla2[y][x], x, y,tabla2);
}


//int darab=0;
int main(int argc, char *argv[]) {
    #if defined(WIN32) || defined(_WIN32)
        SetConsoleCP(1250);
        SetConsoleOutputCP(1250);
    #endif
    SDL_Rect hova = { 0, 0, 0, 0 };
    SDL_Event event;
    SDL_Surface *screen;
    TTF_Font *font;
    SDL_Surface *felirat;
    SDL_Surface *dob;
    SDL_TimerID id;
    int i;
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    screen=SDL_SetVideoMode(400, 400, 0, SDL_ANYFORMAT);


    if (!screen) {
        fprintf(stderr, "Nem sikerult megnyitni az ablakot!\n");
        exit(1);
    }
    SDL_WM_SetCaption("Menu", "Menü");

    /* kep betoltese */
    dob = IMG_Load("doboz.png");
    if (!dob) {
        fprintf(stderr, "Nem sikerult betolteni a kepfajlt!\n");
        exit(2);
    }

    /* betutipus betoltese, 40 pont magassaggal */
    TTF_Init();
    font = TTF_OpenFont("LiberationSerif-Regular.ttf", 40);
    if (!font) {
        fprintf(stderr, "Nem sikerult megnyitni a fontot! %s\n", TTF_GetError());
        exit(2);
    }


    //kirajzolja a tablat
    uj_allas2(tabla2);
    tabla2_rajzol(tabla2, dob, screen,felirat,hova,font);
    SDL_Flip(screen);
    int quit=0,x=0,y=0,le;

    while (!quit)
        while( SDL_PollEvent( &event ) ){
            le=0;
            switch( event.type ){
                case SDL_MOUSEMOTION:{
                            if ((event.motion.x>80)&&(event.motion.y>40)&&(event.motion.x<300)&&(event.motion.y<100)){
                                    x=2; //olyan mintha a 3. menupontrol mennénk az elsore
                                    mozdul2(tabla2,&x,&y,&le,1,0);
                                    tabla2_rajzol(tabla2, dob, screen,felirat,hova,font);
                                    }
                            if ((event.motion.x>80)&&(event.motion.y>140)&&(event.motion.x<300)&&(event.motion.y<180)){
                                    x=0; //olyan mintha a 1. menupontrol mennénk az 2.
                                    mozdul2(tabla2,&x,&y,&le,1,0);
                                    tabla2_rajzol(tabla2, dob, screen,felirat,hova,font);
                                    }
                            if ((event.motion.x>80)&&(event.motion.y>200)&&(event.motion.x<300)&&(event.motion.y<240)){
                                    x=1; //olyan mintha a 2. menupontrol mennénk az 3.
                                    mozdul2(tabla2,&x,&y,&le,1,0);
                                    tabla2_rajzol(tabla2, dob, screen,felirat,hova,font);
                                    }}break;
            case SDL_MOUSEBUTTONDOWN:{
                        // bal egérgombra megnyitja az adott fájlt
                         if (event.button.button == SDL_BUTTON_LEFT) {
                                if ((event.button.x>80)&&(event.button.y>40)&&(event.button.x<300)&&(event.button.y<100))
                                    {sokoban(&argc, argv); return 0;}
                                if ((event.button.x>80)&&(event.button.y>140)&&(event.button.x<300)&&(event.button.y<180))
                                    {ranglista(&argc, argv); return 0;}
                                if ((event.button.x>80)&&(event.button.y>200)&&(event.button.x<300)&&(event.button.y<240))
                                    {quit=1;}
                            }}break;

            case SDL_KEYDOWN:
                switch( event.key.keysym.sym ){
                    // a dobozt mozgatja és, hogy melyik menüponton állunk
                    case SDLK_DOWN:{
                        mozdul2(tabla2,&x,&y,&le,1,0);
                        tabla2_rajzol(tabla2, dob, screen,felirat,hova,font);
                        }
                        break;
                     case SDLK_UP:{
                        mozdul2(tabla2,&x,&y,&le,-1,0);
                         tabla2_rajzol(tabla2, dob, screen,felirat,hova,font);
                        }
                        break;
                    //enterre megnyitja a menüpontot
                    case SDLK_KP_ENTER:
                        switch(x){
                           case 0: { sokoban(&argc, argv); return 0;}
                           case 1: { ranglista(&argc, argv); return 0;}
                           case 2: quit = 1; break;
                        }; break;
                     case SDLK_RETURN:
                        switch(x){
                           case 0: { sokoban(&argc, argv); return 0;}
                           case 1: { ranglista(&argc, argv); return 0;}
                           case 2: quit = 1; break;
                        }; break;
                    //kilép a játékból esc re vagy backshapere
                     case SDLK_BACKSPACE: quit = 1; break;
                     case SDLK_ESCAPE: quit = 1; break;
                     default:break;
                  }break;
            case SDL_KEYUP:le=0; break;
            case SDL_QUIT: quit = 1; break;
        }
        SDL_Flip(screen);
    }

     /* nem kell tobbe */
    TTF_CloseFont(font);

    SDL_Quit();

    return 0;
}

#endif


