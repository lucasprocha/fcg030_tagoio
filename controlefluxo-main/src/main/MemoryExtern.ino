////////////////// privates definitions
#define TAG "MEXT"

bool init_ok = true;

////////////////// privates functions
void extract_variables(char *buf, char *cmd, char *timestamp, char *content){
  char *first, *second;
  first = strchr(buf, ';');
  second = strchr(first + 1, ';');
  strncpy(cmd, buf, first - buf);
  strncpy(timestamp, first + 1, second - first - 1);
  strcpy(content, second + 1);
}

////////////////// publics functions
uint16_t memory_extern_number_of_files() {
  unsigned char buf[256];
  char buf2[SPI_MEMORY_MAX_FILE_SIZE];
  unsigned long chipsize, cur = 0;

  if (!init_ok) return 0;

  DLOG(TAG, "Read Chip Identification:\r\n");
  DLOG(TAG, "%x %x %x\r\n", buf[0], buf[1], buf[2]);
  chipsize = SerialFlash.capacity(buf);
  DLOG(TAG, "Chip size: %.0f\r\n", chipsize);
  
  while(true) {
    SerialFlash.read(cur * SPI_MEMORY_MAX_FILE_SIZE, buf2, SPI_MEMORY_MAX_FILE_SIZE);
    if (buf2[0] == 0) {
      break;
    }
    cur++;
    if (cur * SPI_MEMORY_MAX_FILE_SIZE >= chipsize){
      break;
    }
  }
  return cur;
}

void memory_extern_write(String content){
  if(!content || !init_ok) return;
  SerialFlash.write(curFileOnMemory * SPI_MEMORY_MAX_FILE_SIZE, content.c_str(), content.length());
  curFileOnMemory++;
}

void memory_extern_send_to_server(){
  if(curFileOnMemory == 0) return;
  uint16_t cur = 0;
  char buf2[SPI_MEMORY_MAX_FILE_SIZE];
  char cmd[32], timestamp[16], content[32];
  while(cur < curFileOnMemory) {
    SerialFlash.read(cur * SPI_MEMORY_MAX_FILE_SIZE, buf2, SPI_MEMORY_MAX_FILE_SIZE);
    extract_variables(buf2, cmd, timestamp, content);
    send_inputs_to_broker(cmd, String(content).toDouble());
    cur++;
  }
  SerialFlash.eraseAll();
}

void memory_extern_init() {
  if (!SerialFlash.begin(SPI_MEMORY_CS_PIN)) {
    ELOG("SPI","Unable to access SPI Flash chip\r\n");
    init_ok = false;
    return;
  }
  curFileOnMemory = memory_extern_number_of_files();
  DLOG("DEBUG", "memory_extern_number_of_files(); ok\r\n");
}
