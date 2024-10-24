#include <math.h>

#define SCREEN_WIDTH_PX 480
#define SCREEN_HEIGHT_PX 240

LV_IMG_DECLARE(setonhalllogo.c);

/* Debug class for logging */
typedef struct {
    lv_obj_t *log_panel;
    lv_obj_t *log_label;
    lv_obj_t *scroll_up_btn;
    lv_obj_t *scroll_down_btn;
    lv_obj_t *clear_log_btn;      // Clear Log button
    char log_buffer[1024];
    bool is_log_visible;
} Debug;

/* Global Debug instance for logging */
Debug *global_debug;

/* Step 1: Define a struct to act as the MainUI "class" */
typedef struct MainUI {
    lv_obj_t *main_screen;
    lv_obj_t *toggle_btn;
    lv_obj_t *preset_toggle_btn;
    lv_obj_t *log_toggle_btn;
    lv_obj_t *color_label;
    lv_obj_t *preset_label;
    lv_style_t red_style;
    lv_style_t blue_style;
    char *preset_sentence;  // Store the current preset sentence
    void (*initialize)(struct MainUI*);
    void (*create_main_screen)(struct MainUI*);
    Debug debug;
} MainUI;

/* Predefined sentences */
const char *preset_sentences[] = {
    "Hello, World!",
    "VEX Robotics rocks!",
    "Let's win this!",
    "Good luck!",
};
int current_sentence_index = 0;

/* Function declarations */
void MainUI_initialize(MainUI *ui);
void MainUI_create_main_screen(MainUI *ui);
void MainUI_toggle_btn_event_handler(lv_event_t *e);
void MainUI_preset_toggle_event_handler(lv_event_t *e);
void MainUI_log_toggle_event_handler(lv_event_t *e);
void Debug_scroll_up(lv_event_t *e);
void Debug_scroll_down(lv_event_t *e);
void Debug_clear_log(lv_event_t *e);  // Clear log function
void update_label_display(MainUI *ui);
void Debug_Log(const char *message);  // Update function to be used globally

/* This function initializes the UI by displaying the Seton Hall logo */
void MainUI_initialize(MainUI *ui) {
    // Create the intro screen with the Seton Hall logo
    lv_obj_t *img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, &setonhalllogo);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

    // Transition to the main screen after a delay
    pros::delay(3000);
    ui->create_main_screen(ui);  // Transition to the main screen
}

/* Event handler for the toggle button to change screen color */
void MainUI_toggle_btn_event_handler(lv_event_t *e) {
    MainUI *ui = (MainUI *)lv_event_get_user_data(e);
    lv_obj_t *btn = lv_event_get_target(e);

    if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {
        lv_obj_add_style(ui->main_screen, &ui->red_style, 0);
        lv_obj_remove_style(ui->main_screen, &ui->blue_style, 0);
        lv_label_set_text(ui->color_label, "RED");
    } else {
        lv_obj_add_style(ui->main_screen, &ui->blue_style, 0);
        lv_obj_remove_style(ui->main_screen, &ui->red_style, 0);
        lv_label_set_text(ui->color_label, "BLUE");
    }

    Debug_Log("Toggled background color.");
}

/* Event handler for preset toggle button */
void MainUI_preset_toggle_event_handler(lv_event_t *e) {
    MainUI *ui = (MainUI *)lv_event_get_user_data(e);
    current_sentence_index = (current_sentence_index + 1) % (sizeof(preset_sentences) / sizeof(preset_sentences[0]));
    ui->preset_sentence = (char *)preset_sentences[current_sentence_index];

    update_label_display(ui);
    Debug_Log("Changed preset sentence.");
}

/* Event handler for the log toggle button */
void MainUI_log_toggle_event_handler(lv_event_t *e) {
    MainUI *ui = (MainUI *)lv_event_get_user_data(e);

    if (ui->debug.is_log_visible) {
        lv_obj_add_flag(ui->debug.log_panel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui->debug.scroll_up_btn, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui->debug.scroll_down_btn, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui->debug.clear_log_btn, LV_OBJ_FLAG_HIDDEN);  // Hide clear button
        ui->debug.is_log_visible = false;
    } else {
        lv_obj_clear_flag(ui->debug.log_panel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui->debug.scroll_up_btn, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui->debug.scroll_down_btn, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui->debug.clear_log_btn, LV_OBJ_FLAG_HIDDEN);  // Show clear button
        ui->debug.is_log_visible = true;
    }
}

/* Scroll up the debug log */
void Debug_scroll_up(lv_event_t *e) {
    lv_obj_scroll_by(global_debug->log_panel, 0, 20, LV_ANIM_ON);
}

/* Scroll down the debug log */
void Debug_scroll_down(lv_event_t *e) {
    lv_obj_scroll_by(global_debug->log_panel, 0, -20, LV_ANIM_ON);
}

/* Clear the log */
void Debug_clear_log(lv_event_t *e) {
    memset(global_debug->log_buffer, 0, sizeof(global_debug->log_buffer));  // Clear the log buffer
    lv_label_set_text(global_debug->log_label, "");  // Clear the log label display
    Debug_Log("Log cleared.");
}

/* Update the label display with text */
void update_label_display(MainUI *ui) {
    lv_label_set_text(ui->preset_label, ui->preset_sentence);
}

/* Log a message to the debug panel */
void Debug_Log(const char *message) {
    // Append the new message to the log buffer
    strncat(global_debug->log_buffer, message, sizeof(global_debug->log_buffer) - strlen(global_debug->log_buffer) - 1);
    strncat(global_debug->log_buffer, "\n", sizeof(global_debug->log_buffer) - strlen(global_debug->log_buffer) - 1);

    // Update the log label with the new buffer content
    lv_label_set_text(global_debug->log_label, global_debug->log_buffer);
}

/* This function creates the main screen with a toggle button, color label, and preset sentence label */
void MainUI_create_main_screen(MainUI *ui) {
    // Initialize styles
    lv_style_init(&ui->red_style);
    lv_style_set_bg_color(&ui->red_style, lv_color_hex(0xFF0000));

    lv_style_init(&ui->blue_style);
    lv_style_set_bg_color(&ui->blue_style, lv_color_hex(0x004488));

    // Set global debug instance
    global_debug = &ui->debug;

    // Create a new screen object
    ui->main_screen = lv_obj_create(NULL);
    lv_obj_add_style(ui->main_screen, &ui->blue_style, 0);
    lv_scr_load(ui->main_screen);

    // Create a label to display the color
    ui->color_label = lv_label_create(ui->main_screen);
    lv_label_set_text(ui->color_label, "BLUE");
    lv_obj_set_style_text_color(ui->color_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(ui->color_label, &lv_font_montserrat_48, 0);
    lv_obj_align(ui->color_label, LV_ALIGN_CENTER, 0, -30);

    // Create a label for preset sentences
    ui->preset_label = lv_label_create(ui->main_screen);
    lv_label_set_text(ui->preset_label, preset_sentences[current_sentence_index]);
    lv_obj_set_style_text_color(ui->preset_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(ui->preset_label, &lv_font_montserrat_20, 0);
    lv_obj_align(ui->preset_label, LV_ALIGN_CENTER, 0, 40);

    // Create a toggle button for background color
    ui->toggle_btn = lv_btn_create(ui->main_screen);
    lv_obj_set_size(ui->toggle_btn, 70, 35);
    lv_obj_align(ui->toggle_btn, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    lv_obj_add_flag(ui->toggle_btn, LV_OBJ_FLAG_CHECKABLE);

    lv_obj_t *label = lv_label_create(ui->toggle_btn);
    lv_label_set_text(label, "Toggle");
    lv_obj_center(label);

    lv_obj_add_event_cb(ui->toggle_btn, MainUI_toggle_btn_event_handler, LV_EVENT_VALUE_CHANGED, ui);

    // Create a toggle button for preset sentences
    ui->preset_toggle_btn = lv_btn_create(ui->main_screen);
    lv_obj_set_size(ui->preset_toggle_btn, 50, 25);
    lv_obj_align(ui->preset_toggle_btn, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_obj_add_event_cb(ui->preset_toggle_btn, MainUI_preset_toggle_event_handler, LV_EVENT_CLICKED, ui);

    lv_obj_t *preset_label = lv_label_create(ui->preset_toggle_btn);
    lv_label_set_text(preset_label, "Preset");
    lv_obj_center(preset_label);

    // Create the log panel
    ui->debug.log_panel = lv_obj_create(ui->main_screen);
    lv_obj_set_size(ui->debug.log_panel, 300, 200);
    lv_obj_align(ui->debug.log_panel, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_obj_set_style_bg_color(ui->debug.log_panel, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_width(ui->debug.log_panel, 2, 0);
    lv_obj_set_style_border_color(ui->debug.log_panel, lv_color_hex(0xFFFFFF), 0);
    lv_obj_add_flag(ui->debug.log_panel, LV_OBJ_FLAG_HIDDEN);

    // Create a label inside the log panel
    ui->debug.log_label = lv_label_create(ui->debug.log_panel);
    lv_label_set_text(ui->debug.log_label, "Log Initialized:\n");
    lv_obj_set_style_text_color(ui->debug.log_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(ui->debug.log_label, LV_ALIGN_TOP_LEFT, 5, 5);

    // Initialize the log buffer
    memset(ui->debug.log_buffer, 0, sizeof(ui->debug.log_buffer));
    strncat(ui->debug.log_buffer, "Log Initialized:\n", sizeof(ui->debug.log_buffer) - 1);

    // Add a button to toggle the log panel visibility
    ui->log_toggle_btn = lv_btn_create(ui->main_screen);
    lv_obj_set_size(ui->log_toggle_btn, 80, 30);
    lv_obj_align(ui->log_toggle_btn, LV_ALIGN_BOTTOM_LEFT, 10, -60);
    lv_obj_add_event_cb(ui->log_toggle_btn, MainUI_log_toggle_event_handler, LV_EVENT_CLICKED, ui);

    lv_obj_t *log_label = lv_label_create(ui->log_toggle_btn);
    lv_label_set_text(log_label, "Show Log");
    lv_obj_center(log_label);

    // Add scroll up button for the log
    ui->debug.scroll_up_btn = lv_btn_create(ui->main_screen);
    lv_obj_set_size(ui->debug.scroll_up_btn, 50, 25);
    lv_obj_align(ui->debug.scroll_up_btn, LV_ALIGN_BOTTOM_LEFT, 100, -60);
    lv_obj_add_event_cb(ui->debug.scroll_up_btn, Debug_scroll_up, LV_EVENT_CLICKED, NULL);
    lv_obj_add_flag(ui->debug.scroll_up_btn, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *scroll_up_label = lv_label_create(ui->debug.scroll_up_btn);
    lv_label_set_text(scroll_up_label, "Up");
    lv_obj_center(scroll_up_label);

    // Add scroll down button for the log
    ui->debug.scroll_down_btn = lv_btn_create(ui->main_screen);
    lv_obj_set_size(ui->debug.scroll_down_btn, 50, 25);
    lv_obj_align(ui->debug.scroll_down_btn, LV_ALIGN_BOTTOM_LEFT, 100, -100);
    lv_obj_add_event_cb(ui->debug.scroll_down_btn, Debug_scroll_down, LV_EVENT_CLICKED, NULL);
    lv_obj_add_flag(ui->debug.scroll_down_btn, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *scroll_down_label = lv_label_create(ui->debug.scroll_down_btn);
    lv_label_set_text(scroll_down_label, "Down");
    lv_obj_center(scroll_down_label);

    // Add clear log button for the log
    ui->debug.clear_log_btn = lv_btn_create(ui->main_screen);
    lv_obj_set_size(ui->debug.clear_log_btn, 80, 30);
    lv_obj_align(ui->debug.clear_log_btn, LV_ALIGN_BOTTOM_LEFT, 10, -100);
    lv_obj_add_event_cb(ui->debug.clear_log_btn, Debug_clear_log, LV_EVENT_CLICKED, NULL);
    lv_obj_add_flag(ui->debug.clear_log_btn, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *clear_label = lv_label_create(ui->debug.clear_log_btn);
    lv_label_set_text(clear_label, "Clear Log");
    lv_obj_center(clear_label);

    // Log the initialization
    Debug_Log("Main screen created.");
}