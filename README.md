# meishi2_action

meishi2のキーを押した際に、何かを実行するプログラム。


## 概要

以下の動作を行う。

一番左側のキー -> デフォルトマイクのミュート・アンミュート
一番右側のキー -> Apex Legends(r5apex.exe)の強制終了

真ん中の2つのキーは特にこのプログラムでアクションはしない。


## 設定

meishi2のqmkファームウェアは、F21～F24のキーとして設定する。


```.c
#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT( /* Base */
    KC_F21, KC_F22, KC_F23, KC_F24 \
  )
};

void matrix_init_user(void) {

}

void matrix_scan_user(void) {

}

void led_set_user(uint8_t usb_led) {

}
```

