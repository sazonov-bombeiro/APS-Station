#include <Arduino.h>
#include <esp_display_panel.hpp>

#include <lvgl.h>
#include "lvgl_v8_port.h"
#include <ui.h>

#include <Bonezegei_SoftSerial.h>
Bonezegei_SoftSerial SoftSerial(15, 16);

using namespace esp_panel::drivers;
using namespace esp_panel::board;

#include <Chrono.h>
Chrono chronos[4];

bool passedFirstPost = false;

String DadosInput = "";
String TempValor, Md, MdAlr;
char CharInput[16], charMdAlr[4], charMd[2];
int Modulo = 0;

String Temps[4];
int opacities[4][8];

void setup() {
  String title = "TouchFire_Control_Painel";

  Serial.begin(9600);
  Serial1.begin(9600);

  for (auto &chrono : chronos) chrono.restart();

  Serial.println("Initializing board");
  Board *board = new Board();
  board->init();

#if LVGL_PORT_AVOID_TEARING_MODE
  auto lcd = board->getLCD();
  lcd->configFrameBufferNumber(LVGL_PORT_DISP_BUFFER_NUM);
#if ESP_PANEL_DRIVERS_BUS_ENABLE_RGB && CONFIG_IDF_TARGET_ESP32S3
  auto lcd_bus = lcd->getBus();

  if (lcd_bus->getBasicAttributes().type == ESP_PANEL_BUS_TYPE_RGB) {
    static_cast<BusRGB *>(lcd_bus)->configRGB_BounceBufferSize(lcd->getFrameWidth() * 10);
  }
#endif
#endif
  assert(board->begin());

  lvgl_port_init(board->getLCD(), board->getTouch());

  Serial.println("Creating UI");
  lvgl_port_lock(-1);

  ui_init();
  lvgl_port_unlock();
}

void loop() {
  if (Serial1.available()) {
    DadosInput = Serial1.readString();
    delay(50);
  }

  if (DadosInput.length() > 0) {
    if (DadosInput.length() >= 10) {
      NumMod();
      Temp();
      ModAlrt();
      processModuleData();
    }
    DadosInput = "";
  }
  checkModuleTimeouts();
  delay(50);
}

//***********************************************************  VOID   ***********************************************************************

void Temp() {  //----------------------------------------- VOID TEMPERATURA -------------------------------------------------------
  if (DadosInput.length() >= 9) {
    TempValor = DadosInput.substring(DadosInput.length() - 5, DadosInput.length() - 2);
   // Serial.print("Temperature: ");
   // Serial.println(TempValor);
  }
}

void ModAlrt() {  // ----------------------------------------------------  Mod Alrt  ----------------------------------------------
  if (DadosInput.length() >= 11) {
    MdAlr = DadosInput.substring(DadosInput.length() - 12, DadosInput.length() - 6);
   // Serial.print("Module Alert: ");
   // Serial.println(MdAlr);
  }
}

void NumMod() {  // -----------------------------------------------------  Num Modulo ------------------------------------------------
  if (DadosInput.length() >= 11) {
    Md = DadosInput.substring(DadosInput.length() - 12, DadosInput.length() - 8);
   // Serial.print("Module ID: ");
   // Serial.println(Md);

    // Convert module ID to module number
    if (Md == "M991") {
      Modulo = 1;
      //Serial.println("Module 1 detected");
    } else if (Md == "M992") {
      Modulo = 2;
      //Serial.println("Module 2 detected");
    } else if (Md == "M993") {
      Modulo = 3;
      //Serial.println("Module 3 detected");
    } else if (Md == "M994") {
      Modulo = 4;
      //Serial.println("Module 4detected");
    } else {
      Modulo = 0;
      //Serial.println("Unknown module");
    }
  } else {
    // Serial.println("Data too short for module ID extraction");
    Modulo = 0;
  }
}

void updateModuleStyles(int index, int opa[8]) {
  lv_label_set_text_fmt(ui_Temp1 + index, "%s", Temps[index]);
  for (int i = 0; i < 8; ++i) {
    lv_obj_set_style_opa(ui_Temp1 + index, opa[i], LV_PART_MAIN | LV_STATE_DEFAULT);
  }
}

void processModuleData() {
  if (Modulo > 0 && Modulo <= 4) {
    chronos[Modulo - 1].restart();
    Temps[Modulo - 1] = TempValor;

    if (MdAlr.endsWith("A0")) {
      fillOpacity(Modulo - 1, {255, 0, 0, 0, 255, 255, 255, 0});  // VERDE
    } else if (MdAlr.endsWith("A1")) {
      fillOpacity(Modulo - 1, {0, 0, 0, 255, 255, 255, 255, 0});  // LARANJA
    } else if (MdAlr.endsWith("A2")) {
      fillOpacity(Modulo - 1, {0, 0, 255, 0, 255, 255, 255, 0});  // RED
    } else if (MdAlr.endsWith("A3")) {
      fillOpacity(Modulo - 1, {0, 0, 255, 0, 0, 0, 0, 255});  // MANUAL
    }
    updateModuleStyles(Modulo - 1, opacities[Modulo - 1]);
  }
}

void fillOpacity(int index, std::initializer_list<int> opaList) {
  std::copy(opaList.begin(), opaList.end(), opacities[index]);
}

void checkModuleTimeouts() {
  for (int i = 0; i < 4; i++) {
    if (chronos[i].hasPassed(14000)) {
      fillOpacity(i, {0, 255, 0, 0, 0, 255, 255, 0});  // GRAY timeout
      updateModuleStyles(i, opacities[i]);
    }
  }
}

