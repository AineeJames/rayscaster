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

#define BIRDSEYE_SIZE 200.
#define LOOK_SPEED 200

#define COLLISION_BORDER 0.5

Tile map[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

void DrawBirdsEye(Vector2 pos, float opacity, Player player) {

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
        .x = pos.x + (BIRDSEYE_SIZE/MAP_WIDTH) * col,
        .y = pos.y + (BIRDSEYE_SIZE/MAP_HEIGHT) * row,
        .width = BIRDSEYE_SIZE/MAP_WIDTH,
        .height = BIRDSEYE_SIZE/MAP_HEIGHT
      };
      DrawRectangleRec(square, Fade(color, opacity));
      Vector2 player_pos = Vector2Add(pos, Vector2Scale(player.pos, BIRDSEYE_SIZE/MAP_WIDTH));
      DrawCircleV(player_pos, BIRDSEYE_SIZE/30, YELLOW);

      // DrawLineEx(player_pos, Vector2Scale(Vector2Normalize(Vector2Rotate(player_pos, player.angle)), 100), 5, RED);
      DrawLineEx(player_pos, Vector2Add(player_pos, Vector2Scale(Vector2Normalize(Vector2Rotate((Vector2){.x=1, .y=0}, player.angle-PI)), 100)), 2, RED);

    }
  }
}

// returns 0 if no collision
int IsCollidingWithWalls(Vector2 pos){
 uint8_t collision_bitset = 0;
 for(int i = 0; i < 4; i++){
 Tile player_tile;
 switch (i){
      case 0:
        player_tile = map[(uint8_t)(pos.y)][(uint8_t)(pos.x + COLLISION_BORDER)];
        break;
      case 1: 
        player_tile = map[(uint8_t)(pos.y - COLLISION_BORDER)][(uint8_t)(pos.x)];
        break;
      case 2:
        player_tile = map[(uint8_t)(pos.y )][(uint8_t)(pos.x - COLLISION_BORDER)];
        break;
      case 3:
        player_tile = map[(uint8_t)(pos.y + COLLISION_BORDER)][(uint8_t)(pos.x)];
        break;
      default:
        assert(0 && "UNREACHABLE");
    }
 if(player_tile == WALL){
    collision_bitset |= 1 << i;
  }
 }
  return collision_bitset;
}

void MoveUp(Player *player){
 Vector2 pos = Vector2Add(player->pos, Vector2Scale(Vector2Rotate((Vector2){.x=1,.y=0}, player->angle-PI), 0.1));
  
 uint8_t collision_bitset = IsCollidingWithWalls(pos);
 uint8_t ones_count = __builtin_popcount(collision_bitset);
 if(ones_count > 1){
    return;
  }

 switch (collision_bitset){
    case 0b1:
      player->pos.y = pos.y;
      return;
      break;
    case 0b10:
      player->pos.x = pos.x;
      return;
      break;
    case 0b100:
      player->pos.y = pos.y;
      return;
      break;
    case 0b1000:
      player->pos.x = pos.x;
      return;
      break;
  }

 player->pos = pos;
}

void MoveDown(Player *player){
 Vector2 pos= Vector2Add(player->pos, Vector2Scale(Vector2Rotate((Vector2){.x=1,.y=0}, player->angle-PI), -0.1));
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
      .x = 2,
      .y = 2
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
