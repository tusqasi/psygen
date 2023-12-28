#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define WIDTH 1000
#define HEIGHT 1000

#define GLSL_VERSION 330

#define G (1.0 / 10000.0)
typedef struct _obj {
  Color color;
  Vector2 position;
  Vector2 velocity;
  Vector2 acceleration;
  float mass;
  int size_radius;
} Object;

void run_gravity_engine(Vector2 *forces, const Object *objects,
                        const int n_objects) {

  for (int i = 0; i < n_objects; i++) {
    Vector2 force = Vector2Zero();

    for (int j = 0; j < n_objects; j++) {
      if (i == j) {
        continue;
      }
      float m1 = objects[i].mass;
      float m2 = objects[j].mass;

      Vector2 r_ab = Vector2Subtract(objects[j].position, objects[i].position);
      float distance_sq = Vector2LengthSqr(r_ab);
      if (distance_sq > 0.0) {
        r_ab = Vector2Normalize(r_ab);
        float force_mg = G * m1 * m2 / distance_sq;
        force = Vector2Add(Vector2Scale(r_ab, force_mg), force);
      }
    }
    forces[i] = force;
  }
}

void update_objects(Object *objects, const Vector2 *forces,
                    const int n_objects) {

  for (int i = 0; i < n_objects; i++) {
    Object *current_object = &objects[i];
    current_object->acceleration = forces[i];
    current_object->velocity =
        Vector2Add(current_object->acceleration, current_object->velocity);
    current_object->position =
        Vector2Add(current_object->position, current_object->velocity);

    if (current_object->position.x >= WIDTH) {
      current_object->position.x = WIDTH;
      current_object->velocity.x = -current_object->velocity.x * 0.8;
    }
    if (current_object->position.x <= 0) {
      current_object->position.x = 0;
      current_object->velocity.x = -current_object->velocity.x * 0.8;
    }
    if (current_object->position.y <= 0) {
      current_object->position.y = 0;
      current_object->velocity.y = -current_object->velocity.y * 0.8;
    }
    if (current_object->position.y >= WIDTH) {
      current_object->position.y = WIDTH;
      current_object->velocity.y = -current_object->velocity.y * 0.8;
    }
    current_object->acceleration = Vector2Zero();
  }
}

int main(int argc, char *argv[]) {
  const int n_objects = 4;
  const double trig_size = 300;
  const double trig_diagonal =
      sqrt(trig_size * trig_size - (trig_size / 2) * (trig_size / 2));
  Object objects[] = {
      (Object){BLUE,
               {WIDTH / 2.0, HEIGHT / 2.0 - trig_diagonal / 2},
               {0, 0},
               {0, 0},
               250.0,
               10},
      (Object){GREEN,
               {WIDTH / 2.0 + trig_size / 2, HEIGHT / 2.0 + trig_diagonal / 2},
               {.0, 0.0},
               {0, 0},
               100,
               10},
      (Object){RED,
               {WIDTH / 2.0 - trig_size / 2, HEIGHT / 2.0 + trig_diagonal / 2},
               {0, 0},
               {0, 0},
               100,
               10},
      (Object){WHITE,
               {WIDTH / 2.0 +40 ,
                HEIGHT / 2.0 + trig_size / 2},
               {0, 0},
               20.0,
               10},
      (Object){GRAY,
               {WIDTH / 2.0 - 201, HEIGHT / 2.0},
               {0, 0},
               {0, 0},
               -10.0,
               40},
  };

  float orbital_velocity_force_mg =
      1 * sqrt(G * (objects[1].mass + objects[0].mass) /
               Vector2Distance(objects[0].position, objects[1].position));

  objects[1].velocity.x = -orbital_velocity_force_mg;
  objects[0].velocity.x = +orbital_velocity_force_mg;

  SetTargetFPS(230);
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(WIDTH, HEIGHT, "Ohh yeaaahhh!!!!");

  RenderTexture paths = LoadRenderTexture(WIDTH, HEIGHT);
  RenderTexture sun_texture = LoadRenderTexture(WIDTH, HEIGHT);
  Vector2 *forces = (Vector2 *)malloc(sizeof(Vector2) * n_objects);

  // Shader blur_shader = LoadShader(0, "shaders/blur.fs");

  while (!WindowShouldClose()) {
    ClearBackground(BLACK);

    run_gravity_engine(forces, objects, n_objects);
    for (int i = 0; i < n_objects; i++) {
      printf("%f, %f\n", forces[i].x, forces[i].y);
    }
    update_objects(objects, forces, n_objects);

    BeginTextureMode(paths);
    {
      for (int i = 0; i < n_objects; i++) {

        DrawPixel(objects[i].position.x, WIDTH - objects[i].position.y, WHITE);
      }
      // DrawPixel(cg.x, WIDTH - cg.y, RED);
      DrawRectangle(0, 0, WIDTH, HEIGHT, (Color){0, 0, 0, 1});
    }
    EndTextureMode();

    BeginDrawing();
    {
      DrawTextureRec(paths.texture, (Rectangle){0, 0, WIDTH, HEIGHT},
                     Vector2Zero(), WHITE);
      for (int i = 0; i < n_objects; i++) {
        DrawCircleV(objects[i].position, objects[i].size_radius,
                    objects[i].color);
      }
    }
    EndDrawing();
  }

  free(forces);
  // UnloadShader(blur_shader);
  UnloadRenderTexture(paths);
  UnloadRenderTexture(sun_texture);

  CloseWindow();
  return EXIT_SUCCESS;
}
