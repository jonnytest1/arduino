
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
#include "esp_core_dump.h"
#include "mbedtls/base64.h"
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

String *coredump = nullptr;
std::string *elfShaCrash = nullptr;

void checkCoreDump()
{

  esp_core_dump_summary_t *summary = (esp_core_dump_summary_t *)malloc(sizeof(esp_core_dump_summary_t));
  if (summary)
  {
    esp_err_t err = esp_core_dump_get_summary(summary);
    if (err == ESP_OK)
    {
      elfShaCrash = new std::string((char *)summary->app_elf_sha256);
      printf("App ELF file SHA256: %s\n", (char *)summary->app_elf_sha256);
      printf("Crashed task: %s\n", summary->exc_task);
      printf("Exception cause: %ld\n", summary->ex_info.exc_cause);
    }
    free(summary);
  }

  const esp_partition_t *core_dump_partition = esp_partition_find_first(
      ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_COREDUMP, "coredump");
  if (core_dump_partition)
  {

    uint8_t isWipedMarker = 0xAA;
    // Serial.println("got partition");
    size_t core_dump_size = core_dump_partition->size;
    uint8_t *core_dump_data = (uint8_t *)malloc(core_dump_size + 1);
    if (!core_dump_data)
    {
      Serial.println("Failed to allocate memory for core dump");
      return;
    }
    esp_err_t err = esp_partition_read(core_dump_partition, 0, core_dump_data, core_dump_size);

    if (err != ESP_OK)
    {
      Serial.println("failed to read dump");
      free(core_dump_data);
      return;
    }

    if (core_dump_data[0] == isWipedMarker)
    {
      Serial.println("empty core dump partition");
      return;
    }

    // Serial.println("reAD dump partition");

    int trimLength = 20;
    uint8_t *trimmed_core_dump_data = core_dump_data + trimLength;
    size_t trimmed_core_dump_size = core_dump_size - trimLength;

    size_t encoded_size = 0;
    mbedtls_base64_encode(NULL, 0, &encoded_size, trimmed_core_dump_data, trimmed_core_dump_size);
    uint8_t *encoded_data = (uint8_t *)malloc(encoded_size + 1); // used as return, so freed in calling function
    if (!encoded_data)
    {
      free(core_dump_data); // Free original buffer
      Serial.println("failed alloc");
      return;
    }

    // Encode core dump to Base64
    err = mbedtls_base64_encode(encoded_data, encoded_size, &encoded_size, trimmed_core_dump_data, trimmed_core_dump_size);
    if (err != 0)
    {
      free(core_dump_data); // Free original buffer
      free(encoded_data);   // Free allocated memory
      Serial.println("failed encode");
      return;
    }
    encoded_data[trimmed_core_dump_size] = '\0';

    Serial.print("b64 coredump size: ");
    Serial.println(trimmed_core_dump_size);
    free(core_dump_data);
    // Serial.println((char *)encoded_data);
    *coredump = String((char *)encoded_data, trimmed_core_dump_size + 1);

    Serial.println("free");
    free(encoded_data);
    auto err_erase = esp_partition_erase_range(core_dump_partition, 0, core_dump_partition->size);
    if (err_erase != ESP_OK)
    {
      Serial.println("failed to erased coredump");
      return;
    }
    else
    {
      esp_partition_write(core_dump_partition, 0, &isWipedMarker, sizeof(isWipedMarker));
    }
  }
  else
  {

    Serial.println("nodump part");
  }
}

void logRestartReason()
{
  waitForWifi();

  RESET_REASON reason0 = rtc_get_reset_reason(0);
  RESET_REASON reason1 = rtc_get_reset_reason(1);
  checkCoreDump();

  std::string type = "WARN";
  std::map<std::__cxx11::string, std::__cxx11::string> args = {
      {"reason_nr0", itos(reason0)},                      //
      {"reason_enum0", getReasonName(reason0)},           //
      {"reason_txt0", verbose_get_reset_reason(reason0)}, //
                                                          //
      {"reason_nr1", itos(reason1)},                      //
      {"reason_enum1", getReasonName(reason1)},           //
      {"reason_txt1", verbose_get_reset_reason(reason1)}, //
  };
  if (coredump != nullptr)
  {
    if (elfShaCrash != nullptr)
    {
      args.insert({"elfsha", *elfShaCrash});
    }
    logRaw("ERROR", "esp32 restarted after crash", args, coredump);
    delete elfShaCrash;
  }
  else
  {
    logData("WARN", "esp32 restarted", args);
  }

  Serial.println("esp32 restarted: 0");
  Serial.println(getReasonName(reason0).c_str());
  Serial.println(verbose_get_reset_reason(reason0).c_str());

  Serial.println("esp32 restarted: 1");
  Serial.println(getReasonName(reason1).c_str());
  Serial.println(verbose_get_reset_reason(reason1).c_str());

  const esp_partition_t *pt = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_COREDUMP, "coredump");

  if (pt != NULL)
  {
    uint32_t size = pt->size;
    printf("POINTER IS NOT NULL \n");

    char str_dst[256];
    size_t bytestoread = 256;
    size_t bytesread = 0;
    while (bytestoread > 0)
    {
      printf("STARTING PARTION READ \n");
      printf("Remaining partition size : %zu \n", size);

      esp_err_t er = esp_partition_read(pt, bytesread, str_dst, bytestoread);

      printf("PARTITION READ \n");
      printf("%s", str_dst);

      if (er != ESP_OK)
      {
        printf("READ IS NOT OK!\n");
        // ESP_LOGE(TAG, "Partition read error : %s", esp_err_to_name(er));
        return;
      }

      else if (er == ESP_OK)
      {
        printf("\n READ IS OK!\n");
        bytesread = bytesread + 256;
        size = size - 256;

        if (size <= 256)
        {
          bytestoread = size;
        }
      }
    }
  }
}