#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "util.h"
#include "metar_layer.h"

#define MAX_HEIGHT 168
#define MAX_WIDTH 144
#define MAX_METAR_HEIGHT 84

static int font_options[] = {RESOURCE_ID_FUTURA_13, RESOURCE_ID_FUTURA_14, RESOURCE_ID_FUTURA_16, RESOURCE_ID_FUTURA_18, RESOURCE_ID_FUTURA_22, RESOURCE_ID_FUTURA_26};
static int font_options_count = sizeof(font_options) / sizeof(font_options[0]);

void metar_layer_init(MetarLayer* metar_layer, GRect frame) {
	layer_init(&metar_layer->layer, frame);
	
	// Add text layer
	text_layer_init(&metar_layer->text_layer, GRect(0, 0, frame.size.w, frame.size.h));
	text_layer_set_text_alignment(&metar_layer->text_layer, GTextAlignmentCenter);
	text_layer_set_font(&metar_layer->text_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_13)));
	layer_add_child(&metar_layer->layer, &metar_layer->text_layer.layer);
}

void metar_layer_set_text(MetarLayer* metar_layer, char * text) {
	memcpy(metar_layer->metar_str, text, 90);
	text_layer_set_text(&metar_layer->text_layer, metar_layer->metar_str);
	maximize_font_and_frame_size(metar_layer);
}

void maximize_font_and_frame_size(MetarLayer* metar_layer){
	GContext* ctx = app_get_current_graphics_context();		
	GRect layer_frame = layer_get_frame((Layer*)&metar_layer->layer);
	int current_font_index = 0;
	metar_layer_fit_to_text(ctx, metar_layer); // resize once, then go in to while to find max size
	while(layer_frame.origin.y > MAX_METAR_HEIGHT + 20 && current_font_index < font_options_count){
		current_font_index++;
		// if box size less than max, resize text to larger size.
		set_text_size_from_index(metar_layer, current_font_index);
		// fit to new text
		metar_layer_fit_to_text(ctx, metar_layer);
		// update layer_frame with new layer size
		layer_frame = layer_get_frame((Layer*)&metar_layer->layer);		
	}
}

void set_text_size_from_index(MetarLayer* metar_layer, int index){	
	ResHandle rh = resource_get_handle(font_options[index]);
	text_layer_set_font(&metar_layer->text_layer, fonts_load_custom_font(rh));
}

void metar_layer_fit_to_text(GContext* ctx, MetarLayer* metar_layer) {
	GSize text_size = text_layer_get_max_used_size(ctx, &metar_layer->text_layer);
	GRect layer_frame = layer_get_frame((Layer*)&metar_layer->layer);
	static int bottom_padding = 3;
	
	GSize new_size = {.w = MAX_WIDTH, .h = text_size.h + bottom_padding};
	GRect new_frame = {
						{
							.x = layer_frame.origin.x, 
							.y = MAX_HEIGHT - new_size.h
						}, 
						{
							.w = new_size.w, 
							.h = new_size.h
						}
	};
	
	layer_set_frame((Layer*)&metar_layer->layer, new_frame);
}

void metar_layer_deinit(MetarLayer* metar_layer) {
}

