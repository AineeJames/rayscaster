#include <raylib.h>
#include <raymath.h>
#include <stdint.h>
#include <assert.h>

typedef enum {
  FLOOR,
  WALL,
} Tile;

typedef struct {
  Vector2 pos;
  double angle;
} Player;

#define MAP_WIDTH 13
#define MAP_HEIGHT 13
#define SCREEN_FACTOR 50
#define WINDOW_WIDTH 16 * SCREEN_FACTOR
#define WINDOW_HEIGHT 9 * SCREEN_FACTOR

#define BIRDSEYE_SIZE 300.
#define LOOK_SPEED 200
#define PLAYER_SPEED 0.025

#define COLLISION_BORDER 0.25

Tile map[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

void DrawBirdsEye(Vector2 pos, float opacity, Player player) {

  const float cell_w = BIRDSEYE_SIZE/MAP_WIDTH;
  const float cell_h = BIRDSEYE_SIZE/MAP_HEIGHT;

  for (int row = 0; row < MAP_HEIGHT; row++) {
    for (int col = 0; col < MAP_WIDTH; col++) {

      Color color;
      switch (map[row][col]) {
      case (FLOOR):
        color = BLACK;
        break;
      case (WALL):
        color = BLUE;
        break;
      }

      Rectangle square = {
        .x = pos.x + cell_w * col,
        .y = pos.y + cell_h * row,
        .width = cell_w,
        .height = cell_h 
      };
      DrawRectangleRec(square, Fade(color, opacity));

      DrawLineEx(
        (Vector2){pos.x + col * cell_w, pos.y},
        (Vector2){pos.x + col * cell_w, pos.y + MAP_WIDTH * cell_w},
        1, 
        GRAY
      );
    }

    DrawLineEx(
      (Vector2){pos.x,                      pos.y + row * cell_h}, 
      (Vector2){pos.x + MAP_WIDTH * cell_w, pos.y + row * cell_h},
      1, 
      GRAY
    );
  }

  Vector2 player_pos = {
    .x = pos.x + cell_w * player.pos.x,
    .y = pos.y + cell_h * player.pos.y
  };
  DrawCircleV(player_pos, 5, YELLOW);
  DrawLineEx(player_pos, Vector2Add(player_pos, Vector2Scale(Vector2Normalize(Vector2Rotate((Vector2){.x=1, .y=0}, player.angle-PI)), 100)), 2, RED);

}

// returns 0 if no collision
int IsCollidingWithWalls(Vector2 pos){
 for(int i = 0; i < 4; i++){
 Tile player_tile;
 switch (i){
      case 0:
        player_tile = map[(uint8_t)(pos.y + COLLISION_BORDER)][(uint8_t)(pos.x + COLLISION_BORDER)];
        break;
      case 1: 
        player_tile = map[(uint8_t)(pos.y - COLLISION_BORDER)][(uint8_t)(pos.x + COLLISION_BORDER)];
        break;
      case 2:
        player_tile = map[(uint8_t)(pos.y - COLLISION_BORDER)][(uint8_t)(pos.x - COLLISION_BORDER)];
        break;
      case 3:
        player_tile = map[(uint8_t)(pos.y + COLLISION_BORDER)][(uint8_t)(pos.x - COLLISION_BORDER)];
        break;
      default:
        assert(0 && "UNREACHABLE");
    }
 if(player_tile == WALL){
    return 1;
  }
 }
  return 0;
}

void MoveUp(Player *player){
 Vector2 pos = Vector2Add(player->pos, Vector2Scale(Vector2Rotate((Vector2){.x=1,.y=0}, player->angle-PI), PLAYER_SPEED));
 if(IsCollidingWithWalls(pos)){
    return;
  }
 player->pos = pos;
}

void MoveDown(Player *player){
 Vector2 pos= Vector2Add(player->pos, Vector2Scale(Vector2Rotate((Vector2){.x=1,.y=0}, player->angle-PI), -PLAYER_SPEED));
 if(IsCollidingWithWalls(pos)){
    return;
  }
 player->pos = pos;
}

int main(int argc, char *argv[]) {

  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Ray's caster");
  SetTargetFPS(200);

  Player player = {
    .pos = (Vector2) {
      .x = 1.5,
      .y = 1.5
    },
    .angle = (3*PI)/2
  };

  while (!WindowShouldClose()) {

    if (IsKeyDown(KEY_LEFT)) {
        player.angle -= PI/LOOK_SPEED;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        player.angle += PI/LOOK_SPEED;
    }
    if (IsKeyDown(KEY_UP)) {
      MoveUp(&player);
    }
    if (IsKeyDown(KEY_DOWN)) {
      MoveDown(&player);
    }

    BeginDrawing();

    ClearBackground(RAYWHITE);
    DrawFPS(5, WINDOW_HEIGHT-20);
    DrawBirdsEye(
      (Vector2){.x=10, .y=10},
      0.75,
      player
    );

    EndDrawing();
  }

  return 0;
}
