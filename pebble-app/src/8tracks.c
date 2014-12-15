#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
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
  if(is_playing) {
    text_layer_set_text(text_layer, "Play");
    is_playing = false;
  } else {
    text_layer_set_text(text_layer, "Pause");
    is_playing = true;
  }
  send_msg();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Play");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
