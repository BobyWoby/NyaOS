#include <drivers/keyboard.h>
#include <drivers/ps2.h>
#include <kernel/slab.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define ACK 0xfa
#define RESEND 0xfe
#define PORT 0

typedef enum { READY = 0, BREAK, EXTENDED, EXTENDED_BREAK } KEYBOARD_STATE;

// Scan code set 2 make codes; unspecified entries are KEY_UNKNOWN.
static const KEY normal_keys[256] = {
    [0x01] = F9,        [0x03] = F5,      [0x04] = F3,    [0x05] = F1,         [0x06] = F2,
    [0x07] = F12,       [0x09] = F10,     [0x0A] = F8,    [0x0B] = F6,         [0x0C] = F4,
    [0x0D] = TAB,       [0x0E] = TILDE,   [0x11] = LALT,  [0x12] = LSHIFT,     [0x14] = LCTRL,
    [0x15] = Q,         [0x16] = NUM1,    [0x1A] = Z,     [0x1B] = S,          [0x1C] = A,
    [0x1D] = W,         [0x1E] = NUM2,    [0x21] = C,     [0x22] = X,          [0x23] = D,
    [0x24] = E,         [0x25] = NUM4,    [0x26] = NUM3,  [0x29] = SPACE,      [0x2A] = V,
    [0x2B] = F,         [0x2C] = T,       [0x2D] = R,     [0x2E] = NUM5,       [0x31] = N,
    [0x32] = B,         [0x33] = H,       [0x34] = G,     [0x35] = Y,          [0x36] = NUM6,
    [0x3A] = M,         [0x3B] = J,       [0x3C] = U,     [0x3D] = NUM7,       [0x3E] = NUM8,
    [0x41] = COMMA,     [0x42] = K,       [0x43] = I,     [0x44] = O,          [0x45] = NUM0,
    [0x46] = NUM9,      [0x49] = PERIOD,  [0x4A] = SLASH, [0x4B] = L,          [0x4C] = SEMICOLON,
    [0x4D] = P,         [0x4E] = DASH,    [0x52] = QUOTE, [0x54] = LBRACKET,   [0x55] = EQUAL,
    [0x58] = CAPSLOCK,  [0x59] = RSHIFT,  [0x5A] = RET,   [0x5B] = RBRACKET,   [0x5D] = BACKSLASH,
    [0x66] = BACK,      [0x69] = NP1,     [0x6B] = NP4,   [0x6C] = NP7,        [0x70] = NP0,
    [0x71] = NPDECIMAL, [0x72] = NP2,     [0x73] = NP5,   [0x74] = NP6,        [0x75] = NP8,
    [0x76] = ESC,       [0x77] = NUMLOCK, [0x78] = F11,   [0x79] = ADD,        [0x7A] = NP3,
    [0x7B] = SUBTRACT,  [0x7C] = MULT,    [0x7D] = NP9,   [0x7E] = SCROLLLOCK, [0x83] = F7,
};

// Indexed by the byte following E0 (or E0 F0 for releases).
// Print Screen and Pause require separate multi-byte sequence handling.
static const KEY extended_keys[256] = {
    [0x10] = WWW_SEARCH,    [0x11] = RALT,         [0x14] = RCTRL,       [0x15] = PREV_TRACK,
    [0x18] = WWW_FAVORITES, [0x1F] = LSYSTEM,      [0x20] = WWW_REFRESH, [0x21] = VOLUME_DOWN,
    [0x23] = MUTE,          [0x27] = RSYSTEM,      [0x28] = WWW_STOP,    [0x2B] = CALCULATOR,
    [0x2F] = MENU,          [0x30] = WWW_FORWARD,  [0x32] = VOLUME_UP,   [0x34] = PLAY_PAUSE,
    [0x37] = POWER,         [0x38] = WWW_BACK,     [0x3A] = WWW_HOME,    [0x3B] = MEDIA_STOP,
    [0x3F] = SLEEP,         [0x40] = MY_COMPUTER,  [0x48] = EMAIL,       [0x4A] = DIV,
    [0x4D] = NEXT_TRACK,    [0x50] = MEDIA_SELECT, [0x5A] = NPENTER,     [0x5E] = WAKE,
    [0x69] = END,           [0x6B] = LEFT,         [0x6C] = HOME,        [0x70] = INSERT,
    [0x71] = DELETE,        [0x72] = DOWN,         [0x74] = RIGHT,       [0x75] = UP,
    [0x7A] = PAGEDOWN,      [0x7D] = PAGEUP,
};

KEYBOARD_STATE state = READY;
bool key_states[PAUSE + 1];
bool caps_lock = false;
void (*key_down_handler)(KEY);
void (*key_up_handler)(KEY);

void default_handler(KEY kc) {
    (void)kc;
    return;
}

void handle_keyboard() {
    // printf("interrupt\n");
    uint8_t scancode;
    // scancode = recv();  // read + re-arm IRQ1
    scancode = recv();
    switch (state) {
        case READY:
            if (scancode == 0xf0) {
                state = BREAK;
            } else if (scancode == 0xe0) {
                state = EXTENDED;
            } else {
                // convert scancode to index
                KEY keycode = normal_keys[scancode];
                key_states[keycode] = true;
                if (keycode == CAPSLOCK) caps_lock = !caps_lock;
                if (key_down_handler) key_down_handler(keycode);
            }
            break;
        case BREAK: {
            KEY keycode = normal_keys[scancode];
            key_states[keycode] = false;
            if (key_up_handler) key_up_handler(keycode);
            state = READY;
            break;
        }
        case EXTENDED: {
            KEY keycode = extended_keys[scancode];
            key_states[keycode] = true;
            if (key_down_handler) key_down_handler(keycode);
            break;
        }
        case EXTENDED_BREAK: {
            KEY keycode = extended_keys[scancode];
            key_states[keycode] = false;
            if (key_up_handler) key_up_handler(keycode);
            state = READY;
            break;
        } break;
    }
    // printf("scancode: %#x\n", scancode);
}

void kb_enable_scanning() {
    key_up_handler = default_handler;
    key_down_handler = default_handler;
    // initialize keyboard structures
    send(0xf0, PORT);
    uint8_t res = 0;
    uint8_t scancode;

    res = poll();
    if (res == 0xfa) {
        send(0, PORT);
        if (poll() == 0xfa) {
            scancode = poll();
            printf("Enabling scanning on keyboard with scancode set %x\n", scancode);
        }
    }

    res = 0;
    do {
        send(0xf4, PORT);
    } while ((res = poll()) == 0xfe);

    state = READY;

    // enable interrupts for the ps/2 device 1
    dev1_enable_irq();
}
