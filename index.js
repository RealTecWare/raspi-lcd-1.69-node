const bindings = require("bindings");
const my_lcd = bindings("my_lcd");

const HEIGHT = 280;
const WIDTH = 240;

const Colors = {
  WHITE: 0xffff,
  BLACK: 0x0000,
  BLUE: 0x001f,
  BRED: 0xf81f,
  GRED: 0xffe0,
  GBLUE: 0x07ff,
  RED: 0xf800,
  MAGENTA: 0xf81f,
  GREEN: 0x07e0,
  CYAN: 0x7fff,
  YELLOW: 0xffe0,
  BROWN: 0xbc40,
  BRRED: 0xfc07,
  GRAY: 0x8430,
};

Colors["IMAGE_BACKGROUND"] = Colors.WHITE;
Colors["FONT_FOREGROUND"] = Colors.BLACK;
Colors["FONT_BACKGROUND"] = Colors.WHITE;

my_lcd.WIDTH = WIDTH;
my_lcd.HEIGHT = HEIGHT;
my_lcd.Colors = Colors;

module.exports = my_lcd;
//export { my_lcd as default, my_lcd as LCD, HEIGHT, WIDTH, Colors };
