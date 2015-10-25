#include <pebble.h>

#include "wiki.h"

static Window *s_main_window;
static TextLayer *s_query_layer, *s_wiki_layer;
static BitmapLayer *s_bitmap_layer;
static GBitmap *s_example_bitmap;

static DictationSession *s_dictation_session;
static char s_last_text[1024];

/******************************* Dictation API ********************************/

static void dictation_session_callback(DictationSession *session, DictationSessionStatus status, 
                                       char *transcription, void *context) {
   
  if(status == DictationSessionStatusSuccess) {
    
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Transcription:\t%s", transcription);
    
    // Send to JS
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);     
    Tuplet tuple = TupletCString(0, transcription);
                                 
    dict_write_tuplet(iter, &tuple);
    dict_write_end(iter);

    app_message_outbox_send();
                                 
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Transcription failed.\n\nError ID:\n%d", (int)status);
  }
  
}

/************************************ Callbacks *************************************/
static void in_received_handler(DictionaryIterator *iter, void *context) {
  
  Tuple *data = dict_find(iter, 1);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Received from JS :\t%s", data->value->cstring);
  
  wiki_init(data->value->cstring);
  
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  // Start voice dictation UI
  dictation_session_start(s_dictation_session);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

/************************************ App *************************************/

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_query_layer = text_layer_create(GRect(5, 120, bounds.size.w - 10, bounds.size.h - 30));
  
  //toucan
  s_example_bitmap = gbitmap_create_with_resource(RESOURCE_ID_toucan);
  s_bitmap_layer = bitmap_layer_create(GRect(5, -5, bounds.size.w-20, bounds.size.h-25));
  bitmap_layer_set_bitmap(s_bitmap_layer, s_example_bitmap);
  bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));
 
  //text
  text_layer_set_text(s_query_layer, "Press select and say what you want!");
  text_layer_set_font(s_query_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_color(s_query_layer, GColorDarkGray);
  text_layer_set_text_alignment(s_query_layer, GTextAlignmentCenter);
  text_layer_set_background_color(s_query_layer, GColorClear);
//   layer_set_frame((Layer *)s_query_layer, GRect(x, y, w, h));
  layer_add_child(window_layer, text_layer_get_layer(s_query_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(s_query_layer);
  text_layer_destroy(s_wiki_layer);
  
  gbitmap_destroy(s_example_bitmap);
  bitmap_layer_destroy(s_bitmap_layer);

}


static void init() {
  s_main_window = window_create();
  window_set_click_config_provider(s_main_window, click_config_provider);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_main_window, true);

  // Create new dictation session
  s_dictation_session = dictation_session_create(sizeof(s_last_text), dictation_session_callback, NULL);
  window_set_background_color(s_main_window, GColorWhite);
  
  app_message_register_inbox_received(in_received_handler);
  app_message_open(3000, 3000);
}

static void deinit() {
  // Free the last session data
  dictation_session_destroy(s_dictation_session);

  window_destroy(s_main_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
