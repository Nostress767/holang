#include "types.h"

#ifdef DEBUG

#define RESET_TEXT                         "\033[0m"

#define BLACK_REGULAR_TEXT                 "\033[0;30m"
#define RED_REGULAR_TEXT                   "\033[0;31m"
#define GREEN_REGULAR_TEXT                 "\033[0;32m"
#define YELLOW_REGULAR_TEXT                "\033[0;33m"
#define BLUE_REGULAR_TEXT                  "\033[0;34m"
#define PURPLE_REGULAR_TEXT                "\033[0;35m"
#define CYAN_REGULAR_TEXT                  "\033[0;36m"
#define WHITE_REGULAR_TEXT                 "\033[0;37m"

#define BLACK_BOLD_TEXT                    "\033[1;30m"
#define RED_BOLD_TEXT                      "\033[1;31m"
#define GREEN_BOLD_TEXT                    "\033[1;32m"
#define YELLOW_BOLD_TEXT                   "\033[1;33m"
#define BLUE_BOLD_TEXT                     "\033[1;34m"
#define PURPLE_BOLD_TEXT                   "\033[1;35m"
#define CYAN_BOLD_TEXT                     "\033[1;36m"
#define WHITE_BOLD_TEXT                    "\033[1;37m"

#define BLACK_FAINT_TEXT                   "\033[2;30m"
#define RED_FAINT_TEXT                     "\033[2;31m"
#define GREEN_FAINT_TEXT                   "\033[2;32m"
#define YELLOW_FAINT_TEXT                  "\033[2;33m"
#define BLUE_FAINT_TEXT                    "\033[2;34m"
#define PURPLE_FAINT_TEXT                  "\033[2;35m"
#define CYAN_FAINT_TEXT                    "\033[2;36m"
#define WHITE_FAINT_TEXT                   "\033[2;37m"

#define BLACK_ITALIC_TEXT                  "\033[3;30m"
#define RED_ITALIC_TEXT                    "\033[3;31m"
#define GREEN_ITALIC_TEXT                  "\033[3;32m"
#define YELLOW_ITALIC_TEXT                 "\033[3;33m"
#define BLUE_ITALIC_TEXT                   "\033[3;34m"
#define PURPLE_ITALIC_TEXT                 "\033[3;35m"
#define CYAN_ITALIC_TEXT                   "\033[3;36m"
#define WHITE_ITALIC_TEXT                  "\033[3;37m"

#define BLACK_UNDERLINE_TEXT               "\033[4;30m"
#define RED_UNDERLINE_TEXT                 "\033[4;31m"
#define GREEN_UNDERLINE_TEXT               "\033[4;32m"
#define YELLOW_UNDERLINE_TEXT              "\033[4;33m"
#define BLUE_UNDERLINE_TEXT                "\033[4;34m"
#define PURPLE_UNDERLINE_TEXT              "\033[4;35m"
#define CYAN_UNDERLINE_TEXT                "\033[4;36m"
#define WHITE_UNDERLINE_TEXT               "\033[4;37m"

#define BLACK_BACKGROUND                   "\033[40m"
#define RED_BACKGROUND                     "\033[41m"
#define GREEN_BACKGROUND                   "\033[42m"
#define YELLOW_BACKGROUND                  "\033[43m"
#define BLUE_BACKGROUND                    "\033[44m"
#define PURPLE_BACKGROUND                  "\033[45m"
#define CYAN_BACKGROUND                    "\033[46m"
#define WHITE_BACKGROUND                   "\033[47m"

/* #define PURPLE_REGULAR_BACKGROUND "\033[1;45m" */

#define BLACK_REGULAR_HIGH_INTENSITY_TEXt  "\033[0;90m"
#define RED_REGULAR_HIGH_INTENSITY_TEXT    "\033[0;91m"
#define GREEN_REGULAR_HIGH_INTENSITY_TEXt  "\033[0;92m"
#define YELLOW_REGULAR_HIGH_INTENSITY_TExt "\033[0;93m"
#define BLUE_REGULAR_HIGH_INTENSITY_TEXT   "\033[0;94m"
#define PURPLE_REGULAR_HIGH_INTENSITY_TExt "\033[0;95m"
#define CYAN_REGULAR_HIGH_INTENSITY_TEXT   "\033[0;96m"
#define WHITE_REGULAR_HIGH_INTENSITY_TEXt  "\033[0;97m"

#define BLACK_BOLD_HIGH_INTENSITY_TEXT     "\033[1;90m"
#define RED_BOLD_HIGH_INTENSITY_TEXT       "\033[1;91m"
#define GREEN_BOLD_HIGH_INTENSITY_TEXT     "\033[1;92m"
#define YELLOW_BOLD_HIGH_INTENSITY_TEXT    "\033[1;93m"
#define BLUE_BOLD_HIGH_INTENSITY_TEXT      "\033[1;94m"
#define PURPLE_BOLD_HIGH_INTENSITY_TEXT    "\033[1;95m"
#define CYAN_BOLD_HIGH_INTENSITY_TEXT      "\033[1;96m"
#define WHITE_BOLD_HIGH_INTENSITY_TEXT     "\033[1;97m"

#define BLACK_HIGH_INTENSITY_BACKGROUND    "\033[0;100m"
#define RED_HIGH_INTENSITY_BACKGROUND      "\033[0;101m"
#define GREEN_HIGH_INTENSITY_BACKGROUND    "\033[0;102m"
#define YELLOW_HIGH_INTENSITY_BACKGROUND   "\033[0;103m"
#define BLUE_HIGH_INTENSITY_BACKGROUND     "\033[0;104m"
#define PURPLE_HIGH_INTENSITY_BACKGROUND   "\033[0;105m"
#define CYAN_HIGH_INTENSITY_BACKGROUND     "\033[0;106m"
#define WHITE_HIGH_INTENSITY_BACKGROUND    "\033[0;107m"

#include <stdio.h>
#include <inttypes.h>

/* NOTE: this will be local to each TU */
static u64 ___localLogCounter;

#define ___LOG(outputStream, typeBackgroundColor, type, place, messageBackgroundColor, message) \
  do{                                                                                           \
    fprintf(outputStream, "%s[%s%s%" PRIu64 "%s %s%s%s%s %s'%s']%s%s%s|%s|%s\n",                \
		 BLACK_BOLD_HIGH_INTENSITY_TEXT,                                                \
                 RESET_TEXT,                                                                    \
                 WHITE_REGULAR_TEXT,                                                            \
                 ___localLogCounter++,                                                         \
                 RESET_TEXT,                                                                    \
                 typeBackgroundColor,                                                           \
                 BLACK_FAINT_TEXT,                                                              \
                 type,                                                                          \
                 RESET_TEXT,                                                                    \
                 BLACK_BOLD_HIGH_INTENSITY_TEXT,                                                \
                 place,                                                                         \
                 RESET_TEXT,                                                                    \
                 messageBackgroundColor,                                                        \
                 BLACK_FAINT_TEXT,                                                              \
                 message,                                                                       \
                 RESET_TEXT);                                                                   \
  } while(0)
#define LOG_ERROR(message) ___LOG(stderr, RED_HIGH_INTENSITY_BACKGROUND, "|ERROR|", __func__, RED_HIGH_INTENSITY_BACKGROUND, message)
#define LOG_INFO(message) ___LOG(stdout, GREEN_BACKGROUND, "|INFO|", __func__, GREEN_BACKGROUND, message)
#define LOG_TRACE(message) ___LOG(stdout, BLUE_HIGH_INTENSITY_BACKGROUND, "|TRACE|", __func__, BLUE_HIGH_INTENSITY_BACKGROUND, message)
#define LOG_WARN(message) ___LOG(stdout, YELLOW_BACKGROUND, "|WARN|", __func__, YELLOW_BACKGROUND, message)

#define ___LOGF(outputStream, typeBackgroundColor, type, place, messageBackgroundColor, formatting, ...) \
  do{                                                                                                    \
    fprintf(outputStream, "%s[%s%s%" PRIu64 "%s %s%s%s%s %s'%s']%s%s%s|" formatting "|%s\n",             \
		 BLACK_BOLD_HIGH_INTENSITY_TEXT,                                                         \
                 RESET_TEXT,                                                                             \
                 WHITE_REGULAR_TEXT,                                                                     \
                 ___localLogCounter++,                                                                  \
                 RESET_TEXT,                                                                             \
                 typeBackgroundColor,                                                                    \
                 BLACK_FAINT_TEXT,                                                                       \
                 type,                                                                                   \
                 RESET_TEXT,                                                                             \
                 BLACK_BOLD_HIGH_INTENSITY_TEXT,                                                         \
                 place,                                                                                  \
                 RESET_TEXT,                                                                             \
                 messageBackgroundColor,                                                                 \
                 BLACK_FAINT_TEXT,                                                                       \
                 __VA_ARGS__,                                                                            \
                 RESET_TEXT);                                                                            \
  } while(0)

#define LOGF_ERROR(formatting, ...) ___LOGF(stderr, RED_HIGH_INTENSITY_BACKGROUND, "|ERROR|", __func__, RED_HIGH_INTENSITY_BACKGROUND, formatting, __VA_ARGS__)
#define LOGF_INFO(formatting, ...) ___LOGF(stdout, GREEN_BACKGROUND, "|INFO|", __func__, GREEN_BACKGROUND, formatting, __VA_ARGS__)
#define LOGF_TRACE(formatting, ...) ___LOGF(stdout, BLUE_HIGH_INTENSITY_BACKGROUND, "|TRACE|", __func__, BLUE_HIGH_INTENSITY_BACKGROUND, formatting, __VA_ARGS__)
#define LOGF_WARN(formatting, ...) ___LOGF(stdout, YELLOW_BACKGROUND, "|WARN|", __func__, YELLOW_BACKGROUND, formatting, __VA_ARGS__)

#else /* !defined(DEBUG) */

#define LOG_ERROR(nothing)
#define LOG_INFO(nothing)
#define LOG_TRACE(nothing)
#define LOG_WARN(nothing)

#define LOGF_ERROR(nothing, ...)
#define LOGF_INFO(nothing, ...)
#define LOGF_TRACE(nothing, ...)
#define LOGF_WARN(nothing, ...)

#endif /* DEBUG */

