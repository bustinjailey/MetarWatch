#include <pebble.h>

#include "util.h"
#include "time_layer.h"
#include "metar_layer.h"
#include "config.h"

#define MY_UUID { 0x91, 0x41, 0xB6, 0x28, 0xBC, 0x89, 0x49, 0x8E, 0xB1, 0x47, 0x04, 0x9F, 0x49, 0xC0, 0x99, 0xAD }

// POST variables
#define LATITUDE_KEY 1
#define LONGITUDE_KEY 2

// Received variables
#define METAR_RESULT_KEY 1
#define METAR_HTTP_COOKIE 1949327343
#define TIME_HTTP_COOKIE 1131038282

static Window *window;
static TextLayer *date_layer;
static TimeLayer *time_layer;
static MetarLayer *metar_layer;

GFont font_date;
GFont font_hour;
GFont font_minute;

static int *initial_minute;
static int *our_latitude, our_longitude;
static bool *located;
static GRect *time_frame;
static GRect *date_frame;
static GRect *metar_frame;

void request_metar();
void handle_timer(void *data);

void failed(int32_t cookie, int http_status, void* context) {
	if(cookie == 0) {
		metar_layer_set_text(metar_layer, itoa(http_status));
	}
	else{
		metar_layer_set_text(metar_layer, "other fail");
	}
}

void in_received_handler(DictionaryIterator *received, void *context) {
	Tuple* data_tuple = dict_find(received, METAR_RESULT_KEY);
	if(data_tuple) {
		metar_layer_set_text(metar_layer, (char*)data_tuple->value);
	}
	else{
		metar_layer_set_text(metar_layer, "No Data");
	}
}

void reconnect(void* context) {
	request_metar();
}

void handle_tick(AppContextRef app_ctx, PebbleTickEvent *event) {
	static char date_text[] = "XXX 00";
    static char hour_text[] = "00";
    static char minute_text[] = ":00";
	
    if (event->units_changed & DAY_UNIT)
    {		
	    string_format_time(date_text,
                           sizeof(date_text),
                           "%a %d",
                           event->tick_time);

		if (date_text[4] == '0') /* is day of month < 10? */
		{
            memmove(&date_text[4], &date_text[5], sizeof(date_text) - 1); // remove leading 0
		}
        text_layer_set_text(&date_layer, date_text);
    }

    if (clock_is_24h_style())
    {
        string_format_time(hour_text, sizeof(hour_text), "%H", event->tick_time);
		if (hour_text[0] == '0')
        {
            memmove(&hour_text[0], &hour_text[1], sizeof(hour_text) - 1); // remove leading 0
        }
    }
    else
    {
        string_format_time(hour_text, sizeof(hour_text), "%I", event->tick_time);
        if (hour_text[0] == '0')
        {
            memmove(&hour_text[0], &hour_text[1], sizeof(hour_text) - 1); // remove leading 0
        }
    }

    string_format_time(minute_text, sizeof(minute_text), ":%M", event->tick_time);
    time_layer_set_text(&time_layer, hour_text, minute_text);
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
    PebbleTickEvent tick;
    ResHandle res_d;
    ResHandle res_h;
	
	resource_init_current_app(&APP_RESOURCES);
	window = window_create();
	window_stack_push(&window, true /* Animated */);
	window_set_fullscreen(&window, true);
    window_set_background_color(&window, GColorBlack);
	
    res_d = resource_get_handle(RESOURCE_ID_FUTURA_18);
    res_h = resource_get_handle(RESOURCE_ID_FUTURA_CONDENSED_53);

    font_date = fonts_load_custom_font(res_d);
    font_hour = fonts_load_custom_font(res_h);
    font_minute = fonts_load_custom_font(res_h);
	
	// Individual frame sizes
	time_frame = (GRect(0, 2, TOTAL_FRAME.size.w, (TOTAL_FRAME.size.h)-6));
	date_frame = (GRect(0, 58, TOTAL_FRAME.size.w, (TOTAL_FRAME.size.h)-62));
	// try setting to full screen, resizing in set text function
	metar_frame = (GRect(0, 0, TOTAL_FRAME.size.w, TOTAL_FRAME.size.h));

	// Add time layer
    time_layer_create(time_layer, window.layer.frame);
    time_layer_set_text_color(&time_layer, GColorWhite);
    time_layer_set_background_color(&time_layer, GColorClear);
    time_layer_set_fonts(&time_layer, font_hour, font_minute);
    layer_set_frame(&time_layer.layer, time_frame);
    layer_add_child(&window.layer, &time_layer.layer);

	// Add date text layer
    text_layer_create(window.layer.frame);
    text_layer_set_text_color(&date_layer, GColorWhite);
    text_layer_set_background_color(&date_layer, GColorClear);
    text_layer_set_font(&date_layer, font_date);
    text_layer_set_text_alignment(&date_layer, GTextAlignmentCenter);
    layer_set_frame(&date_layer.layer, date_frame);
    layer_add_child(&window.layer, &date_layer.layer);

	// Set initial time.
	//tm time;
	//time = localtime(time(NULL));
    //tick.tick_time = &time;
    //tick.units_changed = SECOND_UNIT | MINUTE_UNIT | HOUR_UNIT | DAY_UNIT;
	//initial_minute = (time.tm_min % 30);
	//handle_tick(ctx, &tick);
	
	// Add metar layer
	metar_layer_create(metar_layer, metar_frame);
	layer_add_child(&window.layer, (Layer*)&metar_layer.layer);
	
	app_message_register_inbox_received(in_received_handler);
	
	// Request weather
	located = false;
	request_metar();
}

void handle_deinit(AppContextRef ctx) {
	fonts_unload_custom_font(font_date);
    fonts_unload_custom_font(font_hour);
    fonts_unload_custom_font(font_minute);
	
	app_message_deregister_callbacks(void);
	
	metar_layer_deinit(&metar_layer);
	
	layer_destroy(date_layer);
	layer_destroy(time_layer);
	layer_destroy(metar_layer);
	window_destroy(window;)
}

void main(void *params) {
	app_timer_register(1000, handle_timer, NULL);	
	tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
	app_message_open(124, 256);
	
	handle_init();
	app_event_loop(params, &handlers);
	handle_deinit();
}

void handle_timer(void *data) {
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
	HTTPResult result = http_out_get("http://bustinjailey-metar.herokuapp.com", METAR_HTTP_COOKIE, &body);
	if(result != HTTP_OK) {
		metar_layer_set_text(&metar_layer, "!HTTP_OK");
		return;
	}
	dict_write_int32(body, LATITUDE_KEY, our_latitude);
	dict_write_int32(body, LONGITUDE_KEY, our_longitude);

	// Send it.
	if(http_out_send() != HTTP_OK) {
		metar_layer_set_text(&metar_layer, "!HTTP_OK");
		return;
	}
}
