
#ifdef USE_BCM2835_LIB
    #include <bcm2835.h>
#elif USE_WIRINGPI_LIB
    #include <wiringPi.h>
    #include <wiringPiSPI.h>
#elif USE_DEV_LIB
    #include <lgpio.h>
    #define LFLAGS 0
    #define NUM_MAXBUF  4
#endif
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "DEV_Config.h"
#include "Debug.h"
#include "dev_hardware_SPI.h"
#include "sysfs_gpio.h"
#include "fonts.h"
#include "GUI_BMP.h"
#include "GUI_Paint.h"
#include "KEY_APP.h"
#include "LCD_1in69.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <node.h>

using v8::Context;
using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Number;
using v8::Value;
using v8::Function;
using v8::ArrayBuffer;
using v8::Uint8Array;

UWORD *BlackImage;
UDOUBLE Imagesize = LCD_1IN69_HEIGHT*LCD_1IN69_WIDTH*2;

enum UpdateMode { AUTO = 0, MANUAL = 1 };
UpdateMode updateMode = AUTO;

void update() {
  if (updateMode == AUTO) {
    LCD_1IN69_Display(BlackImage);
  }
}

void init(const FunctionCallbackInfo<Value> &args)
{
  if(DEV_ModuleInit() != 0){
    exit(0);
  }
  Isolate* isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  if(args[0]->IsNumber()) {
    updateMode = (UpdateMode)args[0]->Uint32Value(context).ToChecked();
  }

  LCD_1IN69_Init(VERTICAL);
  LCD_1IN69_Clear(IMAGE_BACKGROUND);
  if((BlackImage = (UWORD *)malloc(Imagesize)) == NULL) {
    printf("Failed to apply for black memory...\n");
    exit(0);
  }
  Paint_NewImage(BlackImage, LCD_1IN69_WIDTH, LCD_1IN69_HEIGHT, 0, IMAGE_BACKGROUND, 16);
  Paint_Clear(IMAGE_BACKGROUND);
  update();
}

void exit(const FunctionCallbackInfo<Value> &args) {
  Isolate* isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();

  free(BlackImage);
  BlackImage = NULL;
  DEV_ModuleExit();
}

void clear_node(const FunctionCallbackInfo<Value>& args){
  Isolate* isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  uint16_t color = IMAGE_BACKGROUND;

  if(args[0]->IsNumber()) {
    color = args[3]->Uint32Value(context).ToChecked();
  }

  Paint_NewImage(BlackImage, LCD_1IN69_WIDTH, LCD_1IN69_HEIGHT, 0, color, 16);
  Paint_Clear(color);
  update();
}

void clear_partial(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = args.GetIsolate();
	Local<Context> context = isolate->GetCurrentContext();
	uint16_t color = IMAGE_BACKGROUND;
	
	if(args[0]->IsNumber() && args[1]->IsNumber() && args[2]->IsNumber() && args[3]->IsNumber()) {
		uint32_t x = args[0]->Uint32Value(context).ToChecked();
        	uint32_t y = args[1]->Uint32Value(context).ToChecked();
		uint32_t width = args[2]->Uint32Value(context).ToChecked();
		uint32_t height = args[3]->Uint32Value(context).ToChecked();

		Paint_ClearWindow(x, y, width+x, height+y, IMAGE_BACKGROUND);
	}
	update();
}

void drawString_node(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();

  // Check the number of arguments passed.
  if (args.Length() < 3) {
    // Throw an Error that is passed back to JavaScript
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments").ToLocalChecked()));
    return;
  }

  // Check the argument types
  if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments").ToLocalChecked()));
    return;
  }

  v8::String::Utf8Value arg2(isolate, args[2]);
  const char* s = *arg2;

  sFONT* my_font = &Font12;
  if(args[3]->IsNumber()) {
    uint16_t font_number = args[3]->Uint32Value(context).ToChecked();
    switch(font_number){
      case 8:
        my_font = &Font8;
        break;
      case 12:
        my_font = &Font12;
        break;
      case 16:
        my_font = &Font16;
        break;
      case 20:
        my_font = &Font20;
        break;
      case 24:
        my_font = &Font24;
        break;
    }
  }
  uint16_t color = FONT_FOREGROUND;
  uint16_t bcolor = FONT_BACKGROUND;
  if(args[4]->IsNumber()) {
    color = args[4]->Uint32Value(context).ToChecked();
  }
  if(args[5]->IsNumber()) {
    bcolor = args[5]->Uint32Value(context).ToChecked();
  }
  if (BlackImage) {
    Paint_DrawString_EN(
        args[0]->Uint32Value(context).ToChecked(),
        args[1]->Uint32Value(context).ToChecked(),
        s ? s : "",
        my_font,
        color,
        bcolor
        );
    update();
  } else {
    printf("Must call init() before any other methods\n");
  }
}

void drawCircle_node(const FunctionCallbackInfo<Value>& args){
  Isolate* isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  if (args.Length() < 3) {
    // Throw an Error that is passed back to JavaScript
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments").ToLocalChecked()));
    return;
  }
  if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments").ToLocalChecked()));
    return;
  }
  uint16_t Cx = 0, Cy = 0, Cr = 0, Ccolor = BLACK, isFill = 1;
  DRAW_FILL fill_mode = DRAW_FILL_FULL;
  Cx = args[0]->Uint32Value(context).ToChecked();
  Cy = args[1]->Uint32Value(context).ToChecked();
  Cr = args[2]->Uint32Value(context).ToChecked();
  if(args[3]->IsNumber()) {
    Ccolor = args[3]->Uint32Value(context).ToChecked();
  }
  if(args[4]->IsNumber()) {
    isFill = args[4]->Uint32Value(context).ToChecked();
    if(isFill == 0){
      fill_mode = DRAW_FILL_EMPTY;
    }
  }

  if (BlackImage) {
    Paint_DrawCircle( Cx, Cy, Cr, Ccolor, DOT_PIXEL_DFT, fill_mode);
    update();
  } else {
    printf("Must call init() before any other methods\n");
  }
}

void drawCheck_node(const FunctionCallbackInfo<Value>& args){
  Isolate* isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  if (args.Length() < 3) {
    // Throw an Error that is passed back to JavaScript
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments").ToLocalChecked()));
    return;
  }
  if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments").ToLocalChecked()));
    return;
  }
  uint16_t x = 0, y = 0, Ccolor = BLACK;
  if(args[2]->IsNumber()){
    Ccolor = args[2]->Uint32Value(context).ToChecked();
  }
  x = args[0]->Uint32Value(context).ToChecked();
  y = args[1]->Uint32Value(context).ToChecked();
  if (BlackImage) {
    Paint_DrawLine(x    , y    , x + 5,  y + 5, Ccolor, DOT_PIXEL_DFT, LINE_STYLE_SOLID);
    Paint_DrawLine(x + 5, y + 5, x + 12, y - 8, Ccolor, DOT_PIXEL_DFT, LINE_STYLE_SOLID);
    update();
  } else {
    printf("Must call init() before any other methods\n");
  }
}

void drawRect_node(const FunctionCallbackInfo<Value>& args){
  Isolate* isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  if (args.Length() < 4) {
    // Throw an Error that is passed back to JavaScript
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments").ToLocalChecked()));
    return;
  }
  if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments").ToLocalChecked()));
    return;
  }
  uint16_t x1 = 0, y1 = 0, x2 = LCD_1IN69_HEIGHT, y2 = LCD_1IN69_WIDTH, Ccolor = IMAGE_BACKGROUND;
  if(args[4]->IsNumber()){
    Ccolor = args[4]->Uint32Value(context).ToChecked();
  }
  x1 = args[0]->Uint32Value(context).ToChecked();
  y1 = args[1]->Uint32Value(context).ToChecked();
  x2 = args[2]->Uint32Value(context).ToChecked();
  y2 = args[3]->Uint32Value(context).ToChecked();
  if (BlackImage) {
    Paint_DrawRectangle(x1, y1, x2, y2, Ccolor, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    update();
  } else {
    printf("Must call init() before any other methods\n");
  }
}


void draw(const FunctionCallbackInfo<Value>& args){
  Isolate* isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  if (args.Length() > 0) {
    UpdateMode oldMode = updateMode;
    updateMode = MANUAL;

    Local<Function> cb = Local<Function>::Cast(args[0]);
    cb->Call(context, Null(isolate), 0, NULL).ToLocalChecked();

    updateMode = oldMode;
  }

  LCD_1IN69_Display(BlackImage);
}

void backlight(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = args.GetIsolate();
	Local<Context> context = isolate->GetCurrentContext();
  	if (args.Length() > 0) {
		DEV_SetBacklight(args[0]->Uint32Value(context).ToChecked());
	}
}

void drawPicture(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
	Local<Context> context = isolate->GetCurrentContext();
    if (!args[0]->IsUint8Array()) {
	isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Buffer expected").ToLocalChecked()));
    	return;
    }
    Local<Uint8Array> uint8Array = args[0].As<Uint8Array>();
    Local<ArrayBuffer> buffer = uint8Array->Buffer();
    uint8_t* data = static_cast<uint8_t*>(buffer->GetBackingStore()->Data()) + uint8Array->ByteOffset();
    //uint8_t* data = static_cast<uint8_t*>(buffer->GetContents().Data()) + uint8Array->ByteOffset();
    // Here you need to decode the PNG buffer and draw it on the LCD
    // For example:
    Paint_DrawImage(data, 0, 0, 240, 280);
}

void Initialize(Local<Object> exports) {
  NODE_SET_METHOD(exports, "init", init);
  NODE_SET_METHOD(exports, "exit", exit);
  NODE_SET_METHOD(exports, "backlight", backlight);
  NODE_SET_METHOD(exports, "clear", clear_node);
  NODE_SET_METHOD(exports, "clearPartial", clear_partial);
  NODE_SET_METHOD(exports, "drawPicture", drawPicture);
  NODE_SET_METHOD(exports, "drawString", drawString_node);
  NODE_SET_METHOD(exports, "drawCircle", drawCircle_node);
  NODE_SET_METHOD(exports, "drawCheck", drawCheck_node);
  NODE_SET_METHOD(exports, "drawRect", drawRect_node);
  NODE_SET_METHOD(exports, "draw", draw);
  //NODE_SET_METHOD(exports, "delay", delay);
}

NODE_MODULE(my_lcd, Initialize)
