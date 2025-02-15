#include "stdbool.h"
#include "plotter.h"
#include "stdio.h"
#include "assert.h"
#include <raylib.h>
#include <raymath.h>
#include <stdarg.h>

static int ui_draw_button_va(bool* is_pressed, float x, float y, float font_size, char* buff, Vector2* size_out, const char* str, va_list va) {
  Vector2 mp = GetMousePosition();
  int c = 0;
  vsprintf(buff, str, va);
  float pad = 5.f;
  Vector2 size = Vector2AddValue(help_measure_text(buff, font_size), 2.f * pad);
  if (size_out) *size_out = size;
  Rectangle box = { x, y, size.x, size.y };
  bool is_in = CheckCollisionPointRec(mp, box);
  if (is_in) {
    bool is_p = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    c = is_p ? 2 : 1;
    if (is_p && is_pressed) {
      *is_pressed = !*is_pressed;
    }
  }
  if (is_pressed && *is_pressed) {
    DrawRectangleRec(box, BLUE);
  } else if (is_in) {
    DrawRectangleRec(box, RED);
  }
  help_draw_text(buff, (Vector2){x + pad, y + pad}, font_size, WHITE);
  return c;
}

int ui_draw_button(bool* is_pressed, float x, float y, float font_size, char* buff, const char* str, ...) {
  va_list args;
  va_start(args, str);
  int ret = ui_draw_button_va(is_pressed, x, y, font_size, buff, NULL, str, args);
  va_end(args);
  return ret;
}

static Vector2 stack_pos;
static bool stack_is_inited = false;
static float* stack_scroll_position;
static Vector2 stack_button_size;
static float stack_offset;
static float stack_font_size;
static char* stack_buff;
static int stack_count;
static Vector2 stack_maxsize;

void ui_stack_buttons_init(Vector2 pos, float* scroll_position, float font_size, char* buff) {
  assert(!stack_is_inited);
  stack_pos = pos;
  stack_is_inited = true;
  stack_scroll_position = scroll_position;
  stack_button_size = help_measure_text("T", font_size);
  stack_button_size.y += 5.f;
  stack_offset = -stack_button_size.y * (scroll_position == NULL ? 0.f :(int)*scroll_position);
  stack_font_size = font_size;
  stack_buff = buff;
  stack_count = 0;
  stack_maxsize = (Vector2){0};
}

int ui_stack_buttons_add(bool* is_pressed, char const* str, ...) {
  assert(stack_is_inited);
  Vector2 s = stack_button_size;
  ++stack_count;
  if (stack_offset >= 0) {
    va_list args;
    va_start(args, str);
    int ret = ui_draw_button_va(is_pressed, stack_pos.x, stack_pos.y + stack_offset, stack_font_size, stack_buff, &s, str, args);
    va_end(args);
    if (s.x > stack_maxsize.x) stack_maxsize.x = s.x;
    if (s.y > stack_maxsize.y) stack_maxsize.y = s.y;
    stack_offset += s.y;
    return ret;
  }
  stack_offset += s.y;
  return 0;
}

Vector2 ui_stack_buttons_end(void) {
  assert(stack_is_inited);
  stack_is_inited = false;
  if (stack_count == 0) return stack_pos;
  BoundingBox bb = {.min = {stack_pos.x, stack_pos.y, 0.f}, .max = { stack_pos.x + stack_maxsize.x, stack_pos.y + stack_maxsize.y + stack_offset - stack_button_size.y } };
  if (stack_scroll_position != NULL) {
    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle) {.x = bb.min.x, .y = bb.min.y, .width = bb.max.x - bb.min.x, .height = bb.max.y - bb.min.y })) {
      Vector2 mouse_scroll = GetMouseWheelMoveV();
      *stack_scroll_position += mouse_scroll.y;
    }
    *stack_scroll_position = minf((float)stack_count - 3.f , *stack_scroll_position);
    *stack_scroll_position = maxf(0.f, *stack_scroll_position);
  }
  DrawBoundingBox(bb, RAYWHITE);
  return (Vector2) { stack_pos.x, stack_pos.y + stack_offset };
}

