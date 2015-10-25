#include <pebble.h>

#include "wiki.h"

static Window *s_wiki_window;
static TextLayer *s_text_layer;

static char *wikiData;

/************************************ Callbacks *************************************/

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

/************************************ App *************************************/
static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  s_text_layer = text_layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
  text_layer_set_text(s_text_layer, wikiData);
  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_color(s_text_layer, GColorDarkGray);
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
}

static void window_unload(Window *window) {
  
}

void wiki_init(char *data){
  
  wikiData = data;
  
  s_wiki_window = window_create();
  
  window_set_click_config_provider(s_wiki_window, click_config_provider);
  window_set_window_handlers(s_wiki_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_wiki_window, true);

  window_set_background_color(s_wiki_window, GColorWhite);
  
}

void wiki_deinit(void){
  window_destroy(s_wiki_window);
}