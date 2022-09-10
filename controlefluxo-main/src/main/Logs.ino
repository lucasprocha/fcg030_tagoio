////////////////// privates declarations
#define LOG_COLOR_BLACK         "30"
#define LOG_COLOR_RED           "31"
#define LOG_COLOR_GREEN         "32"
#define LOG_COLOR_YELLOW        "33"
#define LOG_COLOR_BLUE          "34"
#define LOG_COLOR_PURPLE        "35"
#define LOG_COLOR_CYAN          "36"

////////////////// privates functions
void RED() {
  Serial.print("\033[0"LOG_COLOR_RED"m");
}
void YELLOW() {
  Serial.print("\033[0"LOG_COLOR_YELLOW"m");
}
void GREEN() {
  Serial.print("\033[0"LOG_COLOR_GREEN"m");
}
void BLUE() {
  Serial.print("\033[0"LOG_COLOR_BLUE"m");
}
void RESET() {
  Serial.print("\033[0m");
}

////////////////// publics functions
void LOG(const char *type, const char *tag, const char *fmt, ...) {
  char *fullMsg = NULL;
  char *log_str = NULL;
  va_list args;
  va_start(args, fmt);
  vasprintf(&log_str, fmt, args);
  va_end(args);

#if ENABLE_COLORS_LOG
  if (!strcmp(type, "ERROR")) RED();
  else if (!strcmp(type, "WARN")) YELLOW();
  else if (!strcmp(type, "INFO")) GREEN();
#endif
  if (log_str != NULL) {
    asprintf(&fullMsg, "[%s][%s] %s", type, tag, log_str);
    if (fullMsg != NULL) {
      Serial.write(fullMsg);
      free(fullMsg);
    }
    free(log_str);
  }

#if ENABLE_COLORS_LOG
  RESET();
#endif
}
