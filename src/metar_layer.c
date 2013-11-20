#include <pebble.h>
#include "util.h"
#include "metar_layer.h"
#include "config.h"

static int font_options[] = {RESOURCE_ID_FUTURA_13, RESOURCE_ID_FUTURA_14, RESOURCE_ID_FUTURA_16, RESOURCE_ID_FUTURA_18, RESOURCE_ID_FUTURA_22, RESOURCE_ID_FUTURA_26};
static int font_options_count = sizeof(font_options) / sizeof(font_options[0]);

MetarLayer metar_layer_create(MetarLayer* metar_layer, GRect frame) {
	layer_create(metar_layer->layer, frame);

	// Add text layer
	text_layer_create(GRect(0, 0, frame.size.w, frame.size.h));
	text_layer_set_text_alignment(&metar_layer->text_layer, GTextAlignmentCenter);
	layer_add_child(&metar_layer->layer, &metar_layer->text_layer.layer);
}

void metar_layer_set_text(MetarLayer* metar_layer, char * text) {
	memcpy(metar_layer->metar_str, text, 90);
	text_layer_set_text(&metar_layer->text_layer, metar_layer->metar_str);
	maximize_font_and_frame_size(metar_layer);
}

void set_text_size_from_index(MetarLayer* metar_layer, int index){	
	ResHandle rh = resource_get_handle(font_options[index]);
	text_layer_set_font(&metar_layer->text_layer, fonts_load_custom_font(rh));
}

void maximize_font_and_frame_size(MetarLayer* metar_layer){
	GContext* ctx = app_get_current_graphics_context();
	int current_font_index = 0;
	set_text_size_from_index(metar_layer, current_font_index);
	// Fit layer to text at default font size
	metar_layer_fit_to_text(ctx, metar_layer);
	// Get layer size after resize
	GRect layer_frame = layer_get_frame(&metar_layer->layer);
	
	// If size is less than the max, try to increase it
	while (current_font_index < font_options_count &&
		   layer_frame.origin.y > METAR_MAX_Y_POS &&
		   layer_frame.size.h < METAR_MAX_Y_POS){
		// try to increase
		set_text_size_from_index(metar_layer, ++current_font_index);
		// resize again
		metar_layer_fit_to_text(ctx, metar_layer);
		// Get layer size after resize
		layer_frame = layer_get_frame(&metar_layer->layer);
	}

	if(current_font_index > 0){
		// While loop usually overshoots the font size by one, 
		set_text_size_from_index(metar_layer, --current_font_index);
		metar_layer_fit_to_text(ctx, metar_layer);
	}
}

void metar_layer_fit_to_text(GContext* ctx, MetarLayer* metar_layer) {
	GSize text_size = text_layer_get_max_used_size(ctx, &metar_layer->text_layer);
	static int bottom_padding = 3;

	GSize new_size = {.w = TOTAL_FRAME.size.w, .h = text_size.h + bottom_padding};
	GRect new_frame = {
						{
							.x = TOTAL_FRAME.origin.x, 
							.y = TOTAL_FRAME.size.h - new_size.h
						}, 
						{
							.w = TOTAL_FRAME.size.w, 
							.h = new_size.h
						}
	};
	layer_set_frame((Layer*)&metar_layer->layer, new_frame);
}

void metar_layer_deinit(MetarLayer* metar_layer) {
}

