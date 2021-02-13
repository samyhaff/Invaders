/* 
 BUG missile fonctionnel que si counter_invader == 0
 BUG missile traverse des couches
 */

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <time.h>

#define WIDTH 80
#define HEIGHT 24
#define FPS 50
#define MISSILE_FREQ 250
#define INVADERS_FREQ 500
#define NB_INVADERS 36

enum etat {ALIVE, DYING, DEAD};

bool playing = true;
int counter_missile = 0;
int counter_invader = 0;
bool going_left = true;

struct player {
    int position_x;
    int position_y;
    int lives;
    int fired;
    int missile_x;
    int missile_y;
    char *sprite;
    char *missile_sprite;
} player = {WIDTH / 2, HEIGHT - 2, 3, false, 0, 0, "|-^-|", "|"};

struct invader {
    int position_x;
    int position_y;
    char *sprite;
    int state;
    int score;
} invaders[36];

void spawnInvaders() {
    for (int i = 0; i < 36; i++) {
        invaders[i].position_x = (WIDTH / 2) - 7 + ((3 * i) % 18);   
        invaders[i].position_y = 2 + (i / 6);   
        invaders[i].sprite = "/O\\";
        invaders[i].state = ALIVE;
        invaders[i].score = 5;
    }
}

void getAndRunInput() {
    int ch = getch();
    switch (ch) {
        case 'j':
            player.position_x -= 1;
            break;
        case 'k':
            player.position_x += 1;
            break;
        case 'q':
            playing = false;
            break;
        case 'l':
            if (!player.fired) {
                player.fired = true;
                player.missile_x = player.position_x;
                player.missile_y = player.position_y;
            }
            break;
        default: 
            break;
    }
}

bool hit() {
    for (int i = NB_INVADERS - 1; i >= 0; i--) {
        if ((invaders[i].state == ALIVE) && (invaders[i].position_x == player.missile_x) && (invaders[i].position_y == player.missile_y)) {
            invaders[i].state = DYING;
            return true;
        }
    }
    return false;
}

void updateMissile() {
    if (player.fired) {
        if (counter_missile == 0)
            player.missile_y -= 1;
        if (player.missile_y == 0)
            player.fired = false;
       if (hit())
           player.fired = false;
        counter_missile = (counter_missile + 1) % MISSILE_FREQ;
    }
}

int getMinInvaderX() {
    int min = WIDTH;
    for (int i = 0; i < NB_INVADERS; i++) {
        if ((invaders[i].position_x < min) && (invaders[i].state == ALIVE))
            min = invaders[i].position_x;
    }
    return min;
}

int getMaxInvaderX() {
    int max = -1;
    for (int i = 0; i < NB_INVADERS; i++) {
        if ((invaders[i].position_x > max) && (invaders[i].state == ALIVE))
            max = invaders[i].position_x;
    }
    return max;
}

void moveInvaders(int x, int y) {
    for (int i = 0; i < NB_INVADERS; i++) {
        invaders[i].position_x += x;
        invaders[i].position_y += y;
    }
}

void updateInvaders() {
    if (counter_invader == 0) {
        int min = getMinInvaderX();
        int max = getMaxInvaderX();
        if (going_left) {
            if (min == 0) {
                going_left = false;
                moveInvaders(0, 1);
            }
            else {
                moveInvaders(-1, 0);
            }
        }
        else {
            if (max == WIDTH - 3) {
                going_left = true;
                moveInvaders(0, 1);
            }
            else {
                moveInvaders(1, 0);
            }
        }
    }
    counter_invader = (counter_invader + 1) % INVADERS_FREQ;
}

void draw_screen() {
    clear();

    // draw player
    mvprintw(player.position_y, player.position_x - 2, player.sprite);

    // draw missile
    if (player.fired)
        mvprintw(player.missile_y, player.missile_x, player.missile_sprite);

    // draw invaders
    for (int i = 0; i < NB_INVADERS; i++)
        if (invaders[i].state == ALIVE) 
            mvprintw(invaders[i].position_y, invaders[i].position_x, invaders[i].sprite);

    refresh(); 
}

int main(int argc, char **argv) {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    scrollok(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    spawnInvaders();

    while (playing) {
        clock_t begin = clock();
        getAndRunInput();
        updateMissile();
        updateInvaders();
        draw_screen();
        clock_t end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC; 
        sleep((1 / (double) FPS) - time_spent);
    }
    
    endwin();
    return 0;
}
