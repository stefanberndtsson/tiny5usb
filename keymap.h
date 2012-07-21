#ifndef __KEYMAP_H__
#define __KEYMAP_H__

int scancodes[128] = {
  0, // 0x00
  120, // STOP
  129, // 0x02 VOLUME DOWN
  121, // AGAIN
  128, // 0x04 VOLUME UP
  58, // F1
  59, // F2
  67, // F10
  60, // F3
  68, // F11
  61, // F4
  69, // F12
  62, // F5
  230, // 0x0D RALT
  63, // F6
  0, // 0x0F
  64, // F7
  65, // F8
  66, // F9
  226, // 0x13 LALT
  82, // 0x14 UP
  72, // PAUSE
  70, // PrScr
  71, // Scroll Lock
  80, // 0x18 LEFT
  163, // PROPS
  122, // UNDO
  81, // 0x1B DOWN
  79, // 0x1C RIGHT
  41, // ESC
  30, // 1
  31, // 2
  32, // 3
  33, // 4
  34, // 5
  35, // 6
  36, // 7
  37, // 8
  38, // 9
  39, // 0
  45, // -
  46, // =
  53, // `
  42, // BACKSPACE
  73, // 0x2C INSERT
  127, // VOLUME OFF (=?)
  84, // KP /
  85, // KP *
  102, // 0x30 POWER OFF
  119, // 0x31 FRONT (SELECT)
  99, // 0x32 KP DEL
  124, // COPY
  74, // 0x34 HOME
  43, // TAB
  20, // Q
  26, // W
  8, // E
  21, // R
  23, // T
  28, // Y
  24, // U
  12, // I
  18, // O
  19, // P
  47, // 0x40 [
  48, // 0x41 ]
  76, // 0x42 DELETE
  101, // 0x43 COMPOSE (APPLICATION/MENU)
  95, // 0x44 KP HOME
  96, // 0x45 KP UP
  97, // 0x46 KP PGUP
  86, // 0x47 KP -
  116, // 0x48 OPEN (EXECUTE)
  125, // 0x49 PASTE
  77, // 0x4A END
  0, // 0x4B
  224, // 0x4C LCTRL
  4, // A
  22, // S
  7, // D
  9, // F
  10, // G
  11, // H
  13, // J
  14, // K
  15, // L
  51, // 0x56 ;
  52, // 0x57 '
  49, // 0x58 (\)
  40, // 0x59 RETURN
  88, // 0x5A KP ENTER
  92, // 0x5B KP LEFT
  93, // 0x5C KP 5
  94, // 0x5D KP RIGHT
  98, // 0x5E KP INS
  126, // 0x5F FIND
  75, // 0x60 PGUP
  123, // 0x61 CUT
  83, // 0x62 NUMLOCK
  225, // 0x63 LSHIFT
  29, // Z
  27, // X
  6, // C
  25, // V
  5, // B
  17, // N
  16, // M
  54, // 0x6B ,
  55, // 0x6C .
  56, // 0x6D /
  229, // 0x6E RSHIFT
  0, // 0x6F (LINE FEED ??)
  89, // 0x70 KP END
  90, // 0x71 KP DOWN
  91, // 0x72 KP PGDOWN
  0, // 0x73
  0, // 0x74
  0, // 0x75
  117, // 0x76 HELP
  57, // 0x77 CAPSLOCK
  227, // 0x78 LMETA
  44, // 0x79 SPACE
  231, // 0x7A RMETA
  78, // 0x7B PGDOWN
  0, // 0x7C
  87, // 0x7D KP +
  0, // 0x7E
  0, // 0x7F
};


#endif
