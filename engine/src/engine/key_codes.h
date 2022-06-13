#pragma once

namespace  engine
{
    /// \brief Keycodes from glf3.h
    class key_codes
    {
    public:
        /* Printable keys */
        static constexpr int KEY_SPACE              = 32;
        static constexpr int KEY_APOSTROPHE         = 39;  /* ' */
        static constexpr int KEY_COMMA              = 44;  /* , */
        static constexpr int KEY_MINUS              = 45;  /* - */
        static constexpr int KEY_PERIOD             = 46;  /* . */
        static constexpr int KEY_SLASH              = 47;  /* / */
        static constexpr int KEY_0                  = 48;
        static constexpr int KEY_1                  = 49;
        static constexpr int KEY_2                  = 50;
        static constexpr int KEY_3                  = 51;
        static constexpr int KEY_4                  = 52;
        static constexpr int KEY_5                  = 53;
        static constexpr int KEY_6                  = 54;
        static constexpr int KEY_7                  = 55;
        static constexpr int KEY_8                  = 56;
        static constexpr int KEY_9                  = 57;
        static constexpr int KEY_SEMICOLON          = 59;  /* ; */
        static constexpr int KEY_EQUAL              = 61;  /* = */
        static constexpr int KEY_A                  = 65;
        static constexpr int KEY_B                  = 66;
        static constexpr int KEY_C                  = 67;
        static constexpr int KEY_D                  = 68;
        static constexpr int KEY_E                  = 69;
        static constexpr int KEY_F                  = 70;
        static constexpr int KEY_G                  = 71;
        static constexpr int KEY_H                  = 72;
        static constexpr int KEY_I                  = 73;
        static constexpr int KEY_J                  = 74;
        static constexpr int KEY_K                  = 75;
        static constexpr int KEY_L                  = 76;
        static constexpr int KEY_M                  = 77;
        static constexpr int KEY_N                  = 78;
        static constexpr int KEY_O                  = 79;
        static constexpr int KEY_P                  = 80;
        static constexpr int KEY_Q                  = 81;
        static constexpr int KEY_R                  = 82;
        static constexpr int KEY_S                  = 83;
        static constexpr int KEY_T                  = 84;
        static constexpr int KEY_U                  = 85;
        static constexpr int KEY_V                  = 86;
        static constexpr int KEY_W                  = 87;
        static constexpr int KEY_X                  = 88;
        static constexpr int KEY_Y                  = 89;
        static constexpr int KEY_Z                  = 90;
        static constexpr int KEY_LEFT_BRACKET       = 91;  /* [ */
        static constexpr int KEY_BACKSLASH          = 92;  /* \ */
        static constexpr int KEY_RIGHT_BRACKET      = 93;  /* ] */
        static constexpr int KEY_GRAVE_ACCENT       = 96;  /* ` */
        static constexpr int KEY_WORLD_1            = 161; /* non-US #1 */
        static constexpr int KEY_WORLD_2            = 162; /* non-US #2 */

        /* Function keys */
        static constexpr int KEY_ESCAPE             = 256;
        static constexpr int KEY_ENTER              = 257;
        static constexpr int KEY_TAB                = 258;
        static constexpr int KEY_BACKSPACE          = 259;
        static constexpr int KEY_INSERT             = 260;
        static constexpr int KEY_DELETE             = 261;
        static constexpr int KEY_RIGHT              = 262;
        static constexpr int KEY_LEFT               = 263;
        static constexpr int KEY_DOWN               = 264;
        static constexpr int KEY_UP                 = 265;
        static constexpr int KEY_PAGE_UP            = 266;
        static constexpr int KEY_PAGE_DOWN          = 267;
        static constexpr int KEY_HOME               = 268;
        static constexpr int KEY_END                = 269;
        static constexpr int KEY_CAPS_LOCK          = 280;
        static constexpr int KEY_SCROLL_LOCK        = 281;
        static constexpr int KEY_NUM_LOCK           = 282;
        static constexpr int KEY_PRINT_SCREEN       = 283;
        static constexpr int KEY_PAUSE              = 284;
        static constexpr int KEY_F1                 = 290;
        static constexpr int KEY_F2                 = 291;
        static constexpr int KEY_F3                 = 292;
        static constexpr int KEY_F4                 = 293;
        static constexpr int KEY_F5                 = 294;
        static constexpr int KEY_F6                 = 295;
        static constexpr int KEY_F7                 = 296;
        static constexpr int KEY_F8                 = 297;
        static constexpr int KEY_F9                 = 298;
        static constexpr int KEY_F10                = 299;
        static constexpr int KEY_F11                = 300;
        static constexpr int KEY_F12                = 301;
        static constexpr int KEY_F13                = 302;
        static constexpr int KEY_F14                = 303;
        static constexpr int KEY_F15                = 304;
        static constexpr int KEY_F16                = 305;
        static constexpr int KEY_F17                = 306;
        static constexpr int KEY_F18                = 307;
        static constexpr int KEY_F19                = 308;
        static constexpr int KEY_F20                = 309;
        static constexpr int KEY_F21                = 310;
        static constexpr int KEY_F22                = 311;
        static constexpr int KEY_F23                = 312;
        static constexpr int KEY_F24                = 313;
        static constexpr int KEY_F25                = 314;
        static constexpr int KEY_KP_0               = 320;
        static constexpr int KEY_KP_1               = 321;
        static constexpr int KEY_KP_2               = 322;
        static constexpr int KEY_KP_3               = 323;
        static constexpr int KEY_KP_4               = 324;
        static constexpr int KEY_KP_5               = 325;
        static constexpr int KEY_KP_6               = 326;
        static constexpr int KEY_KP_7               = 327;
        static constexpr int KEY_KP_8               = 328;
        static constexpr int KEY_KP_9               = 329;
        static constexpr int KEY_KP_DECIMAL         = 330;
        static constexpr int KEY_KP_DIVIDE          = 331;
        static constexpr int KEY_KP_MULTIPLY        = 332;
        static constexpr int KEY_KP_SUBTRACT        = 333;
        static constexpr int KEY_KP_ADD             = 334;
        static constexpr int KEY_KP_ENTER           = 335;
        static constexpr int KEY_KP_EQUAL           = 336;
        static constexpr int KEY_LEFT_SHIFT         = 340;
        static constexpr int KEY_LEFT_CONTROL       = 341;
        static constexpr int KEY_LEFT_ALT           = 342;
        static constexpr int KEY_LEFT_SUPER         = 343;
        static constexpr int KEY_RIGHT_SHIFT        = 344;
        static constexpr int KEY_RIGHT_CONTROL      = 345;
        static constexpr int KEY_RIGHT_ALT          = 346;
        static constexpr int KEY_RIGHT_SUPER        = 347;
        static constexpr int KEY_MENU               = 348;

        static constexpr int KEY_LAST               = KEY_MENU;
    };
    
}
