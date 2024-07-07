#include <assert.h>
#include <raylib.h>
#include <raymath.h>
#include <stdint.h>
#include <stdio.h>

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
#define NUM_RAYCASTS 50

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

  const float cell_w = BIRDSEYE_SIZE / MAP_WIDTH;
  const float cell_h = BIRDSEYE_SIZE / MAP_HEIGHT;

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

      Rectangle square = {.x = pos.x + cell_w * col,
                          .y = pos.y + cell_h * row,
                          .width = cell_w,
                          .height = cell_h};
      DrawRectangleRec(square, Fade(color, opacity));

      DrawLineEx((Vector2){pos.x + col * cell_w, pos.y},
                 (Vector2){pos.x + col * cell_w, pos.y + MAP_WIDTH * cell_w}, 1,
                 GRAY);
    }

    DrawLineEx((Vector2){pos.x, pos.y + row * cell_h},
               (Vector2){pos.x + MAP_WIDTH * cell_w, pos.y + row * cell_h}, 1,
               GRAY);
  }

  Vector2 player_pos = {.x = pos.x + cell_w * player.pos.x,
                        .y = pos.y + cell_h * player.pos.y};
  // Draw raycast lines
  for(int i = 0; i < NUM_RAYCASTS;i++){
    Vector2 end_of_ray = Vector2Add(player_pos, 
                                    Vector2Scale(Vector2Normalize(Vector2Rotate((Vector2){.x=1,.y=0}, (2 * PI * i)/ NUM_RAYCASTS)), 100));
    DrawLineEx(player_pos, end_of_ray, 2, BLUE);
  }

  DrawCircleV(player_pos, 5, YELLOW);
  DrawLineEx(player_pos,
             Vector2Add(player_pos, Vector2Scale(Vector2Normalize(Vector2Rotate(
                                                     (Vector2){.x = 1, .y = 0},
                                                     player.angle - PI)),
                                                 100)),
             2, RED);
}

// returns 0 if no collision
int IsCollidingWithWalls(Vector2 pos, Vector2 move_vec) {
  uint8_t collision_bitset = 0;
  for (int i = 0; i < 4; i++) {
    Tile player_tile;
    switch (i) {
    case 0:
      player_tile = map[(uint8_t)(pos.y)][(uint8_t)(pos.x + COLLISION_BORDER)];
      if(move_vec.x < 0){
          player_tile = FLOOR;
          continue;
        }
      break;
    case 1:
      player_tile = map[(uint8_t)(pos.y - COLLISION_BORDER)][(uint8_t)(pos.x)];
      if(move_vec.y > 0){
          player_tile = FLOOR;
          continue;
        }
      break;
    case 2:
      player_tile = map[(uint8_t)(pos.y)][(uint8_t)(pos.x - COLLISION_BORDER)];
      if(move_vec.x > 0){
          player_tile = FLOOR;
          continue;
        }
      break;
    case 3:
      player_tile = map[(uint8_t)(pos.y + COLLISION_BORDER)][(uint8_t)(pos.x)];
      if(move_vec.y < 0){
          player_tile = FLOOR;
          continue;
        }
      break;
    default:
      assert(0 && "UNREACHABLE");
    }
    if (player_tile == WALL) {
      collision_bitset |= 1 << i;
    }
  }
  return collision_bitset;
}

void UpdatePlayerPos(Vector2 *player_pos, Vector2 move_vec){

  Vector2 pos = Vector2Add(*player_pos, move_vec);
  uint8_t collision_bitset = IsCollidingWithWalls(pos,move_vec);
  uint8_t ones_count = __builtin_popcount(collision_bitset);
  if (ones_count > 1) {
    return;
  }
  // only check for collisions in direction traveling

  switch (collision_bitset) {
  case 0b1:
    player_pos->y = pos.y;
    return;
    break;
  case 0b10:
    player_pos->x = pos.x;
    return;
    break;
  case 0b100:
    player_pos->y = pos.y;
    return;
    break;
  case 0b1000:
    player_pos->x = pos.x;
    return;
    break;
  }

  *player_pos = pos;
}

void MoveUp(Player *player) {
  Vector2 move_vec =
      Vector2Scale(Vector2Rotate((Vector2){.x = 1, .y = 0}, player->angle - PI),
                   PLAYER_SPEED);
  UpdatePlayerPos(&player->pos, move_vec);
}

void MoveDown(Player *player) {
  Vector2 move_vec =
      Vector2Scale(Vector2Rotate((Vector2){.x = 1, .y = 0}, player->angle - PI),
                   -PLAYER_SPEED);
  Vector2 pos = Vector2Add(player->pos, move_vec);
  uint8_t collision_bitset = IsCollidingWithWalls(pos,move_vec);
  uint8_t ones_count = __builtin_popcount(collision_bitset);
  if (ones_count > 1) {
    return;
  }
  switch (collision_bitset) {
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

int main(int argc, char *argv[]) {

  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Ray's caster");
  SetTargetFPS(200);

  Player player = {.pos = (Vector2){.x = 1.5, .y = 1.5}, .angle = (3 * PI) / 2};

  while (!WindowShouldClose()) {

    if (IsKeyDown(KEY_LEFT)) {
      player.angle -= PI / LOOK_SPEED;
    }
    if (IsKeyDown(KEY_RIGHT)) {
      player.angle += PI / LOOK_SPEED;
    }
    if (IsKeyDown(KEY_UP)) {
      MoveUp(&player);
    }
    if (IsKeyDown(KEY_DOWN)) {
      MoveDown(&player);
    }

    BeginDrawing();

    ClearBackground(RAYWHITE);
    DrawFPS(5, WINDOW_HEIGHT - 20);
    DrawBirdsEye((Vector2){.x = 10, .y = 10}, 0.75, player);

    EndDrawing();
  }

  return 0;
}
