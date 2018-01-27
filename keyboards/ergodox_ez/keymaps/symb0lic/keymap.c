#include QMK_KEYBOARD_H
#include "debug.h"
#include "action_layer.h"

#define _______ KC_TRNS

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[0] = LAYOUT_ergodox_visual(
     _______, KC_1  , KC_2  , KC_3  , KC_4  , KC_5  ,_______,       _______, KC_6  , KC_7  , KC_8  , KC_8  , KC_9  ,_______,
     _______, KC_Q  , KC_W  , KC_E  , KC_R  , KC_T  ,_______        _______, KC_Y  , KC_U  , KC_I  , KC_O  , KC_P  ,_______,
     _______, KC_A  , KC_S  , KC_D  , KC_F  , KC_G  ,_______,       _______, KC_H  , KC_J  , KC_K  , KC_L  ,KC_QUOT,_______,
     _______, KC_Z  , KC_X  , KC_C  , KC_V  , KC_B  ,_______,       _______, KC_N  , KC_M  ,KC_COMM, KC_DOT,KC_SLSH,_______,
     _______,_______,_______,_______,_______,                               KC_MINS, KC_EQL,KC_LBRC,KC_RBRC,KC_BSLS,
					     _______,_______,       _______,_______,
						     _______,       _______,
				     _______,_______,_______,       _______,_______,_______),

[1] = LAYOUT_ergodox_visual(
     _______, KC_F1 , KC_F2 , KC_F3 , KC_F4 , KC_F5 , KC_F6 ,        KC_F7 , KC_F8 , KC_F9 , KC_F10, KC_F11, KC_F12,_______,
     _______,_______,_______,KC_HOME, KC_END,KC_PGUP,_______,       _______, KC_7  , KC_8  , KC_9  ,KC_LBRC,KC_RBRC,_______,
     _______,KC_LEFT,KC_DOWN, KC_UP ,KC_RGHT,KC_PGDN,_______,       _______, KC_4  , KC_5  , KC_6  ,KC_PAST,KC_SCLN,_______,
     _______,KC_MS_L,KC_MS_D,KC_MS_U,KC_MS_R,KC_BTN1,_______,       _______, KC_1  , KC_2  , KC_2  ,KC_PDOT,KP_PSLS,_______,
     _______,KC_WH_L,KC_WH_D,KC_WH_U,KC_WH_R,                               KC_PMNS,KC_PPLS, KC_0  ,KC_PEQL, KC_GRV,
					     _______,_______,       _______,_______,
						     _______,       _______,
				     KC_BTN2,_______,_______,       _______,_______,_______)
};

const uint16_t PROGMEM fn_actions[] = {
    [1] = ACTION_LAYER_TAP_TOGGLE(SYMB)                // FN1 - Momentary Layer 1 (Symbols)
};

const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt)
{
  // MACRODOWN only works in this function
  switch(id) {
    case 0:
      if (record->event.pressed) {
        register_code(KC_RSFT);
      } else {
        unregister_code(KC_RSFT);
      }
      break;
  }
  return MACRO_NONE;
};

// Runs just one time when the keyboard initializes.
void matrix_init_user(void) {

};

// Runs constantly in the background, in a loop.
void matrix_scan_user(void) {

  uint8_t layer = biton32(layer_state);

  ergodox_board_led_off();
  ergodox_right_led_1_off();
  ergodox_right_led_2_off();
  ergodox_right_led_3_off();
  switch (layer) {
    // TODO: Make this relevant to the ErgoDox EZ.
    case SYMB:
      ergodox_right_led_1_on();
      break;
    case MDIA:
      ergodox_right_led_2_on();
      break;
    default:
      // none
      break;
  }

};
