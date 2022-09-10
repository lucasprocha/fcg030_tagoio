////////////////// privates definitions
#define TAG "I2C"

#define gpioa 0x12  //GPIOA 0x12 endereço 
#define gpiob 0x13  //GPIOB 0x13 endereço
#define iodira 0x00  //IODIRA 0x00 endereço de configuração da saídas A
#define iodirb 0x01  //IODIRA 0x01 endereço de configuração da saídas A

#define addr1 2 //addr1 = A2 low , A1 low , A0 low =010 -  Endereço U10 -  expansão e leds
#define addr2 4 //addr2 = A2 low , A1 low , A0 high =100  -  endereço U9 

const uint8_t output_correction [] = {3, 0, 1, 2, 4, 5, 6, 7, 15, 14, 13, 12, 11, 10, 9, 8};

////////////////// privates functions
uint8_t reorder_inputs(uint8_t gpio) {
  uint8_t ret = gpio & 0x0f;
  if (TEST_BIT(gpio, 4)) SET_BIT(ret, 7);
  if (TEST_BIT(gpio, 5)) SET_BIT(ret, 6);
  if (TEST_BIT(gpio, 6)) SET_BIT(ret, 5);
  if (TEST_BIT(gpio, 7)) SET_BIT(ret, 4);
  return ret;
}

////////////////// publics functions
#if SIMULATE_DATA
uint8_t read_input_expand_i2c() {
  uint8_t ret = millis() % 256;
  return reorder_inputs(ret);
}
#else
uint8_t read_input_expand_i2c() {
  if (I2C_ERROR) {
    ELOG(TAG, "Error on the I2C\r\n");
    return 0;
  }
  Wire.beginTransmission(0x20 + addr2);
  Wire.write(gpiob);
  Wire.endTransmission();
  Wire.requestFrom(0x20 + addr2, 1);
  return reorder_inputs(Wire.read());
}
#endif

void send_to_mpc_i2c() {
  if (I2C_ERROR) {
    ELOG(TAG, "Error on the I2C\r\n");
    return;
  }

  String status = readFile(OUTPUT_PIN_STATUS);
  DynamicJsonDocument doc(512);
  deserializeJson(doc, status);

  uint16_t res = 0;
  for (uint8_t i = 0; i < 16; i++) {
    if (doc[i]) {
      SET_BIT(res, output_correction[i]);
    }
  }
  DLOG(TAG, "Out PINS: %i\r\n", res);
#if !SIMULATE_DATA
  Wire.beginTransmission(0x20 + addr1);
  Wire.write(gpiob); // address PORT B
  Wire.write((res >> 8) & 0x00ff);    // PORT B
  Wire.endTransmission();

  Wire.beginTransmission(0x20 + addr2);
  Wire.write(gpioa); // address PORT A
  Wire.write(res & 0x00ff);    // PORT A
  Wire.endTransmission();
#endif
}

int init_io_expand_i2c() {
#if !SIMULATE_DATA
  //setup U10
  Wire.begin(); // wake up I2C bus
  Wire.beginTransmission(0x20 + addr1);
  Wire.write(iodirb); // IODIRB register
  Wire.write(0x00); // set entire PORT B as output
  Wire.endTransmission();

  Wire.begin(); // wake up I2C bus
  Wire.beginTransmission(0x20 + addr1);
  Wire.write(iodira); // IODIRBA register
  Wire.write(0xff); // set entire PORT A as input
  Wire.endTransmission();

  //Setup U9
  Wire.begin(); // wake up I2C bus
  Wire.beginTransmission(0x20 + addr2);
  Wire.write(iodirb); // IODIRB register
  Wire.write(0xff); // set entire PORT B as input
  Wire.endTransmission();

  Wire.begin(); // wake up I2C bus
  Wire.beginTransmission(0x20 + addr2);
  Wire.write(iodira); // IODIRA register
  Wire.write(0x00); // set entire PORT A as output
  Wire.endTransmission();

  /////
#endif
  if (existFile(OUTPUT_PIN_STATUS)) {
    send_to_mpc_i2c();
  } else {
    WLOG(TAG, "Not found the doc of status of output pins, creating it...\r\n");
    const size_t CAPACITY = JSON_ARRAY_SIZE(16);

    StaticJsonDocument<CAPACITY> doc;

    JsonArray array = doc.to<JsonArray>();

    for (uint8_t i = 0; i < 16; i++) {
      array.add(0);
    }

    String res;
    serializeJson(doc, res);

    writeFile(OUTPUT_PIN_STATUS, res.c_str());
#if !SIMULATE_DATA
    Wire.beginTransmission(0x20 + addr1);
    Wire.write(gpiob); // address PORT B
    Wire.write(0);    // PORT B
    Wire.endTransmission();

    Wire.beginTransmission(0x20 + addr2);
    Wire.write(gpioa); // address PORT A
    Wire.write(0);    // PORT A
    Wire.endTransmission();
#endif
  }

  return 0;
}
