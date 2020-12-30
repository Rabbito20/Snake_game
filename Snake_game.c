#include<stdio.h>
#include<windows.h>
#include<stdlib.h>
#include<math.h>
#include<conio.h>   //  Console input/output    //  Za _kbhit() i _getch()
#include<time.h>    //  Za time() u RandomFrog()

//  Definisali smo velicinu matrice i naseg polja za igru
#define N 20    //  Broj redova
#define M 50    //  Broj kolona

//  Globalne promenljive
int i, j, Field[N][M], x, y, Gy, head, tail, Game = 0, Frogs, a, b, var, dir, tailDec = 2;
int score, HighScore, speed;

FILE *f;

/**
Field[N][M] je matrica u kojoj se igra odvija, postavili smo da joj velicine budu
vec definisani N i M, tako da nam Zmija nece izlaziti van zaglavlja.

x i y su koordinate pocetne pozicije nase zmije.

Head i Tail su glava i rep.

Gy - dodali smo ovo, jer iz nekog razloga postoji neki bag pri iniciranju,
sve ostalo radi normalno.

Game - nam signalizira kraj igre kad postane 1.

Frogs - hrana za zmiju koja ce se pojavljivati na random mestima.

a i b - Sadrze podatke o poziciji zabice.

var - pamti vrednost sa tastature, u sustini smer u kom ce se zmija kretati.

dir - zadrzava pravac do ponovnog pritiska.

tailDec - za koliko nam se povecava rep, tj. zmija.

score - Da znamo koliko smo zabica pojeli.

HighScore - "To be the best you've gotta beat the best!"

f - Za fajl u kom cemo cuvati HighScore

speed - brzina osvezavanja i kretanja zmije
*/

void RandomFrog();
int snake_initialization();
void print();
void reset_screen_poition();
int getch_noblock();
int movement();
void tail_remove();
void Game_over();
void Game_over_check();
void scoreIncrease();

//  INSTANCIRA POCETNU LOKACIJU I VELICINU ZMIJE
int snake_initialization()
{
    //  Pri pokretanju, zelimo da otvorimo fajl da iscitamo HighScore od ranije
    f = fopen("./SnakeGame_HighestOfScores.txt", "r");
    fscanf(f, "%d", &HighScore);
    fclose(f);

    //  Ispunjava celu matricu sa nulama
    for(i = 0; i < N; i++) {
        for(j = 0; j < M; j++) {
            Field[i][j] = 0;
        }
    }
    score = 0;

    //  Kako bi zmija pocela u centru
    //  Delimo M i N sa 2
    x = N/2; y = M/2;
    Gy = y;
    head = 5; tail = 1;
    Frogs = 0; Game = 0;
    dir = 'd';    //  Kako bi se pri pokretanju programa pokrenula i zmija

    for(i = 0; i < head; i++) {
        Gy++;
        Field[x][Gy-head] = i+1;
    }

    return 0;
}

void scoreIncrease()
{
    score += 5;
}

//  Daje nam neku vrednost nasumicno i
void RandomFrog()
{
    //  Uzima vreme od pocetka programa kao promenljivu
    //  I pretvara je u random vrednost
    srand(time(0));

/**
    Uzeli smo vrednosti 18 i 58, jer nam je velicina matrice u kojoj se
    zmija krece 20 i 60
*/
    a = 1 + rand() % 18;
    b = 1 + rand() % 38;
    if(Frogs == 0 && Field[a][b] == 0) {
        Field[a][b] = -1;
        Frogs = 1;

        //  Random ubrzava kad pojedemo zabicu
        if(speed > 10 && score != 0) speed = speed - 5;
    }
}
///////////////////////////////////////////////////////////////////////////////
///  CRTA ZAGLAVLJE U KOM SE IGRA ODVIJA

//  Igra se odvija unutar matrice
void print()
{
    /**

    //  Koriscene ASCII vrednosti za zaglavlje: 186, 187, 188, 200, 201, 205
    */

    //  Kreiramo gornje ivice programa
    printf("%c", 201);      //  Gornja leva ivica
    for(i = 0; i <= M; i++) {
        if(i > 0) {
            printf("%c", 205);} //  Prava linija
        if(i == M){
            printf("%c", 187);} //  Gornja desna ivica
    }
    printf("\tCurrent score: %d", score);
    printf(" High Score: %d", HighScore);
    printf("\n");

    //  Kreiramo ivice sa leve i desne strane
    for(i = 0; i < N; i++) {
        printf("%c", 186);        //  Crta vertikalnu liniju na pocetku
        for(j = 0; j < M; j++){
            //  Sve ostalo izmedju je prazno
            if (Field[i][j] == 0) {printf(" ");}

            //  Crta zmiju unutar polja
            if(Field[i][j] > 0 && Field[i][j] != head){
                printf("%c", 176);}
            if(Field[i][j] == head){
                //  Crta glavu zmije
                printf("%c", 178);} //  Alternativa je ascii vrednost 254

            //  Stampa zabicu na random poziciji
            if(Field[i][j] == -1) {printf("%c",  15);}

            //  Jos jedna vertikalna linija samo na kraju reda
            if(j == M-1) {printf("%c\n", 186);}

        }
    }
    //  Kreiramo donju ivicu
    printf("%c", 200);      //  Donja leva ivica
    for(i = 0; i <= M; i++) {
        if(i > 0) {
            printf("%c", 205);} //  Prava linija
        if(i == M){
            printf("%c", 188);} //  Donja desna ivica
    }
    printf("\n");
}

void reset_screen_poition()
{
    HANDLE hOut;
    COORD Position; //  Koordinate
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);  //  "Hendluje" standardni ulaz programa
    //  Postavljamo ivice naseg ekrana
    Position.X = 0;
    Position.Y = 0;

    //  Svaki put kad se ekran resetuje,
    //  postavlja kursor na pocetak, tj gornji levi ugao
    SetConsoleCursorPosition(hOut, Position);

    //  Tako ce izgledati kao da se ivice ne stampaju iznova
    print();
}
///////////////////////////////////////////////////////////////////////////////
///  KOMAND I KRETANJE ZMIJE
//  Zelimo da se igra krece non stop
int getch_noblock()
{
    //  Ako smo pritisnuli taster neki
    //  Vraca tu komandu
    if(_kbhit()){
        return _getch();
    } else
        return -1;  //  U suprotnom ne prestaje sa radom

    return -1;
}

/**
1.  Dodali smo komande 'w', 'a', 's' i 'd'.
2.  Dodali smo da zadrzi smer i da se stalno krece.
3.  Resili problem gde se program "crashovao", i izlazak izvan okvira.
4.  Dodali da rep zmije prati glavu.
5.  Dodali osvezavanje novih zabica kad se pojedu.
6.  Dodali smo funkciju za game over.
*/

int movement()
{
    var = getch_noblock();
    var = tolower(var);

    //  Ukoliko nista nije pritisnuto, zadrzava pravac
    //  Ako je pritisnut suprotan smer, onda se smer ne menja,
    //  zato je dodat ovaj uskov na kraju
    if(((var == 'd' || var == 'a') || (var == 's' || var == 'w'))
       && (abs(dir - var) > 5)) dir = var;

    //  Kad se krecemo povecavamo poziciju glave za 1 i smanjujemo rep za 1
    //  Pravac zadrzavamo tako sto pamtimo vrednost dir i dok se ne promeni
    //  ta vrednost, zmija se stalno krece
    if(dir == 'd') {    //  DESNO
        y++;
        Game_over_check();  //  Provera da nismo ujeli sebe

        //  Ako stigne do kraja reda sa desne strane, ne prelazi u novi
        //  Vec se pojavljuje na drugoj strani u istom
        if(y == M-1){y = 0;}

        //  Ako smo pojeli zabicu
        if(Field[x][y] == -1) {
            Frogs = 0;
            tail -= tailDec;    //  Povecava zmiju
            scoreIncrease();
        }

        head++;
        Field[x][y] = head;
    }

    if(dir == 'a') {    //  LEVO
        y--;
        Game_over_check();  //  Provera da nismo ujeli sebe

        //  Ako stigne do kraja reda sa leve strane, ne puca program
        //  Vec se pojavljuje odozdo u istom
        if(y == -1){y = M-1;}

        //  Ako smo pojeli zabicu
        if(Field[x][y] == -1) {
            Frogs = 0;
            tail -= tailDec;    //  Povecava zmiju
            scoreIncrease();
        }

         head++;
        Field[x][y] = head;
    }

    if(dir == 'w') {    //  GORE
        x--;

        //  Ako stigne do kraja kolone na gore, ne prelazi u novu,
        //  vec se pojavljuje na drugoj strani u istoj koloni
        if(x == -1){x = N-1;}
        Game_over_check();  //  Provera da nismo ujeli sebe

        //  Ako smo pojeli zabicu
        if(Field[x][y] == -1) {
            Frogs = 0;
            tail -= tailDec;    //  Povecava zmiju
            scoreIncrease();
        }

         head++;
        Field[x][y] = head;
    }

    if(dir == 's') {    //  DOLE
        x++;

        //  Ako stigne do kraja kolone dole, ne prelazi u novu,
        //  vec se pojavljuje na drugoj strani u istoj koloni
        if(x == N-1){x = 0;}
        Game_over_check();  //  Provera da nismo ujeli sebe

        //  Ako smo pojeli zabicu
        if(Field[x][y] == -1) {
            Frogs = 0;
            tail -= tailDec;    //  Povecava zmiju
            scoreIncrease();
        }

         head++;
        Field[x][y] = head;
    }

    return dir;
}

//  Brise ostatak repa pri kretanju zmije
void tail_remove()
{
    for(i = 0; i < N; i++) {
        for(j = 0; j < M; j++) {
            if(Field[i][j] == tail) {
                Field[i][j] = 0;
            }
        }
    }
    tail++;
}
///////////////////////////////////////////////////////////////////////////////
/// GAME OVER
void Game_over()
{
    printf("\a");   //  "Beep" obavestenje
    Sleep(1500);    //  Zadrzace malo sliku
    system("Cls");  //  Komanda "Cls", Cisti sve sto je trenutno na ekranu

    //  Dodajemo HighScore, ako je score veci od postojaceg u fajl.
    if(score > HighScore) {
        //printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
        printf("\t\t\t\tNEW HIGH SCORE ACHIVED!\n");
        system("pause");
        f = fopen("./SnakeGame_HighestOfScores.txt", "w");
        fprintf(f, "%d", score);
        fclose(f);
        //printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
        system("Cls");
    }

    printf("\n\n\n\n\n\n\n\n\n\n\n\n");
    printf("\t\t\t!!!!!!!!!!!GAME OVER!!!!!!!!!!!\n\n");
    printf("\n\t\t\tWELL DONE! YOU SCORED %d POINTS!\n\n", score);
    printf("\t\t\tPres ENTER to play again or ESC to exit...\n" );
    printf("\n\n\n\n\n\n\n\n\n\n\n\n");

    //  Dajemo opciju ENTER = 13 ili ESC = 27
    while(1)
    {
        var = getch_noblock();
        if(var == 13) {   //  Ne radi dobro!
            Game = 0;
            snake_initialization();
            continue;
        } else if(var == 27) {
            Game = 1;
            break;
        }
    }
    //system("pause");
}

void Game_over_check()
{
    //  Ako smo se ugrizli, tj. ako smo zasli u polje gde smo mi, onda zove Game_over()
    if(Field[x][y] != 0 && Field[x][y] != -1)
        Game_over();
}

///////////////////////////////////////////////////////////////////////////////
int main()
{
    snake_initialization(); //  Inicira pocetnu lokaciju i velicinu zmije
    print();        //  Pozvali smo funkciju da nam iscrta ivice ekrana

    //OVDE POCINJE MAGIJA
    //  Igra se krece dokle god je Game == 0
    while(Game == 0)
    {
        reset_screen_poition(); //  Iscrtava zaglavlje ekrana
        RandomFrog();   //  Poziva random zabice
        //print();

        //  Kretanje
        movement();
        tail_remove();

        Sleep(speed);      //  Ovo zamrzne ekran na odredjen broj milisekundi
        //Game = 0;
    }

    return (0);
}
