////////////////// privates definitions
#define TAG "FS"

////////////////// privates functions

////////////////// publics functions
void listDir(const char * dirname, uint8_t levels) {
  FLOG(TAG, "Listing directory: %s\r\n", dirname);

  File root = SPIFFS.open(dirname);
  if (!root) {
    ELOG(TAG, "- failed to open directory\r\n");
    return;
  }
  if (!root.isDirectory()) {
    WLOG(TAG, " - not a directory\r\n");
    return;                                        
  }             

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      DLOG(TAG, "  DIR: %s\r\n", file.name());
      if (levels) {
        listDir(file.name(), levels - 1);
      }
    } else {
      DLOG(TAG, "  FILE: %s\tSIZE: %i\r\n", file.name(), file.size());
    }
    file = root.openNextFile();
  }
}

String readFile( const char * path) {
  FLOG(TAG, "Reading file: %s\r\n", path);
  String finalString = "";
  File file = SPIFFS.open(path, "r");
  if (!file || file.isDirectory()) {
    ELOG(TAG, "- failed to open file for reading\r\n");
    return "";
  }

  String str = file.readString();

  file.close();
  return str;
}

int readFile( const char * path, char *content) {
  FLOG(TAG, "Reading file: %s\r\n", path);
  String finalString = "";
  File file = SPIFFS.open(path, "r");
  if (!file || file.isDirectory()) {
    ELOG(TAG, "- failed to open file for reading\r\n");
    return -1;
  }

  String str = file.readString();
  strcpy(content, str.c_str());

  file.close();
  return 0;
}

void writeFile(const char * path, const char * message) {
  if (path == NULL || message == NULL) {
    ELOG(TAG, "Parametros invalidos para escrever em arquivos\r\n")  ;
    return;
  }
  FLOG(TAG, "Writing file: %s\r\n", path);

  File file = SPIFFS.open(path, "w");
  if (!file) {
    ELOG(TAG, "Failed to open file for writing\r\n");
    return;
  }
  if (file.print(message)) {
    DLOG(TAG, "ok\r\n");
  } else {
    ELOG(TAG, "failed\r\n");
  }
  file.close();
}

void appendFile(const char * path, const char * message, bool showLog = true){
    if (path == NULL || message == NULL) {
    ELOG(TAG, "Parametros invalidos para anexar em arquivos\r\n")  ;
    return;
  }
  if (showLog)
  FLOG(TAG, "Appending file: %s => ", path);

    File file = SPIFFS.open(path, FILE_APPEND);
    if(!file){
        ELOG(TAG, "Failed to open file for appending\r\n");
        return;
    }
    if(file.print(message)){
      if (showLog)
        DLOG(TAG, "ok\r\n");
    } else {
        ELOG(TAG, "failed\r\n");
    }
    file.close();
}

int sizeFile( const char * path) {
  FLOG(TAG, "Verify file size: %s\r\n", path);
  int fileSize = 0;
  File file = SPIFFS.open(path, "r");
  if (!file || file.isDirectory()) {
    ELOG(TAG, "- failed to open file for reading\r\n");
    return -1;
  }

  fileSize = file.size();

  file.close();
  return fileSize;
}

void deleteFile(const char * path) {
  FLOG(TAG, "Deleting file: %\r\n", path);
  if (SPIFFS.remove(path)) {
    DLOG(TAG, "ok\r\n");
  } else {
    ELOG(TAG, "failed\r\n");
  }
}

boolean existFile(const char *path) {
  return SPIFFS.exists(path);
}
