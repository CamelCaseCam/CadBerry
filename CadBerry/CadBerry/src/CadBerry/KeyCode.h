#pragma once

#define CDB_KEY_SPACE              32
#define CDB_KEY_APOSTROPHE         39  /* ' */
#define CDB_KEY_COMMA              44  /* , */
#define CDB_KEY_MINUS              45  /* - */
#define CDB_KEY_PERIOD             46  /* . */
#define CDB_KEY_SLASH              47  /* / */
#define CDB_KEY_0                  48
#define CDB_KEY_1                  49
#define CDB_KEY_2                  50
#define CDB_KEY_3                  51
#define CDB_KEY_4                  52
#define CDB_KEY_5                  53
#define CDB_KEY_6                  54
#define CDB_KEY_7                  55
#define CDB_KEY_8                  56
#define CDB_KEY_9                  57
#define CDB_KEY_SEMICOLON          59  /* ; */
#define CDB_KEY_EQUAL              61  /* = */
#define CDB_KEY_A                  65
#define CDB_KEY_B                  66
#define CDB_KEY_C                  67
#define CDB_KEY_D                  68
#define CDB_KEY_E                  69
#define CDB_KEY_F                  70
#define CDB_KEY_G                  71
#define CDB_KEY_H                  72
#define CDB_KEY_I                  73
#define CDB_KEY_J                  74
#define CDB_KEY_K                  75
#define CDB_KEY_L                  76
#define CDB_KEY_M                  77
#define CDB_KEY_N                  78
#define CDB_KEY_O                  79
#define CDB_KEY_P                  80
#define CDB_KEY_Q                  81
#define CDB_KEY_R                  82
#define CDB_KEY_S                  83
#define CDB_KEY_T                  84
#define CDB_KEY_U                  85
#define CDB_KEY_V                  86
#define CDB_KEY_W                  87
#define CDB_KEY_X                  88
#define CDB_KEY_Y                  89
#define CDB_KEY_Z                  90
#define CDB_KEY_LEFT_BRACKET       91  /* [ */
#define CDB_KEY_BACKSLASH          92  /* \ */
#define CDB_KEY_RIGHT_BRACKET      93  /* ] */
#define CDB_KEY_GRAVE_ACCENT       96  /* ` */
#define CDB_KEY_WORLD_1            161 /* non-US #1 */
#define CDB_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define CDB_KEY_ESCAPE             256
#define CDB_KEY_ENTER              257
#define CDB_KEY_TAB                258
#define CDB_KEY_BACKSPACE          259
#define CDB_KEY_INSERT             260
#define CDB_KEY_DELETE             261
#define CDB_KEY_RIGHT              262
#define CDB_KEY_LEFT               263
#define CDB_KEY_DOWN               264
#define CDB_KEY_UP                 265
#define CDB_KEY_PAGE_UP            266
#define CDB_KEY_PAGE_DOWN          267
#define CDB_KEY_HOME               268
#define CDB_KEY_END                269
#define CDB_KEY_CAPS_LOCK          280
#define CDB_KEY_SCROLL_LOCK        281
#define CDB_KEY_NUM_LOCK           282
#define CDB_KEY_PRINT_SCREEN       283
#define CDB_KEY_PAUSE              284
#define CDB_KEY_F1                 290
#define CDB_KEY_F2                 291
#define CDB_KEY_F3                 292
#define CDB_KEY_F4                 293
#define CDB_KEY_F5                 294
#define CDB_KEY_F6                 295
#define CDB_KEY_F7                 296
#define CDB_KEY_F8                 297
#define CDB_KEY_F9                 298
#define CDB_KEY_F10                299
#define CDB_KEY_F11                300
#define CDB_KEY_F12                301
#define CDB_KEY_F13                302
#define CDB_KEY_F14                303
#define CDB_KEY_F15                304
#define CDB_KEY_F16                305
#define CDB_KEY_F17                306
#define CDB_KEY_F18                307
#define CDB_KEY_F19                308
#define CDB_KEY_F20                309
#define CDB_KEY_F21                310
#define CDB_KEY_F22                311
#define CDB_KEY_F23                312
#define CDB_KEY_F24                313
#define CDB_KEY_F25                314
#define CDB_KEY_KP_0               320
#define CDB_KEY_KP_1               321
#define CDB_KEY_KP_2               322
#define CDB_KEY_KP_3               323
#define CDB_KEY_KP_4               324
#define CDB_KEY_KP_5               325
#define CDB_KEY_KP_6               326
#define CDB_KEY_KP_7               327
#define CDB_KEY_KP_8               328
#define CDB_KEY_KP_9               329
#define CDB_KEY_KP_DECIMAL         330
#define CDB_KEY_KP_DIVIDE          331
#define CDB_KEY_KP_MULTIPLY        332
#define CDB_KEY_KP_SUBTRACT        333
#define CDB_KEY_KP_ADD             334
#define CDB_KEY_KP_ENTER           335
#define CDB_KEY_KP_EQUAL           336
#define CDB_KEY_LEFT_SHIFT         340
#define CDB_KEY_LEFT_CONTROL       341
#define CDB_KEY_LEFT_ALT           342
#define CDB_KEY_LEFT_SUPER         343
#define CDB_KEY_RIGHT_SHIFT        344
#define CDB_KEY_RIGHT_CONTROL      345
#define CDB_KEY_RIGHT_ALT          346
#define CDB_KEY_RIGHT_SUPER        347
#define CDB_KEY_MENU               348

/*! @brief If this bit is set one or more Shift keys were held down.
 *
 *  If this bit is set one or more Shift keys were held down.
 */
#define CDB_MOD_SHIFT           0x0001
 /*! @brief If this bit is set one or more Control keys were held down.
  *
  *  If this bit is set one or more Control keys were held down.
  */
#define CDB_MOD_CONTROL         0x0002
  /*! @brief If this bit is set one or more Alt keys were held down.
   *
   *  If this bit is set one or more Alt keys were held down.
   */
#define CDB_MOD_ALT             0x0004
   /*! @brief If this bit is set one or more Super keys were held down.
	*
	*  If this bit is set one or more Super keys were held down.
	*/
#define CDB_MOD_SUPER           0x0008
	/*! @brief If this bit is set the Caps Lock key is enabled.
	 *
	 *  If this bit is set the Caps Lock key is enabled and the @ref
	 *  CDB_LOCK_KEY_MODS input mode is set.
	 */
#define CDB_MOD_CAPS_LOCK       0x0010
	 /*! @brief If this bit is set the Num Lock key is enabled.
	  *
	  *  If this bit is set the Num Lock key is enabled and the @ref
	  *  CDB_LOCK_KEY_MODS input mode is set.
	  */
#define CDB_MOD_NUM_LOCK        0x0020