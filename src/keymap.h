#ifndef _KEYMAP_H_
#define _KEYMAP_H_

#include "debug.h"

int matrix2pos[] = {1,  3,  5,  7,  9,  11, 13, 56, 2,  4,  6,  8,  10,
                      12, 14, 57, 15, 17, 19, 21, 23, 25, 27, 58, 16, 18,
                      20, 22, 24, 26, 28, 59, 29, 31, 33, 35, 37, 39, 41,
                      60, 30, 32, 34, 36, 38, 40, 54, 61, 42, 44, 46, 48,
                      50, 52, 55, 62, 43, 45, 47, 49, 51, 53, 64, 63};

// clang-format off
enum KEY_CODE
{
    /*------------------------- HID report data -------------------------*/
    LEFT_CTRL = -8,LEFT_SHIFT = -7,LEFT_ALT = -6,LEFT_GUI = -5,
    RIGHT_CTRL = -4,RIGHT_SHIFT = -3,RIGHT_ALT = -2,RIGHT_GUI = -1,

    KEY_NO_EVENT = 0,ERROR_ROLL_OVER,POST_FAIL,ERROR_UNDEFINED,
    A,B,C,D,E,F,G,H,I,J,K,L,M,
    N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
    NUM_1/*1!*/,NUM_2/*2@*/,NUM_3/*3#*/,NUM_4/*4$*/,NUM_5/*5%*/,
    NUM_6/*6^*/,NUM_7/*7&*/,NUM_8/*8**/,NUM_9/*9(*/,NUM_0/*0)*/,
    ENTER,ESC,BACKSPACE,TAB,SPACE,
    MINUS/*-_*/,EQUAL/*=+*/,LEFT_U_BRACE/*[{*/,RIGHT_U_BRACE/*]}*/,
    BACKSLASH/*\|*/,NONE_US/**/,SEMI_COLON/*;:*/,QUOTE/*'"*/,
    GRAVE_ACCENT/*`~*/,COMMA/*,<*/,PERIOD/*.>*/,SLASH/*/?*/,
    CAP_LOCK,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,
    PRINT,SCROLL_LOCK,PAUSE,INSERT,HOME,PAGE_UP,DELETE,END,PAGE_DOWN,
    RIGHT_ARROW,LEFT_ARROW,DOWN_ARROW,UP_ARROW,PAD_NUM_LOCK,
    PAD_SLASH,PAD_ASTERISK,PAD_MINUS,PAD_PLUS,PAD_ENTER,
    PAD_NUM_1,PAD_NUM_2,PAD_NUM_3,PAD_NUM_4,PAD_NUM_5,
    PAD_NUM_6,PAD_NUM_7,PAD_NUM_8,PAD_NUM_9,PAD_NUM_0,
    PAD_PERIOD , NONUS_BACKSLASH,APPLICATION,POWER,PAD_EQUAL,
    F13,F14,F15,F16,F17,F18,F19,F20,F21,F22,F23,F24, EXECUTE,
    HELP,MENU,SELECT,STOP,AGAIN,UNDO,CUT,COPY,PASTE,FIND,MUTE,VOLUME_UP,VOLUME_DOWN,
    FN = 1000,
    PROFILE1,
    PROFILE2,
    PROFILE3,
    PROFILE4,
    /*------------------------- HID report data -------------------------*/
};

const uint16_t g_default_keymap[5][64] = {
    {
        ESC/*0*/, NUM_1/*1*/, NUM_2/*2*/, NUM_3/*3*/, NUM_4/*4*/, NUM_5/*5*/, NUM_6/*6*/, NUM_7/*7*/, NUM_8/*8*/, NUM_9/*9*/, NUM_0/*10*/, MINUS/*11*/, EQUAL/*12*/, BACKSPACE/*13*/,
        TAB/*14*/, Q/*15*/, W/*16*/, E/*17*/, R/*18*/, T/*19*/, Y/*20*/, U/*21*/, I/*22*/, O/*23*/, P/*24*/, LEFT_U_BRACE/*25*/, RIGHT_U_BRACE/*26*/, BACKSLASH/*27*/,
        CAP_LOCK/*28*/, A/*29*/, S/*30*/, D/*31*/, F/*32*/, G/*33*/, H/*34*/, J/*35*/, K/*36*/, L/*37*/, SEMI_COLON/*38*/, QUOTE/*39*/, ENTER/*40*/,
        LEFT_SHIFT/*41*/, Z/*42*/, X/*43*/, C/*44*/, V/*45*/, B/*46*/, N/*47*/, M/*48*/, COMMA/*49*/, PERIOD/*50*/, SLASH/*51*/, RIGHT_SHIFT/*52*/, UP_ARROW/*53*/, DELETE/*54*/,
        LEFT_CTRL/*55*/, LEFT_GUI/*56*/, LEFT_ALT/*57*/, SPACE/*58*/, RIGHT_ALT/*59*/, FN/*60*/, LEFT_ARROW/*61*/, DOWN_ARROW/*62*/, RIGHT_ARROW/*63*/,
    },

    {
        GRAVE_ACCENT, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, BACKSPACE,
        TAB, PROFILE1, PROFILE2, PROFILE3, PROFILE4, T, Y, U, I, O, P, LEFT_U_BRACE, RIGHT_U_BRACE, BACKSLASH,
        CAP_LOCK, A, S, D, F, G, H, J, K, L, SEMI_COLON, QUOTE, ENTER,
        LEFT_SHIFT, Z, X, C, V, B, N, M, COMMA, PERIOD, SLASH, RIGHT_SHIFT, UP_ARROW, DELETE,
        LEFT_CTRL, LEFT_GUI, LEFT_ALT, SPACE, RIGHT_ALT, FN, LEFT_ARROW, DOWN_ARROW, RIGHT_ARROW,
    }

};
// clang-format on

#endif
