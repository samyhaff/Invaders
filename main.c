#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <time.h>

#define WIDTH 80
#define HEIGHT 24
#define FPS 60
#define MISSILE_DELAY 300
#define INVADERS_DELAY 500
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
    int fired;
    int missile_x;
    int missile_y;
    int counter_missile;
} invaders[36];

void spawnInvaders() {
    for (int i = 0; i < 36; i++) {
        invaders[i].position_x = (WIDTH / 2) - 7 + ((3 * i) % 18);   
        invaders[i].position_y = 2 + (i / 6);   
        invaders[i].sprite = "/O\\";
        invaders[i].state = ALIVE;
        invaders[i].score = 5;
        invaders[i].fired = false;
        invaders[i].missile_x = 0;
        invaders[i].missile_y = 0;
        invaders[i].counter_missile = 0;
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
        if ((invaders[i].state == ALIVE) && (invaders[i].position_x <= player.missile_x) && (invaders[i].position_y == player.missile_y) && (player.missile_x <= invaders[i].position_x + 2)) {
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
        counter_missile = (counter_missile + 1) % MISSILE_DELAY;
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

int getMaxInvaderY() {
    int max = 0;
    for (int i = 0; i < NB_INVADERS; i++) {
        if ((invaders[i].position_y > max) && (invaders[i].state == ALIVE))
            max = invaders[i].position_y;
    }
    return max;
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
    counter_invader = (counter_invader + 1) % INVADERS_DELAY;
}

void invadersAttack() {
    int r;
    for (int i = 0; i < NB_INVADERS; i++) {
        r = rand(); 
        if ((r < (int) (0.000001 * RAND_MAX)) && (invaders[i].state == ALIVE) && !(invaders[i].fired)) {
            invaders[i].fired = true;
            invaders[i].missile_x = invaders[i].position_x;
            invaders[i].missile_y = invaders[i].position_y;
        }
    }
}

void updateInvaderMissile() {
    for (int i = 0; i < NB_INVADERS; i++) {
        if (invaders[i].fired) {
           if (invaders[i].missile_y == player.position_y + 1) 
               invaders[i].fired = false;
           if (invaders[i].counter_missile == 0)
               invaders[i].missile_y += 1;
        }
        invaders[i].counter_missile = (invaders[i].counter_missile + 1) % MISSILE_DELAY;
    }
}

void gameOver() {
    int count_alive = 0;
    for (int i = 0; i < NB_INVADERS; i++) {
        if (invaders[i].state == ALIVE) 
            count_alive += 1;
    }
    if ((count_alive == 0) || (getMaxInvaderY() == player.position_y))
        playing = false;
    for (int i = 0; i < NB_INVADERS; i++) {
        if ((invaders[i].missile_y == player.position_y) && (invaders[i].position_x >= player.position_x) && (invaders[i].position_x <= player.position_x + 2))
            playing = false;
    }
}

void draw_screen() {
    // draw player
    mvprintw(player.position_y, player.position_x - 2, player.sprite);

    // draw missile
    if (player.fired)
        mvprintw(player.missile_y, player.missile_x, player.missile_sprite);

    // draw invaders
    for (int i = 0; i < NB_INVADERS; i++)
        if (invaders[i].state == ALIVE) 
            mvprintw(invaders[i].position_y, invaders[i].position_x, invaders[i].sprite);

    // draw invaders missiles
    for (int i = 0; i < NB_INVADERS; i++)
        if (invaders[i].fired)
            mvprintw(invaders[i].missile_y, invaders[i].missile_x, "|");
}

int main(int argc, char **argv) {
    srand(time(NULL));

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
        invadersAttack();
        updateInvaderMissile();
        gameOver();
        erase();
        draw_screen();
        clock_t end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC; 
        refresh();
        sleep((1 / (double) FPS) - time_spent);
    }
    
    endwin();
    return 0;
}
