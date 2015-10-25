#include <pebble.h>

#include "wiki.h"

#define MAX_STEPS 300
static Window *s_wiki_window;
static TextLayer *s_text_layer;
static char *wikiData[MAX_STEPS];
PropertyAnimation *s_box_animation;

static int currStep = 0;
static int numSteps = 0;
static int number_of_pixels = 0;

/************************************ Callbacks *************************************/

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  currStep++;

  if(currStep >= numSteps - 1){
    text_layer_set_text(s_text_layer, "End of wiki page. Press back to search again.");
    return;
  }
  else
    changeText(currStep, FONT_KEY_GOTHIC_18_BOLD);

}
static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(currStep < 2){
    changeText(0, FONT_KEY_GOTHIC_28_BOLD);
    return;
  }
  currStep--;
  changeText(currStep, FONT_KEY_GOTHIC_18_BOLD);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);

}

static void changeText(int step, const char* font){
  animation_unschedule_all();
  text_layer_set_font(s_text_layer, fonts_get_system_font(font));
  text_layer_set_text(s_text_layer, wikiData[step]);
  number_of_pixels = layer_get_bounds(window_get_root_layer(s_wiki_window)).size.h - text_layer_get_content_size(s_text_layer).h;
  if (number_of_pixels < 0) {
      animate_quote(number_of_pixels);
  }
}

/************************************ App *************************************/
static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  s_text_layer = text_layer_create(GRect(0, 0, bounds.size.w - 10, 2000));
  text_layer_set_text_color(s_text_layer, GColorDarkGray);
  
  changeText(0, FONT_KEY_GOTHIC_28_BOLD);
  text_layer_set_text(s_text_layer, wikiData[0]);
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
  // if height of quote > height of window, initiate animation to scroll
  GSize text_size = text_layer_get_content_size(s_text_layer);
  number_of_pixels = bounds.size.h - text_size.h;
  printf("bound height: %d\n", bounds.size.h);
  printf("text height: %d\n", text_size.h);
  if (number_of_pixels < 0) {
      printf("%s\n", "ANIMATION FOR DAYS");
      animate_quote(number_of_pixels);
  }
  
  
}

void animate_quote(int pixels_to_scroll_by) {
  GRect start_frame = GRect(0, (pixels_to_scroll_by < 0? 0 : -pixels_to_scroll_by), 144, 2000);
  GRect finish_frame =  GRect(0, (pixels_to_scroll_by < 0? pixels_to_scroll_by : 0), 144, 2000);

  s_box_animation = property_animation_create_layer_frame(text_layer_get_layer(s_text_layer), &start_frame, &finish_frame);
  animation_set_handlers((Animation*)s_box_animation, (AnimationHandlers) {
    .stopped = anim_stopped_handler
  }, NULL);
  animation_set_duration((Animation*)s_box_animation, abs(pixels_to_scroll_by) * 35); // delay is proportional to text size
  animation_set_curve((Animation*)s_box_animation, AnimationCurveLinear);  // setting equal speed animation
  animation_set_delay((Animation*)s_box_animation, 3000); //initial delay of 3 seconds to let user start reading quote

  animation_schedule((Animation*)s_box_animation);
}

void anim_stopped_handler(Animation *animation, bool finished, void *context) {
  // Free the animation
  property_animation_destroy(s_box_animation);

  // Schedule the reverse animation, unless the app is exiting
  if (finished) {
    number_of_pixels = -number_of_pixels;
    animate_quote(number_of_pixels);
  }
}

static void window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
}

void wiki_init(char *data){
//   memset(wikiData, 0, MAX_STEPS);
  
  for(int i = 0; i < MAX_STEPS; i++)    
    wikiData[i] = NULL;
  
  printf("%s\n", "got here");
  printf("data: %s\n", data);
  //printf("%s\n", pch);
  printf("%s\n", "I did the thing\n");
  char *cur = data;
  char *start = data;
  int curNum = 0;
  while (*cur) {
    if (*cur != '`') {
      cur++;
    } else {
      *cur = '\0';
      wikiData[curNum] = start;
      printf("%s\n", wikiData[curNum]);
      start = cur + 1;
      cur = start;
      curNum++;
    }
  }
  numSteps = curNum;
  
  s_wiki_window = window_create();
  window_set_click_config_provider(s_wiki_window, click_config_provider);
  window_set_window_handlers(s_wiki_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_wiki_window, true);

  window_set_background_color(s_wiki_window, GColorWhite);
  printf("%s\n", "3\n");
}

void wiki_deinit(void){
  window_destroy(s_wiki_window);
}