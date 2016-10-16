
#ifndef jatek_INCLUDED
#define jatek_INCLUDED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_gfxPrimitives.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL_ttf.h>
#include "main.c"
#include "ranglista.c"
/* mezon allo figura. ugyanolyan sorrendben vannak, mint a kepen,
 * igy a kapott egesz szamok megegyeznek a png-beli indexekkel */
typedef enum Tabla {
    Ures = -1,
    fal, doboz, ember,celpont
} Tabla;
 enum { MERET = 46, KERET = 30 };

int darab=0;


// beolvassa a pályát fájlból
static Tabla **uj_allas( int *sor,int *oszlop, int *ex, int *ey,int *celdb) {
   FILE *fajl;
    char fajlneve[]="beol.txt";
    int c;
    int db=0;
    Tabla **tomb;
    fajl=fopen(fajlneve,"rt");
    *sor=0;
    *oszlop=0;
    while ((c=fgetc(fajl)) != EOF){
        if (c=='\n')
            ++*sor;
        else
            if (c!=' ')
                db++;

        }
    *oszlop=db/(*sor);
   tomb=(Tabla**) malloc(*sor*sizeof(Tabla*));
    *tomb=(Tabla*) malloc(db*sizeof(Tabla));
    int i,j;
    for (i=0;i<*sor;i++)
       tomb[i]=*tomb+(*oszlop*i);

    rewind(fajl);

   i=0, j=0;
   *celdb=0;
    while ((c=fgetc(fajl)) != EOF){
        if (c=='\n'){
            i++;
            j=0;
        }
        else
            if (c!=' '){
                switch (c){
                    case 'd': tomb[i][j++]=doboz; break;
                    case 'e': {tomb[i][j]=ember;
                        *ex=i; *ey=j; j++;}break;
                    case 'u': tomb[i][j++]=Ures; break;
                    case 'f': tomb[i][j++]=fal;  break;
                    case 'c': {tomb[i][j++]=celpont; (*celdb)++;}; break;
            }
        }
    }

    fclose(fajl);
    return *tomb;
}


static void mozdul(size_t oszlop, Tabla tabla[][oszlop],int *x,int *y,int *le,int x1,int y1,int *darab){

    if ((!*le)&&(tabla[*x+x1][*y+y1]==doboz)) //nem lenne reuturn újra léptetné az embert akövetkezõ if
       if ((tabla[*x+2*x1][*y+2*y1]==Ures)||(tabla[*x+2*x1][*y+2*y1]==celpont)){
            tabla[*x][*y]=Ures;
            *x+=x1;
            *y+=y1;
            tabla[*x][*y]=ember;
            tabla[*x+x1][*y+y1]=doboz;
            *le=1;
            ++*darab;
            return;
        }

     if ((!*le)&&((tabla[*x+x1][*y+y1]==Ures)||(tabla[*x+x1][*y+y1]==celpont))){
        tabla[*x][*y]=Ures;
        *x+=x1;
        *y+=y1;
        tabla[*x][*y]=ember;
        *le=1;
        ++*darab;
        }

    *le=1;

}

/* kiszamolja, hogy milyen koordinatan van a kepernyon az adott mezo */
int palyapos(int koord) {
    return MERET*koord + KERET;
}

/* kirajzol egy mezot; a forras a betoltott png, a cel nevu kepre rajzol.
 * melyik babut, milyen koordinatakra: melyik, x, y. */
static void mezo_rajzol(int sor,int oszlop,SDL_Surface *forraskep, SDL_Surface *celkep, Tabla melyik, int x, int y,Tabla cel[][oszlop],Tabla tabla[][oszlop],int celdb,int **darab,int **nyert) {
    char lep[20];
    sprintf(lep, "Lepesek: %d",**darab);
    stringRGBA(celkep, 310, 10,lep, 0, 0, 0, 255);

    /* a forras kepbol ezekrol a koordinatakrol, ilyen meretu reszletet masolunk. */
    SDL_Rect src = { 10+(melyik%3)*60+3, 10+(melyik/3)*60+3, MERET, MERET };
    /* a cel kepre, ezekre a koordinatakra masoljuk. (0, 0 lenne a meret, de az nem szamit,
     * a masolando kepreszlet meretet az elozo struct adja meg. */
    SDL_Rect dest = { x*MERET + KERET, y*MERET + KERET, 0, 0 };

    /* mezo alapszine */
    if (cel[y][x]==celpont)
        boxColor(celkep, palyapos(x), palyapos(y), palyapos(x+1)-1, palyapos(y+1)-1, 0xe9b25bFF);
        else
        boxColor(celkep, palyapos(x), palyapos(y), palyapos(x+1)-1, palyapos(y+1)-1, 0xFFFF99FF);
    rectangleColor(celkep, palyapos(x), palyapos(y), palyapos(x+1), palyapos(y+1), 0x00000080);
    stringRGBA(celkep, 40, 10,"Menu", 0, 0, 0, 255);
    stringRGBA(celkep, 100, 10,"Ujra", 0, 0, 0, 255);
    int a,b,db=0;
    for (a=0;a<sor;a++)
        for(b=0;b<oszlop;b++)
            if ((cel[a][b]==celpont)&&(tabla[a][b]==doboz))
               db++;

    if (db==celdb){
        **nyert=1;}
       // stringRGBA(celkep, 170, 10, "Nyert", 0, 0, 0, 255);

    if (melyik==Ures)
        return;

    /* kepreszlet masolasa */
    SDL_BlitSurface(forraskep, &src, celkep, &dest);
}

/* kirajzolja az egesz tablat. forraskep a betoltott png, celkep ahova rajzol. */
static void tabla_rajzol(int sor,int oszlop,Tabla tabla[][oszlop], SDL_Surface *forraskep, SDL_Surface *celkep,int *darab,Tabla cel[][oszlop],int celdb,int *nyert) {
    int x, y;
    /* az egeszet kitolti */
    boxColor(celkep, 0, 0, celkep->w-1, celkep->h-1, 0xFFFF99FF);
    // keret:  rectangleColor(celkep, palyapos(0)-1, palyapos(0)-1, palyapos(8), palyapos(8), 0x00000080);
    /* kirajzolja a mezoket */
    for (x=0; x<sor; x++)
        for (y=0; y<oszlop; y++)
            mezo_rajzol(sor,oszlop,forraskep, celkep, tabla[x][y], y, x,cel,tabla,celdb,&darab,&nyert);
}




void sokoban(int argc, char *argv[]) {
int nyert=0;
SDL_Surface *screen;
SDL_Surface *babuk;
Tabla **tabla;
SDL_Event event;
int celdb=0;
SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
darab=0;

    if (!screen) {
        fprintf(stderr, "Nem sikerult megnyitni az ablakot!\n");
        exit(1);
    }
    SDL_WM_SetCaption("Sokoban", "sokoban");

    /* kep betoltese */
    babuk = IMG_Load("sokoban.png");
    if (!babuk) {
        fprintf(stderr, "Nem sikerult betolteni a kepfajlt!\n");
        exit(2);
    }

    int sor,oszlop,ex,ey;
    /* uj allas letrehozasa es kirajzolasa */
    tabla=uj_allas(&sor,&oszlop,&ex,&ey,&celdb);
    Tabla **cel;
    cel=uj_allas(&sor,&oszlop,&ex,&ey,&celdb);
    screen=SDL_SetVideoMode(MERET*oszlop+KERET*2, MERET*sor+KERET*2, 0, SDL_ANYFORMAT);

    tabla_rajzol(sor,oszlop,tabla, babuk, screen,&darab,cel,celdb,&nyert);
   SDL_Flip(screen);
int quit=0,x=ex,y=ey,le;

 //csak egyet mozdul
while (!quit&&!nyert)
    while( SDL_PollEvent( &event ) ){
        le=0;
        switch( event.type ){
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if ((event.button.x>40)&&(event.button.y>10)&&(event.button.x<70)&&(event.button.y<20))
                       {darab=0; main(&argc, argv);
                       return 0;};
                    if ((event.button.x>100)&&(event.button.y>10)&&(event.button.x<130)&&(event.button.y<20)){
                       tabla=uj_allas(&sor,&oszlop,&ex,&ey,&celdb);
                       darab=0; x=ex; y=ey;
                       tabla_rajzol(sor,oszlop,tabla, babuk, screen,&darab,cel,celdb,&nyert);
                       }
                }break;
            case SDL_KEYDOWN:
                switch( event.key.keysym.sym ){
                    case SDLK_DOWN:{
                        mozdul(oszlop,tabla,&x,&y,&le,1,0,&darab);
                        tabla_rajzol(sor,oszlop,tabla, babuk, screen,&darab,cel,celdb,&nyert);
                        }
                        break;
                     case SDLK_UP:{
                        mozdul(oszlop,tabla,&x,&y,&le,-1,0,&darab);
                        tabla_rajzol(sor,oszlop,tabla, babuk, screen,&darab,cel,celdb,&nyert);
                        }
                        break;
                     case SDLK_LEFT:{
                        mozdul(oszlop,tabla,&x,&y,&le,0,-1,&darab);
                        tabla_rajzol(sor,oszlop,tabla, babuk, screen,&darab,cel,celdb,&nyert);
                        }
                        break;
                      case SDLK_RIGHT:{
                        mozdul(oszlop,tabla,&x,&y,&le,0,1,&darab);
                        tabla_rajzol(sor,oszlop,tabla, babuk, screen,&darab,cel,celdb,&nyert);
                        }
                        break;
                      case SDLK_BACKSPACE: {darab=0; main(&argc, argv); return 0;} break;
                      case SDLK_ESCAPE: {darab=0; main(&argc, argv); return 0;} break;
                      case SDLK_KP_ENTER:{
                            tabla=uj_allas(&sor,&oszlop,&ex,&ey,&celdb);
                            darab=0; x=ex; y=ey;
                            tabla_rajzol(sor,oszlop,tabla, babuk, screen,&darab,cel,celdb,&nyert);
                            }break;
                      case SDLK_RETURN:{
                            tabla=uj_allas(&sor,&oszlop,&ex,&ey,&celdb);
                            darab=0; x=ex; y=ey;
                            tabla_rajzol(sor,oszlop,tabla, babuk, screen,&darab,cel,celdb,&nyert);
                            }break;
                      default:break;
                  }
            case SDL_KEYUP:le=0; break;
            case SDL_QUIT: quit = 1; break;
        }
        SDL_Flip(screen);
    }
    /* nincs mar ra szukseg: felszabaditjuk a memoriat */
    SDL_FreeSurface(babuk);
    if (nyert)
        {ranglista(&argc, argv); return 0;}
    free(*tabla);
    free(tabla);
     /* idozito torlese */
//    SDL_RemoveTimer(id);
    SDL_Quit();

return 0;
}
#endif

