#include <pebble.h>

static Window* window;

static ActionBarLayer* action_bar;

static GBitmap* action_icon_play;
static GBitmap* action_icon_pause;
static GBitmap* action_icon_volume_up;
static GBitmap* action_icon_volume_down;
static GBitmap* action_icon_right_arrow;

static bool is_playing = false;

typedef enum {
  KEY_MSG = 0x0
} MessageKey;

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Message received!");

  /* Tuple *t = dict_read_first(iterator); */
  /*  */
  /* // Process all pairs present */
  /* while (t != NULL) { */
  /*   // Long lived buffer */
  /*   static char s_buffer[64]; */
  /*  */
  /*   // Process this pair's key */
  /*   switch (t->key) { */
  /*     case KEY_MSG: */
  /*       snprintf(s_buffer, sizeof(s_buffer), "Received '%s'", t->value->cstring); */
  /*       text_layer_set_text(s_output_layer, s_buffer); */
  /*       break; */
  /*   } */
  /*  */
  /*   // Get next pair, if any */
  /*   t = dict_read_next(iterator); */
  /* } */
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void send_msg() {
  DictionaryIterator* outbox_iter;
  if (app_message_outbox_begin(&outbox_iter) != APP_MSG_OK){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "opening outbox failed\n");
    return;
  }

  if(outbox_iter == NULL){
    return;
  }

  dict_write_cstring(outbox_iter, KEY_MSG, "toggle");

  if(dict_write_end(outbox_iter) == 0){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "the parameters for writing were invalid" );
  }

  app_message_outbox_send();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  send_msg();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(action_bar, window);
  action_bar_layer_set_click_config_provider(action_bar, click_config_provider);

  action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, action_icon_play);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, action_icon_right_arrow);
}

static void window_unload(Window *window) {
}

static void init(void) {
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  action_icon_play        = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_PLAY);
  action_icon_pause       = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_PAUSE);
  action_icon_volume_up   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_VOLUME_UP);
  action_icon_volume_down = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_VOLUME_DOWN);
  action_icon_right_arrow = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_RIGHT_ARROW);

  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  gbitmap_destroy(action_icon_play);
  gbitmap_destroy(action_icon_pause);
  gbitmap_destroy(action_icon_volume_up);
  gbitmap_destroy(action_icon_volume_down);
  gbitmap_destroy(action_icon_right_arrow);

  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
