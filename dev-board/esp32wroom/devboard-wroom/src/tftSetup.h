#include <Arduino.h>

#pragma once

// Identify that a user setup file has been loaded
#define USER_SETUP_LOADED 1
#define USER_SETUP_ID 27

// === Display Driver ===
#define ST7796_DRIVER

// === Pin Configuration ===
#define TFT_MISO -1
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS 4
#define TFT_DC 5
#define TFT_RST -1 // Set to -1 if not connected

// === Font Loading ===
#define LOAD_GLCD  // Font 1. Original Adafruit 8 pixel font
#define LOAD_FONT2 // Small 16 pixel high font
#define LOAD_FONT4 // Medium 26 pixel high font
#define LOAD_FONT6 // Large 48 pixel font
#define LOAD_FONT7 // 7-segment 48 pixel font
#define LOAD_FONT8 // Large 75 pixel font
#define LOAD_GFXFF // FreeFonts from Adafruit_GFX

// === SPI Settings ===
#define SPI_FREQUENCY 40000000 // You can adjust this if needed

#define USE_FSPI_PORT

constexpr int tft_cs_pin = TFT_CS;
constexpr int tft_dc_pin = TFT_DC;
constexpr int tft_rst_pin = TFT_RST;
constexpr int tft_mosi_pin = TFT_MOSI;
constexpr int tft_sclk_pin = TFT_SCLK;
constexpr int tft_miso_pin = TFT_MISO;