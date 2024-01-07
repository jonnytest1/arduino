
#ifdef ESP_IDF_VERSION_MAJOR // IDF 4+
#if CONFIG_IDF_TARGET_ESP32  // ESP32/PICO-D4
#include "esp32/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C30Serial.println(
#include "esp32c3/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/rtc.h"
#else
#error Target CONFIG_IDF_TARGET is not supported
#endif
#else // ESP32 Before IDF 4.0
#include "rom/rtc.h"
#endif

#include "wi_fi.h"
#include "log.h"
#include "str.h"

#include "esp_debug_helpers.h"

std::string verbose_get_reset_reason(int reason)
{
  switch (reason)
  {
  case 1:
    return "Vbat power on reset";
  case 3:
    return "Software reset digital core";
  case 4:
    return "Legacy watch dog reset digital core";
  case 5:
    return "Deep Sleep reset digital core";
  case 6:
    return "Reset by SLC module, reset digital core";
  case 7:
    return "Timer Group0 Watch dog reset digital core";
  case 8:
    return "Timer Group1 Watch dog reset digital core";
  case 9:
    return "RTC Watch dog Reset digital core";
  case 10:
    return "Instrusion tested to reset CPU";
  case 11:
    return "Time Group reset CPU";
  case 12:
    return "Software reset CPU";
  case 13:
    return "RTC Watch dog Reset CPU";
  case 14:
    return "for APP CPU, reseted by PRO CPU";
  case 15:
    return "Reset when the vdd voltage is not stable";
  case 16:
    return "RTC Watch dog reset digital core and rtc module";
  default:
    return "NO_MEAN";
  }
}
std::string getReasonName(int reason)
{
  switch (reason)
  {
  case 1:
    return "POWERON_RESET"; /**<1,  Vbat power on reset*/
  case 3:
    return "SW_RESET"; /**<3,  Software reset digital core*/
  case 4:
    return "OWDT_RESET"; /**<4,  Legacy watch dog reset digital core*/
  case 5:
    return "DEEPSLEEP_RESET"; /**<5,  Deep Sleep reset digital core*/
  case 6:
    return "SDIO_RESET"; /**<6,  Reset by SLC module, reset digital core*/
  case 7:
    return "TG0WDT_SYS_RESET"; /**<7,  Timer Group0 Watch dog reset digital core*/
  case 8:
    return "TG1WDT_SYS_RESET"; /**<8,  Timer Group1 Watch dog reset digital core*/
  case 9:
    return "RTCWDT_SYS_RESET"; /**<9,  RTC Watch dog Reset digital core*/
  case 10:
    return "INTRUSION_RESET"; /**<10, Instrusion tested to reset CPU*/
  case 11:
    return "TGWDT_CPU_RESET"; /**<11, Time Group reset CPU*/
  case 12:
    return "SW_CPU_RESET"; /**<12, Software reset CPU*/
  case 13:
    return "RTCWDT_CPU_RESET"; /**<13, RTC Watch dog Reset CPU*/
  case 14:
    return "EXT_CPU_RESET"; /**<14, for APP CPU, reseted by PRO CPU*/
  case 15:
    return "RTCWDT_BROWN_OUT_RESET"; /**<15, Reset when the vdd voltage is not stable*/
  case 16:
    return "RTCWDT_RTC_RESET"; /**<16, RTC Watch dog reset digital core and rtc module*/
  default:
    return "NO_MEAN";
  }
}

void logRestartReason()
{
  waitForWifi();

  RESET_REASON reason0 = rtc_get_reset_reason(0);
  RESET_REASON reason1 = rtc_get_reset_reason(1);

  logData("WARN", "esp32 restarted", {
                                         {"reason_nr0", itos(reason0)},                      //
                                         {"reason_enum0", getReasonName(reason0)},           //
                                         {"reason_txt0", verbose_get_reset_reason(reason0)}, //
                                                                                             //
                                         {"reason_nr1", itos(reason1)},                      //
                                         {"reason_enum1", getReasonName(reason1)},           //
                                         {"reason_txt1", verbose_get_reset_reason(reason1)}, //
                                     });

  Serial.println("esp32 restarted: 0");
  Serial.println(getReasonName(reason0).c_str());
  Serial.println(verbose_get_reset_reason(reason0).c_str());

  Serial.println("esp32 restarted: 1");
  Serial.println(getReasonName(reason1).c_str());
  Serial.println(verbose_get_reset_reason(reason1).c_str());
}