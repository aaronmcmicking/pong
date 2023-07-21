#define _DEFAULT_SOURCE // includes definition of usleep(s) in unistd.h
#include <unistd.h>
#ifdef __linux__
    #include <unistd.h>
    #define sleep_ms(s) usleep(s*1000)
    #define sleep_s(s) sleep(s)
#else
#ifdef _WIN32
    /* Macros resolve conflicting definitions in raylib.h and windows.h */
    /* Sourced from github.com/raysan5/raylib/issues/1217 */
    #define NOGDI             // All GDI defines and routines
	#define NOUSER            // All USER defines and routines
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #undef near
	#undef far
    #define sleep_ms Sleep(s)
    #define sleep_s(s) Sleep(s*1000)
#endif // _WIN32
#endif // __linux__

#include "pong.h"
#include <raylib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <time.h>

void render_debug(Score score){
    char buf[50] = "";
    snprintf(buf, 49, "%ld", score.left);
    DrawText(buf, 20, 20, 40, DEBUG_COLOUR);
    snprintf(buf, 49, "%ld", score.right);
    DrawText(buf, 80, 20, 40, DEBUG_COLOUR);
}

Paddle construct_paddle(int up_key, int down_key, enum WIN_SIDE side){
    float width = WIN_WIDTH/120.0;
    float height = WIN_HEIGHT/7.0;
    float init_y_pos = WIN_HEIGHT/2.0 - height/2.0;
    int vel = 1200 / TARGET_FPS;

    float side_padding;
    if(side == LEFT){
        side_padding = WIN_WIDTH/35.0;
    }else{
        side_padding = WIN_WIDTH - WIN_WIDTH/35.0 - width;
    }

    Rectangle r = { 
		.x = side_padding, 
		.y = init_y_pos, 
		.width =width, 
		.height = height 
    };

    return (Paddle){ 
		.up_key = up_key, 
		.down_key = down_key, 
		.vel = vel, 
        .side = side,
		.pad = r 
    };
}

Ball construct_ball(){
    float radius = WIN_WIDTH/90.0;
    float v = 1200.0 / TARGET_FPS;

    float init_y_vel = rand() % (int) v;

    return (Ball){ 
		.pos = (Vector2){
                    .x = WIN_WIDTH/2.0,
                    .y = WIN_HEIGHT/2.0
                }, 
		.vel = (Vector2){ 
					.x = v, 
					.y = init_y_vel 
                }, 
		.base_vel = (Vector2){ 
					.x = v, 
					.y = v 
                }, 
		.radius = radius 
    };
}

void init_objects(Paddle *lp, Paddle *rp, Ball *b){
    *lp = construct_paddle(KEY_W, KEY_S, LEFT);
    *rp = construct_paddle(KEY_UP, KEY_DOWN, RIGHT);
    *b = construct_ball();
}

float absf(float x){
    return x >= 0 ? x : -x;
}

float calculate_ball_y_velocity(Ball b, Paddle p){
    float distance_from_paddle_center = absf(b.pos.y - (p.pad.y + p.pad.height/2.0));
    distance_from_paddle_center /= p.pad.height/2.0; // clamp to 0-1
    // Scale non-linearly with equation 0.95*sqrt(2*x)
    // Magic numbers chosen arbitrarily, and 1.34 ≈ 0.95*sqrt(2)
    float new_velocity = b.base_vel.y * (1.34*sqrt(distance_from_paddle_center));
    new_velocity = b.vel.y > 0 ? new_velocity : -new_velocity; // get correct direction
    return new_velocity;
}

enum SCREEN process_logic(Paddle* lp, Paddle* rp, Ball* b, Score* s){
    update_paddle(lp);
    update_paddle(rp);
    GameLogicInfo frame_info = update_ball(b, *lp, *rp);
    enum SCREEN next_screen = frame_info.next_screen;
    if(next_screen == GAME_OVER){
        update_score(s, frame_info.winner);
    }
    return next_screen;
}

void render(Paddle lp, Paddle rp, Ball b, Score score, enum SCREEN screen){
    ClearBackground(BACKGROUND_COLOUR);
    BeginDrawing();
    draw_score(score);
    draw_centre_line();
    if(screen == PLAYING){
        DrawRectangleRec(lp.pad, OBJECT_COLOUR);
        DrawRectangleRec(rp.pad, OBJECT_COLOUR);
        DrawCircleV(b.pos, b.radius, OBJECT_COLOUR);
    }
    RENDER_DEBUG(score);
    EndDrawing();
}

void draw_score(Score s){
    float center_padding = WIN_WIDTH/8.0;
    float top_padding = WIN_HEIGHT/10.0;
    int font_size = WIN_HEIGHT/10.0;
    char num_buf[10] = {0};
    
    snprintf(num_buf, 8, "%d\n", (int)s.left);
    DrawText(num_buf, WIN_WIDTH/2.0 - center_padding - font_size/2.0, top_padding, font_size, OBJECT_COLOUR);
    
    snprintf(num_buf, 8, "%d\n", (int)s.right);
    DrawText(num_buf, WIN_WIDTH/2.0 + center_padding, top_padding, font_size, OBJECT_COLOUR);
}

void draw_centre_line(){
    int num_dashes = 17;
    float width = WIN_WIDTH/190.0;
    float height = WIN_HEIGHT/25.0;
    float height_offset = ((float)WIN_HEIGHT - (num_dashes*height)) / (num_dashes+1);
    Rectangle rec = {.width = width, .height = height, .x = WIN_WIDTH/2.0-(width/2.0), .y = height_offset};

    assert(num_dashes*height <= WIN_HEIGHT);

    for(int i = 0; i < num_dashes; i++){
        DrawRectangleRec(rec, ACCENT_COLOUR);
        rec.y += height_offset + height;
    }
}

void update_paddle(Paddle* p){
    bool at_screen_top = (p->pad.y <= 0);
    bool at_screen_bottom = (p->pad.y + p->pad.height >= WIN_HEIGHT);
    if(IsKeyDown(p->up_key)){
        if(at_screen_top){
            p->pad.y = 0;
        }else{
            p->pad.y -= p->vel;
        }
    }else if(IsKeyDown(p->down_key)){
        if(at_screen_bottom){
            p->pad.y = WIN_HEIGHT - p->pad.height;
        }else{
            p->pad.y += p->vel;
        }
    }
}

GameLogicInfo update_ball(Ball* b, Paddle lp, Paddle rp){
    // bounce off top and bottom of screen
    bool ball_touching_top_of_screen = b->pos.y - b->radius <= 0;
    bool ball_touching_bottom_of_screen = b->pos.y + b->radius >= WIN_HEIGHT;
    if(ball_touching_top_of_screen || ball_touching_bottom_of_screen){
        b->vel.y = -b->vel.y;
    }

    // wall collision detection
    bool ball_touching_left_wall = b->pos.x - b->radius <= 0;
    bool ball_touching_right_wall = b->pos.x + b->radius >= WIN_WIDTH;
    if(ball_touching_left_wall){
        return (GameLogicInfo){.next_screen = GAME_OVER, .winner = RIGHT};
    }else if(ball_touching_right_wall){
        return (GameLogicInfo){.next_screen = GAME_OVER, .winner = LEFT};
    }
    
    // paddle collision detection
    bool touching_left_paddle = CheckCollisionCircleRec(b->pos, b->radius, lp.pad);
    bool touching_right_paddle = CheckCollisionCircleRec(b->pos, b->radius, rp.pad);
    if(touching_left_paddle){
        bool ball_hit_top_or_bottom_of_paddle = b->pos.x <= lp.pad.x + lp.pad.width;
        if(ball_hit_top_or_bottom_of_paddle){
            b->vel.y = -b->vel.y;
        }else{
            b->vel.x = -b->vel.x;
            b->vel.y = calculate_ball_y_velocity(*b, lp);
        }
    }else if(touching_right_paddle){
        bool ball_hit_top_or_bottom_of_paddle = b->pos.x >= rp.pad.x;
        if(ball_hit_top_or_bottom_of_paddle){
            b->vel.y = -b->vel.y;
        }else{
            b->vel.x = -b->vel.x;
            b->vel.y = calculate_ball_y_velocity(*b, rp);
        }
    }

    // update position
    b->pos.x += b->vel.x;
    b->pos.y += b->vel.y;
    return (GameLogicInfo){.next_screen = PLAYING, .winner = 0};
}

void update_score(Score* s, enum WIN_SIDE winner){
    switch(winner){
        case LEFT: s->left++; break;
        case RIGHT: s->right++; break;
        default: assert(0 && "unreachable"); break;
    }
}


int main(){
    srand(time(0));


    Paddle left_paddle = {0}, right_paddle = {0};
    Ball ball = {0};
    Score score = {0};

    init_objects(&left_paddle, &right_paddle, &ball);
    InitWindow(WIN_WIDTH, WIN_HEIGHT, "pong");
    SetTargetFPS(TARGET_FPS);
    
    bool game_over = false;
    bool wait = false;
    enum SCREEN screen = PLAYING;
    while(!WindowShouldClose()){
        if(screen == PLAYING){
            screen = process_logic(&left_paddle, &right_paddle, &ball, &score);
        }else if(IsKeyPressed(KEY_R)){
            screen = PLAYING;
            init_objects(&left_paddle, &right_paddle, &ball);
            wait = true;
        }
        render(left_paddle, right_paddle, ball, score, screen);
        if(wait) { sleep_ms(300); wait = false; }
    }

    CloseWindow();

    (void) game_over;

    return 0;
}








