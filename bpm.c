/**
 BPM - A small but effective tracker player using xmp
 Copyright © 2017 Fernando Meyer

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 */
#include "sound.h"
#include <stdio.h>
#include <stdlib.h>
#include <xmp.h>

// Whenever a new file starts to play I print the details to a file, so I can
// use it on OBS.
// TODO(fmeyer): pass filename as attribute
// TODO(fmeyer): test stdout output
static void print_modinfo(char *filename, struct xmp_module_info *mi) {
  struct xmp_module *mod = mi->mod;

  FILE *fp = fopen("~/Documents/twitch/assets/teleprompt/music.txt", "w+");
  if (!fp) {
    perror("File creation failed");
    exit(EXIT_FAILURE);
  }

  char *info_buffer = malloc(1024);
  sprintf(info_buffer, "mod: %s - bpm: %d \n", mod->name, mod->bpm);
  fputs(info_buffer, fp);
  fflush(fp);

  // discart allocated file and buffer
  fclose(fp);
  free(info_buffer);
}

int main(int argc, char **argv) {
  static xmp_context ctx;
  static struct xmp_module_info mi;
  struct xmp_frame_info fi;
  int row, i;

  if (sound_init(44000, 2) < 0) {
    fprintf(stderr, "Coudn't start audio interface, blame alsa\n");
    exit(1);
  }

  ctx = xmp_create_context();

  for (i = 1; i < argc; i++) {
    if (xmp_load_module(ctx, argv[i]) < 0) {
      fprintf(stderr, "%s: error loading %s\n", argv[0], argv[i]);
      continue; // skips the current file
    }

    if (xmp_start_player(ctx, 44100, 0) == 0) {
      xmp_get_module_info(ctx, &mi);
      print_modinfo(argv[i], &mi);
      row = -1;
      while (xmp_play_frame(ctx) == 0) {
        xmp_get_frame_info(ctx, &fi);
        if (fi.loop_count > 0)
          break;
        // TODO(fmeyer): test a different player than alsa
        sound_play(fi.buffer, fi.buffer_size);

        if (fi.row != row) {
          row = fi.row;
        }
      }
      xmp_end_player(ctx);
    }

    xmp_release_module(ctx);
  }

  xmp_free_context(ctx);

  return 0;
}
