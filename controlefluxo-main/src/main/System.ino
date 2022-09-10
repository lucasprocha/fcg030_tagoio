////////////////// privates definitions
#define TAG "SYS"

////////////////// privates functions

////////////////// publics functions
void print_reset_reason(RESET_REASON reason)
{
  switch ( reason)
  {
    case 1 : DLOG (TAG,"POWERON_RESET\r\n"); break;         /**<1,  Vbat power on reset*/
    case 3 : DLOG (TAG,"SW_RESET\r\n"); break;              /**<3,  Software reset digital core*/
    case 4 : DLOG (TAG,"OWDT_RESET\r\n"); break;            /**<4,  Legacy watch dog reset digital core*/
    case 5 : DLOG (TAG,"DEEPSLEEP_RESET\r\n"); break;       /**<5,  Deep Sleep reset digital core*/
    case 6 : DLOG (TAG,"SDIO_RESET\r\n"); break;            /**<6,  Reset by SLC module, reset digital core*/
    case 7 : DLOG (TAG,"TG0WDT_SYS_RESET\r\n"); break;      /**<7,  Timer Group0 Watch dog reset digital core*/
    case 8 : DLOG (TAG,"TG1WDT_SYS_RESET\r\n"); break;      /**<8,  Timer Group1 Watch dog reset digital core*/
    case 9 : DLOG (TAG,"RTCWDT_SYS_RESET\r\n"); break;      /**<9,  RTC Watch dog Reset digital core*/
    case 10 : DLOG (TAG,"INTRUSION_RESET\r\n"); break;      /**<10, Instrusion tested to reset CPU*/
    case 11 : DLOG (TAG,"TGWDT_CPU_RESET\r\n"); break;      /**<11, Time Group reset CPU*/
    case 12 : DLOG (TAG,"SW_CPU_RESET\r\n"); break;         /**<12, Software reset CPU*/
    case 13 : DLOG (TAG,"RTCWDT_CPU_RESET\r\n"); break;     /**<13, RTC Watch dog Reset CPU*/
    case 14 : DLOG (TAG,"EXT_CPU_RESET\r\n"); break;        /**<14, for APP CPU, reseted by PRO CPU*/
    case 15 : DLOG (TAG,"RTCWDT_BROWN_OUT_RESET\r\n"); break; /**<15, Reset when the vdd voltage is not stable*/
    case 16 : DLOG (TAG,"RTCWDT_RTC_RESET\r\n"); break;     /**<16, RTC Watch dog reset digital core and rtc module*/
    default : DLOG (TAG,"NO_MEAN\r\n");
  }
}

void verbose_print_reset_reason(RESET_REASON reason)
{
  switch ( reason)
  {
    case 1  : DLOG (TAG,"Vbat power on reset\r\n"); break;
    case 3  : DLOG (TAG,"Software reset digital core\r\n"); break;
    case 4  : DLOG (TAG,"Legacy watch dog reset digital core\r\n"); break;
    case 5  : DLOG (TAG,"Deep Sleep reset digital core\r\n"); break;
    case 6  : DLOG (TAG,"Reset by SLC module, reset digital core\r\n"); break;
    case 7  : DLOG (TAG,"Timer Group0 Watch dog reset digital core\r\n"); break;
    case 8  : DLOG (TAG,"Timer Group1 Watch dog reset digital core\r\n"); break;
    case 9  : DLOG (TAG,"RTC Watch dog Reset digital core\r\n"); break;
    case 10 : DLOG (TAG,"Instrusion tested to reset CPU\r\n"); break;
    case 11 : DLOG (TAG,"Time Group reset CPU\r\n"); break;
    case 12 : DLOG (TAG,"Software reset CPU\r\n"); break;
    case 13 : DLOG (TAG,"RTC Watch dog Reset CPU\r\n"); break;
    case 14 : DLOG (TAG,"for APP CPU, reseted by PRO CPU\r\n"); break;
    case 15 : DLOG (TAG,"Reset when the vdd voltage is not stable\r\n"); break;
    case 16 : DLOG (TAG,"RTC Watch dog reset digital core and rtc module\r\n"); break;
    default : DLOG (TAG,"NO_MEAN\r\n");
  }
}
