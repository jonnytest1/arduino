

#pragma once

#include "page.h"
#include <INA226_WE.h>
#include <Wire.h>
#include <vector>

struct DataPoint
{
    float voltage; // Voltage in volts
    float ampere;
    unsigned long millis; // State of charge in percent
};

class Ina266PAge : public Page
{
  private:
    INA226_WE ina266;
    std::vector<DataPoint> *data;
    unsigned long start = 0;
    static inline const int left_offset = 50;
    static inline const int font_height = 12;
    static inline const int top_offset = Ina266PAge::font_height;

  public:
    Ina266PAge(TwoWire *wire)
    {
        ina266 = INA226_WE(wire, 0x40);
    }

    String name()
    {
        return "ina266";
    }

    void onSelected()
    {
        data = new std::vector<DataPoint>();
        data->reserve(1500);
        if (!ina266.init())
        {
            assert(false);
        }
        ina266.setAverage(AVERAGE_4);
        ina266.setMeasureMode(CONTINUOUS);
        ina266.setResistorRange(0.002, 10);
        start = millis();

        ESP_LOGI(L_TAG, "ina266 setup %d", start);

        screen->setTextDatum(TL_DATUM);
        screen->drawString(String("| 2min"), Ina266PAge::left_offset, 0);
        screen->drawString(String("| 1min"), screen->width() / 2 + (Ina266PAge::left_offset / 2), 0);
        screen->setTextDatum(TR_DATUM);
        screen->drawString(String("0 |"), screen->width(), 0);
        screen->setTextDatum(TL_DATUM);
    }
    void onUnSelected()
    {
        delete data;
    }

    bool frame()
    {
        return true;
    }

    void draw()
    {
        float mA = ina266.getCurrent_mA();
        float mV = ina266.getBusVoltage_V() * 1000;
        unsigned long now = millis();
        DataPoint pt = {mV, mA, now};
        data->push_back(pt);

        long cutoff = now - (1000 * 60 * 2);

        while (!data->empty() && ((long)data->at(0).millis) < cutoff)
        {
            ESP_LOGI(L_TAG, "ina266 erase %d now %d %d", data->at(0).millis, now, cutoff);
            data->erase(data->begin());
        }

        screen->fillRect(0, Ina266PAge::top_offset, screen->width(), screen->height() - Ina266PAge::top_offset,
                         TFT_BLACK);

        float maxV = -1;
        float maxmA = -1;
        float min = 0;

        for (auto point : *data)
        {
            maxmA = std::max(point.ampere, maxmA);
            maxV = std::max(point.voltage, maxV);
        }
        maxV += 1000;
        maxmA += 10;
        screen->setTextColor(TFT_GREEN, TFT_BLACK);
        screen->drawString(String((int)((maxV / 1000) + 1)) + "V", 0, Ina266PAge::top_offset);
        screen->setTextColor(TFT_ORANGE, TFT_BLACK);
        screen->drawString(String((int)((maxmA) + 10)) + "mA", 0, Ina266PAge::top_offset + Ina266PAge::font_height);
        screen->setTextColor(TFT_YELLOW, TFT_BLACK);
        screen->drawString(String("0V/A"), 0, screen->height() - Ina266PAge::top_offset);
        screen->drawString(String(mV) + "mV", 100, screen->height() - Ina266PAge::top_offset);
        screen->drawString(String(mA) + "mA", 200, screen->height() - Ina266PAge::top_offset);
        for (auto point : *data)
        {
            long yV = map(point.voltage, min, maxV, screen->height(), Ina266PAge::top_offset);
            long yA = map(point.ampere, min, maxmA, screen->height(), Ina266PAge::top_offset);

            long xTime = map(millis() - point.millis, 0, 1000 * 60 * 2, screen->width(), Ina266PAge::left_offset);

            screen->drawPixel(xTime, yV, TFT_GREEN);
            screen->drawPixel(xTime, yA, TFT_ORANGE);
        }
        // ESP_LOGI(L_TAG, "ina266 draw %d min %f maxV %fmaxmA %f cuurentV %f cmA %f", data->size(), min, maxV, maxmA,
        // mV,
        //       mA);
    }
};