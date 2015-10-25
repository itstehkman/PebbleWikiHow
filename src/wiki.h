#pragma once

void wiki_init(char *data);

void wiki_deinit(void);

static void changeText(int step, const char* font);

void animate_quote(int pixels_to_scroll_by);
void anim_stopped_handler(Animation *animation, bool finished, void *context);