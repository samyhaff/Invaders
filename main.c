#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <time.h>

#define WIDTH 80
#define HEIGHT 24
#define FPS 50
#define MISSILE_FREQ 200

bool playing = true;
int counter = 0;

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

void update_missile() {
    if (player.fired) {
        if (counter == 0)
            player.missile_y -= 1;
        if (player.missile_y == 0)
            player.fired = false;
        counter = (counter + 1) % MISSILE_FREQ;
    }
}

void draw_screen() {
    clear();

    // draw player
    mvprintw(player.position_y, player.position_x - 2, player.sprite);

    // draw missile
    if (player.fired)
        mvprintw(player.missile_y, player.missile_x, player.missile_sprite);

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

    while (playing) {
        clock_t begin = clock();
        getAndRunInput();
        update_missile();
        draw_screen();
        clock_t end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC; 
        sleep((1 / (double) FPS) - time_spent);
    }
    
    endwin();
    return 0;
}
