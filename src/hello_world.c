#include <pebble.h>
// #include <pebble_os.h>
#define TIME_STARTED_WORKING 0
#define TIME_WORKED_TODAY 1

Window *window;
TextLayer *text_layer;
char *counter; 
char *time_str;

/**
 * returns 0 if not found
 */
int persist_get_int(const uint32_t key) {
  return persist_exists(key) ? persist_read_int(key) : 0;
}
void persist_incr_int(const uint32_t key, uint32_t increment_by) {
  persist_write_int(key, persist_get_int(key) + increment_by);
}

void toggle_start_stop(ClickRecognizerRef recognizer, void *context) {
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Adding 1!");
  //strcat(counter, "1");
  bool already_logging = persist_get_int(TIME_STARTED_WORKING) > 0;
  if (already_logging) {
    int32_t start = persist_read_int(TIME_STARTED_WORKING);
    int32_t amt_worked_now = time(0) - start;
    // add to total time you've worked today
    persist_incr_int(TIME_WORKED_TODAY, amt_worked_now);
    // stop counting
    persist_write_int(TIME_STARTED_WORKING, 0);
  } else {
    // start counting
    // record start time
    persist_write_int(TIME_STARTED_WORKING, time(0));
  }
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  // TODO FIGURE OUT TIME FROM HERE
  // time_t is just an integer / timestamp
  // struct tm is an actual object.
  // strftime wants a struct tm
  // but we want to store timestamps so we can subtract/divide.
  int32_t time_started_working = persist_get_int(TIME_STARTED_WORKING);
  if (time_started_working > 0) {
    time_t time_worked = time(0) - time_started_working;
    int hours_worked = time_worked / 3600;
    int minutes_worked = (time_worked / 60) % 60;
    int seconds_worked = time_worked % 60;
    snprintf(time_str, 25, "so far: \n %02d:%02d:%02d", 
      hours_worked, minutes_worked, seconds_worked);
    text_layer_set_text(text_layer, time_str);
  } else {
    text_layer_set_text(text_layer, "click select to start recording");
  }
}

void config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, toggle_start_stop);
}

void handle_init(void) {
	// Create a window and text layer
	window = window_create();
  counter = (char*) malloc(25*sizeof(char));
  time_str = (char*) malloc(25*sizeof(char));
	text_layer = text_layer_create(GRect(0, 0, 144, 154));
	
	// Set the text, font, and text alignment
  strcpy(counter, "Counta!");
	text_layer_set_text(text_layer, counter);
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	
	// Add the text layer to the window
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));
  // I expected to be able to write:
  // add_layer_to_window(window, layer);

	// Push the window
	window_stack_push(window, true);
  
  // subscribe to things
  window_set_click_config_provider(window, config_provider);
  tick_timer_service_subscribe(SECOND_UNIT, handle_minute_tick);


	
	// App Logging!
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Just pushed a window!");
}

void handle_deinit(void) {
	// Destroy the text layer
	text_layer_destroy(text_layer);
	
	// Destroy the window
	window_destroy(window);
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}
