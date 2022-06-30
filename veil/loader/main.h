#pragma once

// Project Options
//-------------- Project Keybinds
#define KEYBIND_TOGGLE_MENU VK_INSERT
#define KEYBIND_UNLOAD VK_DELETE


/*
******** PROJECT_DEBUG *******
	When PROJECT_DEBUG is defined:
1) a debugging console should appear.
2) debug prints will be enabled.
*/
#define PROJECT_DEBUG




// WORKINGS OF ABOVE. DO NOT TOUCH.
#ifdef PROJECT_DEBUG
#define DEBUG(...) printf(__VA_ARGS__);
#else
#define DEBUG(...) (void)0;
#endif

inline bool running = true;
