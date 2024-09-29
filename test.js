const LCD = require(".");
const { Colors, HEIGHT, WIDTH } = LCD;
const { RED, BLACK, WHITE, GREEN, BLUE, GRAY } = Colors;

const x = WIDTH / 2;
const y = HEIGHT / 2;
const fontSize = 24;
const r = 20;
const fill = 1;
const color = RED;

  // Reset the LCD and set the draw mode to manual
  LCD.init();
  LCD.clear(BLACK);
setInterval(()=>{
	LCD.clear(BLACK);
	LCD.drawString(x, y, new Date(Date.now()).toLocaleTimeString(), 12, BLACK, WHITE);
}, 1000);
setTimeout(()=>{
  LCD.clear(BLACK);
  LCD.exit();
}, 30000);
