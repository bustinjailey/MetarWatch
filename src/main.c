#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
	
#include "http.h"
#include "util.h"
#include "time_layer.h"
#include "metar_layer.h"
#include "config.h"

#define MY_UUID { 0x91, 0x41, 0xB6, 0x28, 0xBC, 0x89, 0x49, 0x8E, 0xB1, 0x47, 0x04, 0x9F, 0x49, 0xC0, 0x99, 0xAD }
PBL_APP_INFO(MY_UUID,
             "Metar Watch", "Justin Bailey",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

// POST variables
#define LATITUDE_KEY 1
#define LONGITUDE_KEY 2

// Received variables
#define METAR_RESULT_KEY 1
#define HTTP_COOKIE 1949327671

static Window window;
static TimeLayer time_layer;
static MetarLayer metar_layer;

static int our_latitude, our_longitude;
static bool located;

void request_metar();
void handle_timer(AppContextRef app_ctx, AppTimerHandle handle, uint32_t cookie);

void failed(int32_t cookie, int http_status, void* context) {
	if(cookie == 0) {
		app_log(2, "main.c", 36, itoa(cookie));
		app_log(2, "main.c", 36, itoa(http_status));
		metar_layer_display_errortext(&metar_layer, "fail");
	}
}

void success(int32_t cookie, int http_status, DictionaryIterator* received, void* context) {
	Tuple* data_tuple = dict_find(received, METAR_RESULT_KEY);
	if(data_tuple) {
		metar_layer_set_text(&metar_layer, (char*)data_tuple->value);
	}
}

void reconnect(void* context) {
	request_metar();
}

void handle_tick(AppContextRef app_ctx, PebbleTickEvent *event) {
	time_layer_set_time(&time_layer, *(event->tick_time));
}

void set_timer(AppContextRef ctx) {
	app_timer_send_event(ctx, 1740000, 1);
}

void location(float latitude, float longitude, float altitude, float accuracy, void* context) {
	// Fix the floats
	our_latitude = latitude * 10000;
	our_longitude = longitude * 10000;
	located = true;
	request_metar();
	set_timer((AppContextRef)context);
}

void handle_init(AppContextRef ctx) {
	resource_init_current_app(&APP_RESOURCES);
	window_init(&window, "Metar Watch");
	window_stack_push(&window, true /* Animated */);
	window_set_fullscreen(&window, true);
	
	// Add time layer.
	time_layer_init(&time_layer, GPoint(0, 0));
	layer_add_child(&window.layer, &time_layer.layer);
	// Set initial time.
	PblTm time;
	get_time(&time);
	time_layer_set_time(&time_layer, time);
	// Add metar layer
	metar_layer_init(&metar_layer, GPoint(0, 100));
	layer_add_child(&window.layer, &metar_layer.layer);
	
	http_register_callbacks((HTTPCallbacks){
		.failure=failed,
		.success=success,
		.reconnect=reconnect,
		.location=location
	}, (void*)ctx);
	
	// Request weather
	located = false;
	request_metar();
}

void handle_deinit(AppContextRef ctx) {
	time_layer_deinit(&time_layer);
	metar_layer_deinit(&metar_layer);
}

void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
		.init_handler = &handle_init,
		.deinit_handler = &handle_deinit,
		.tick_info = {
			.tick_handler = &handle_tick,
			.tick_units = MINUTE_UNIT
		},
		.timer_handler = handle_timer,
		.messaging_info = {
			.buffer_sizes = {
				.inbound = 124,
				.outbound = 256,
			}
		}
	};
	app_event_loop(params, &handlers);
}

void handle_timer(AppContextRef ctx, AppTimerHandle handle, uint32_t cookie) {
	request_metar();
	// Update again in fifteen minutes.
	if(cookie)
		set_timer(ctx);
}

void request_metar() {
	if(!located) {
		http_location_request();
		return;
	}
	// Build the HTTP request
	DictionaryIterator *body;
	HTTPResult result = http_out_get("http://pwdb.kathar.in/pebble/weather3.php", HTTP_COOKIE, &body);
	if(result != HTTP_OK) {
		metar_layer_display_errortext(&metar_layer,  "!OK");
		return;
	}
	dict_write_int32(body, LATITUDE_KEY, our_latitude);
	dict_write_int32(body, LONGITUDE_KEY, our_longitude);

	// Send it.
	if(http_out_send() != HTTP_OK) {
		metar_layer_display_errortext(&metar_layer, "!OK");
		return;
	}
	else{
		metar_layer_display_errortext(&metar_layer, "OK!");
	}
}
