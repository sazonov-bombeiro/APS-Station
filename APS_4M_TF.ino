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
Chrono chrono1, chrono2, chrono3, chrono4;

boolean passedFirstPost = false;

String DadosInput = "";
String TempValor, Md, MdAlr, Temp1, Temp2, Temp3, Temp4;
char CharInput[16], charMdAlr[4], charMd[2];
int Modulo = 0;

int Green1_opal, Green2_opal, Green3_opal, Green4_opal;
int Grey1_opal, Grey2_opal, Grey3_opal, Grey4_opal;
int Red1_opal, Red2_opal, Red3_opal, Red4_opal;
int Yellou1_opal, Yellou2_opal, Yellou3_opal, Yellou4_opal;
int Help1_opal, Help2_opal, Help3_opal, Help4_opal;
int Temp1Back_opal, Temp2Back_opal, Temp3Back_opal, Temp4Back_opal;
int Temp1_opal, Temp2_opal, Temp3_opal, Temp4_opal;
int ImgTemp1_opal, ImgTemp2_opal, ImgTemp3_opal, ImgTemp4_opal;

void setup() {  //------------------------------------------------------------------  SETUP -------------------------------------------------------
  String title = "TouchFire_Control_Painel";

  Serial.begin(9600);
  Serial1.begin(9600);

  chrono1.restart();
  chrono2.restart();
  chrono3.restart();
  chrono4.restart();

  Serial.println("Initializing board");
  Board *board = new Board();
  board->init();

#if LVGL_PORT_AVOID_TEARING_MODE
  auto lcd = board->getLCD();
  // When avoid tearing function is enabled, the frame buffer number should be set in the board driver
  lcd->configFrameBufferNumber(LVGL_PORT_DISP_BUFFER_NUM);
#if ESP_PANEL_DRIVERS_BUS_ENABLE_RGB && CONFIG_IDF_TARGET_ESP32S3
  auto lcd_bus = lcd->getBus();

  if (lcd_bus->getBasicAttributes().type == ESP_PANEL_BUS_TYPE_RGB) {
    static_cast<BusRGB *>(lcd_bus)->configRGB_BounceBufferSize(lcd->getFrameWidth() * 10);
  }
#endif
#endif
  assert(board->begin());

  //  Serial.println("Initializing LVGL");
  lvgl_port_init(board->getLCD(), board->getTouch());

  Serial.println("Creating UI");
  /* Lock the mutex due to the LVGL APIs are not thread-safe */
  lvgl_port_lock(-1);


  ui_init();
  lvgl_port_unlock();
}

void loop() {  // ------------------------------------------------- LOOP ----------------- LOOP --------------------------------------------
  /*  Serial.println("IDLE loop");
    lv_label_set_text_fmt(ui_RXMess,"%s",DadosInput);
    lv_label_set_text_fmt(ui_RXMess,"%s",DadosInput);
    lv_label_set_text_fmt(ui_RXMess,"%s",DadosInput);
    delay(1000);*/

  // Read data from serial
 /* while (Serial1.available()) {
    
    DadosInput = Serial1.readString();
    // char c = mySerial.read();
    // DadosInput += c;
    delay(10);
  }
*/
  if (Serial1.available()) {
    DadosInput = Serial1.readString();
    delay(50);
  }

  // Process received data if we have any
  if (DadosInput.length() > 0) {
    //Serial.print("Received: ");
   // Serial.println(DadosInput);

    // Process data**********************************************************************************************   PARSER  ***************************
    if (DadosInput.length() >= 10) {  // Check if we have enough data for parsing
      NumMod();                       // Identify module number
      Temp();                         // Extract temperature
      ModAlrt();                      // Extract module alert

      // Process based on module number
      processModuleData();
    } else {
     // Serial.println("Warning: Received data too short for processing");
    }
    //*******************************************************---------------------------------******** DADOS PARA DISPLAY  ****************************************************

    //**************************   MODULO 1  ***********************************
    lv_label_set_text_fmt(ui_Temp1, "%s", Temp1);
    lv_obj_set_style_opa(ui_Temp1, Temp1_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_Gray1, Grey1_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_Red1, Red1_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_Yellou1, Yellou1_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_Green1, Green1_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_HELP1, Help1_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_ImgTemp1, ImgTemp1_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_Temp1Back, Temp1Back_opal, LV_PART_MAIN | LV_STATE_DEFAULT);



    //**************************   MODULO 2  ***********************************
    lv_label_set_text_fmt(ui_Temp2, "%s", Temp2);
    lv_obj_set_style_opa(ui_Temp2, Temp2_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_Gray2, Grey2_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_Red2, Red2_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_Yellou2, Yellou2_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_Green2, Green2_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_HELP2, Help2_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_ImgTemp2, ImgTemp2_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_Temp2Back, Temp2Back_opal, LV_PART_MAIN | LV_STATE_DEFAULT);



    //**************************   MODULO 3  ***********************************
    lv_label_set_text_fmt(ui_Temp3, "%s", Temp3);
    lv_obj_set_style_opa(ui_Temp3, Temp3_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_Gray3, Grey3_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_Red3, Red3_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_Yellou3, Yellou3_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_Green3, Green3_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_HELP3, Help3_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_ImgTemp3, ImgTemp3_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_Temp3Back, Temp3Back_opal, LV_PART_MAIN | LV_STATE_DEFAULT);


    //**************************   MODULO 4  ***********************************
    lv_label_set_text_fmt(ui_Temp4, "%s", Temp4);
    lv_obj_set_style_opa(ui_Temp4, Temp4_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_Gray4, Grey4_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_Red4, Red4_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_Yellou4, Yellou4_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_Green4, Green4_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_HELP4, Help4_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_ImgTemp4, ImgTemp4_opal, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_Temp4Back, Temp4Back_opal, LV_PART_MAIN | LV_STATE_DEFAULT);


    //*************************************-------------------------------**********   DEBUG   ****************-------------------------------**************

   //  lv_label_set_text_fmt(ui_Titulo_Touch_Fire, "%s", TempValor);


    // Clear input data for next read
    DadosInput = "  ";
  }
  // Check for module timeouts
  checkModuleTimeouts();

  delay(50);  // Small delay to prevent CPU hogging
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

void processModuleData() {  //------------------------  VOID --Process Modulos Data -------------------------------------------------
  switch (Modulo) {
    case 1:  // Module 1
      {

        chrono1.restart();  // Reset timeout timer

        // Process alert level for Module 1
        if (MdAlr == "M991A0") {  //--------------------------("Module 1 Alert 0");-------------------------------
          Temp1 = TempValor;
          Green1_opal = 255;  //VERDE
          Grey1_opal = 0;
          Red1_opal = 0;
          Yellou1_opal = 0;
          Temp1_opal = 255;
          Temp1Back_opal = 255;
          ImgTemp1_opal = 255;
          Help1_opal = 0;


        } else if (MdAlr == "M991A1") {  // -----------------------("Module 1 Alert 1");----------------------------------
          Temp1 = TempValor;
          Green1_opal = 0;
          Grey1_opal = 0;
          Red1_opal = 0;
          Yellou1_opal = 255;  //  LARANGA
          Temp1_opal = 255;
          Temp1Back_opal = 255;
          ImgTemp1_opal = 255;
          Help1_opal = 0;


        } else if (MdAlr == "M991A2") {  // -----------------------("Module 1 Alert 2");----------------------------------
          Temp1 = TempValor;
          Green1_opal = 0;
          Grey1_opal = 0;
          Red1_opal = 255;  // RED
          Yellou1_opal = 0;
          Temp1_opal = 255;
          Temp1Back_opal = 255;
          ImgTemp1_opal = 255;
          Help1_opal = 0;

        } else if (MdAlr == "M991A3") {  // -----------------------("Module 1 Alert MANUAL");----------------------------------
          Green1_opal = 0;
          Grey1_opal = 0;
          Red1_opal = 255;  // RED
          Yellou1_opal = 0;
          Temp1_opal = 0;
          Temp1Back_opal = 0;
          ImgTemp1_opal = 0;
          Help1_opal = 255;
        }
      }
      break;

    case 2:  // Module 2
      {

        chrono2.restart();  // Reset timeout timer

        // Process alert level for Module 2
        if (MdAlr == "M992A0") {  // -----------------------("Module 2 Alert 0");----------------------------------
          Temp2 = TempValor;
          Green2_opal = 250;  //VERDE
          Grey2_opal = 1;
          Red2_opal = 1;
          Yellou2_opal = 1;
          Temp2_opal = 250;
          Temp2Back_opal = 250;
          ImgTemp2_opal = 250;
          Help2_opal = 1;


        } else if (MdAlr == "M992A1") {  // -----------------------("Module 2 Alert 1");----------------------------------
          Temp2 = TempValor;
          Green2_opal = 0;
          Grey2_opal = 0;
          Red2_opal = 0;
          Yellou2_opal = 255;  // LARANGA
          Temp2_opal = 255;
          Temp2Back_opal = 255;
          ImgTemp2_opal = 255;
          Help2_opal = 0;


        } else if (MdAlr == "M992A2") {  // -----------------------("Module 2 Alert 2");----------------------------------
          Temp2 = TempValor;
          Green2_opal = 0;
          Grey2_opal = 0;
          Red2_opal = 255;  // RED
          Yellou2_opal = 0;
          Temp2_opal = 255;
          Temp2Back_opal = 255;
          ImgTemp2_opal = 255;
          Help2_opal = 0;

        } else if (MdAlr == "M992A3") {  // -----------------------("Module 2 Alert MANUAL ");----------------------------------
          Green2_opal = 0;
          Grey2_opal = 0;
          Red2_opal = 255;  // RED
          Yellou2_opal = 0;
          Temp2_opal = 0;
          Temp2Back_opal = 0;
          ImgTemp2_opal = 0;
          Help2_opal = 255;
        }
      }
      break;

    case 3:  // Module 1
      {

        chrono3.restart();  // Reset timeout timer

        // Process alert level for Module 3
        if (MdAlr == "M993A0") {  // -----------------------("Module 3 Alert 0");----------------------------------
          Temp3 = TempValor;
          Green3_opal = 255;  //VERDE
          Grey3_opal = 0;
          Red3_opal = 0;
          Yellou3_opal = 0;
          Temp3_opal = 255;
          Temp3Back_opal = 255;
          ImgTemp3_opal = 255;
          Help3_opal = 0;

        } else if (MdAlr == "M993A1") {  // -----------------------("Module 3 Alert 1");----------------------------------
          Temp3 = TempValor;
          Green3_opal = 0;
          Grey3_opal = 0;
          Red3_opal = 0;
          Yellou3_opal = 255;  // LARANGA
          Temp3_opal = 255;
          Temp3Back_opal = 255;
          ImgTemp3_opal = 255;
          Help3_opal = 0;

        } else if (MdAlr == "M993A2") {  // -----------------------("Module 3 Alert 2");----------------------------------
          Temp3 = TempValor;
          Green3_opal = 0;
          Grey3_opal = 0;
          Red3_opal = 255;  // RED
          Yellou3_opal = 0;
          Temp3_opal = 255;
          Temp3Back_opal = 255;
          ImgTemp3_opal = 255;
          Help3_opal = 0;

        } else if (MdAlr == "M993A3") {  // -----------------------("Module 3 Alert MANUAL  ");----------------------------------
          Green3_opal = 0;
          Grey3_opal = 0;
          Red3_opal = 255;  // RED
          Yellou3_opal = 0;
          Temp3_opal = 0;
          Temp3Back_opal = 0;
          ImgTemp3_opal = 0;
          Help3_opal = 255;
        }
      }
      break;

    case 4:  // Module 4
      {

        chrono4.restart();  // Reset timeout timer

        // Process alert level for Module 4
        if (MdAlr == "M994A0") {  // -----------------------("Module 4 Alert 0");----------------------------------
          Temp4 = TempValor;
          Green4_opal = 255;  //VERDE
          Grey4_opal = 0;
          Red4_opal = 0;
          Yellou4_opal = 0;
          Temp4_opal = 255;
          Temp4Back_opal = 255;
          ImgTemp4_opal = 255;
          Help4_opal = 0;

        } else if (MdAlr == "M994A1") {  // -----------------------("Module 4 Alert 1");----------------------------------
          Temp4 = TempValor;
          Green4_opal = 0;
          Grey4_opal = 0;
          Red4_opal = 0;
          Yellou4_opal = 255;  // LARANGA
          Temp4_opal = 255;
          Temp4Back_opal = 255;
          ImgTemp4_opal = 255;
          Help4_opal = 0;


        } else if (MdAlr == "M994A2") {  // -----------------------("Module 4 Alert 2");----------------------------------
          Temp4 = TempValor;
          Green4_opal = 0;
          Grey4_opal = 0;
          Red4_opal = 255;  // RED
          Yellou4_opal = 0;
          Temp4_opal = 255;
          Temp4Back_opal = 255;
          ImgTemp4_opal = 255;
          Help4_opal = 0;

        } else if (MdAlr == "M994A3") {  // -----------------------("Module 4 Alert MANUAL ");----------------------------------
          Green4_opal = 0;
          Grey4_opal = 0;
          Red4_opal = 255;  // RED
          Yellou4_opal = 0;
          Temp4_opal = 0;
          Temp4Back_opal = 0;
          ImgTemp4_opal = 0;
          Help4_opal = 255;
        }
      }
      break;


    default:
     // Serial.println("Unknown module - no processing");
      break;
  }
}

void checkModuleTimeouts() {
  // Check for module timeouts
  if (chrono1.hasPassed(14000)) {  // ---------------------("Module 1 timeout");------------------------------------
    Green1_opal = 0;
    Grey1_opal = 255;  // GRAY
    Red1_opal = 0;
    Yellou1_opal = 0;
    Temp1_opal = 0;
    Temp1Back_opal = 255;
    ImgTemp1_opal = 255;
    Help1_opal = 0;
  }

  if (chrono2.hasPassed(14000)) {  // ---------------------("Module 2 timeout");------------------------------------
    Grey2_opal = 250;              // GRAY
    Red2_opal = 0;
    Yellou2_opal = 0;
    Green2_opal = 0;
    Temp2_opal = 1;
    Temp2Back_opal = 250;
    ImgTemp2_opal = 250;
    Help2_opal = 0;
  }

  if (chrono3.hasPassed(14000)) {  // --------------------------------("Module 3 timeout"); -------------------------------
    Grey3_opal = 255;              // GRAY
    Red3_opal = 0;
    Yellou3_opal = 0;
    Green3_opal = 0;
    Temp3_opal = 0;
    Temp3Back_opal = 255;
    ImgTemp3_opal = 255;
    Help3_opal = 0;
  }

  if (chrono4.hasPassed(14000)) {  // --------------------------------("Module 4 timeout"); -------------------------------
    Grey4_opal = 255;              // GRAY
    Red4_opal = 0;
    Yellou4_opal = 0;
    Green4_opal = 0;
    Temp4_opal = 0;
    Temp4Back_opal = 255;
    ImgTemp4_opal = 255;
    Help4_opal = 0;
  }
}
