#include "logger.h"
#include "sgl.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static logLevel _level = LOG_LEVEL_COUNT;

typedef struct {
	int level;
	char* text;
} _log;

#define LOG_BUFFER_SIZE 1024
static _log logs[LOG_BUFFER_SIZE];
static int logCounter = -1;

void logClear() {
	for (int i = logCounter; i >= 0; i--) {
		free(logs[i].text);
	}
	logCounter = -1;
}

void logMsg(logLevel lvl, const char* text, ...) {
	 if (logCounter == LOG_BUFFER_SIZE) {
		free(logs[--logCounter].text);
	}

	// int args = 0;
	// int len = 0;
	//
	// while (text[len]) {
	// 	args += (text[len] == '%');
	// 	len++;
	// }

	int len = strlen(text);
	char* t = malloc(len*2);

    va_list valist;
	va_start(valist, text);

	vsprintf(t, text, valist);

	logs[++logCounter] = (_log){lvl, t};

	va_end(valist);
}

void logSetLogLevel(logLevel level) {
	_level = level;
}


static uint32_t logColor[LOG_LEVEL_COUNT] = {
	0xffffffff, // info
	0xffdd44ff, // warn
	0xff4433ff, // err
	0x22ff55ff, // success
};

void logDraw(sglBuffer* buf, sglFont* font, int x, int y) {
	// printf("test\n");
	for (int i = logCounter; i >= 0; i--) {
		_log l = logs[i];

		sglDrawText(buf, l.text, logColor[l.level], x, y + (logCounter - i) * font->fontHeight, font);
	// printf("test1\n");
	}
}


