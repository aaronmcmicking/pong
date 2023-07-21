/*
 * Simple pong clone using raylib
 */

#include <raylib.h>
#include <rlgl.h>
#include <stdlib.h>

#define WIN_SCALE 16.0/9.0
#define WIN_HEIGHT 1080
#define WIN_WIDTH 1080*WIN_SCALE
#define TARGET_FPS 60

#define OBJECT_COLOUR WHITE
#define BACKGROUND_COLOUR BLACK
#define ACCENT_COLOUR DARKGRAY
#define DEBUG_COLOUR RED

// #define DEBUG
#ifdef DEBUG
#define RENDER_DEBUG(s) render_debug(s)
#else
#define RENDER_DEBUG(s) 
#endif // DEBUG

enum WIN_SIDE{
    LEFT = 1,
    RIGHT = 2
};

enum SCREEN{
    GAME_OVER = 0,
    PLAYING
};

typedef struct Paddle Paddle;
struct Paddle{
    int up_key;
    int down_key;
    float vel;
    enum WIN_SIDE side;
    Rectangle pad;
};

typedef struct Ball Ball;
struct Ball{
    Vector2 base_vel; // default velocity vector
    Vector2 vel;
    Vector2 pos;
    float radius;
};

typedef struct Score Score;
struct Score{
    size_t left;
    size_t right;
};

typedef struct LogicReturnInfo LogicReturnInfo;
struct LogicReturnInfo{
    enum SCREEN next_screen;
    enum WIN_SIDE winner; // 0 if next_screen != GAME_OVER
};

Ball construct_ball();
Paddle construct_paddle(int up_key, int down_key, enum WIN_SIDE side);

void init_objects(Paddle* lp, Paddle* rp, Ball* b);

enum SCREEN process_logic(Paddle* lp, Paddle* rp, Ball* b, Score* s);
void update_paddle(Paddle* p);
LogicReturnInfo update_ball(Ball* b, Paddle lp, Paddle rp);
void update_score(Score* s, enum WIN_SIDE winner);

void render(Paddle lp, Paddle rp, Ball b, Score score, enum SCREEN screen);
void draw_score(Score s);
void draw_centre_line();
