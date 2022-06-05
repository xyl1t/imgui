#ifndef IMGUI_LOGGER_H
#define IMGUI_LOGGER_H

#include "sgl.h"

typedef enum logLevel {
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
	LOG_SUCCESS,
	LOG_LEVEL_COUNT
} logLevel;

// TODO: one log function and macros for info, warn, ...

void logMsg(logLevel lvl, const char* text, ...);
void logClear();

#define logInfo(text, ...) logMsg(LOG_INFO, text, ##__VA_ARGS__);
#define logWarn(text, ...) logMsg(LOG_WARN, text, ##__VA_ARGS__);
#define logError(text, ...) logMsg(LOG_ERROR, text, ##__VA_ARGS__);
#define logSuccess(text, ...) logMsg(LOG_SUCCESS, text, ##__VA_ARGS__);

void logDraw(sglBuffer* buf, sglFont* font, int x, int y);

#endif // IMGUI_LOGGER_H
