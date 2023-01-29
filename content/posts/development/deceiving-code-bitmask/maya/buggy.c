#include <stdio.h>

typedef enum {
  WIN_MODE_FULLSCREEN = 1 << 0,
  WIN_MODE_STENCIL = 1 << 1,
  WIN_MODE_DOUBLE_BUFFER = 1 << 2
} win_mode_t;

int create_window(win_mode_t mode)
{
  if(mode & WIN_MODE_FULLSCREEN == WIN_MODE_FULLSCREEN) {
    printf("fullscreen\n");
  }
  if(mode & WIN_MODE_STENCIL == WIN_MODE_STENCIL) {
    printf("stencil\n");
  }
  if(mode & WIN_MODE_DOUBLE_BUFFER == WIN_MODE_DOUBLE_BUFFER) {
    printf("double buffer\n");
  }
  return 0;
}

int main()
{
  create_window(WIN_MODE_FULLSCREEN | WIN_MODE_DOUBLE_BUFFER);
  return 0;
}
