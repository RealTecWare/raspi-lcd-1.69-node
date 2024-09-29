{
  "targets": [
    {
      "target_name": "my_lcd",
			"defines": [
        "USE_DEV_LIB"
			],
	"ldflags": [
	"-Wl,-z,defs"
        ],
      "sources": [
        "lib/Config/DEV_Config.c",
        "lib/Config/dev_hardware_SPI.c",
        "lib/Config/sysfs_gpio.c",
        "lib/Fonts/font12.c",
        "lib/Fonts/font16.c",
        "lib/Fonts/font20.c",
        "lib/Fonts/font24.c",
        "lib/Fonts/font8.c",
        "lib/GUI/GUI_BMP.c",
        "lib/GUI/GUI_Paint.c",
        "lib/GUI/KEY_APP.c",
        "lib/LCD/LCD_1in69.c",
        "main.cc"
      ],
      "include_dirs": [
        "lib/Config",
        "lib/Fonts",
        "lib/GUI",
        "lib/LCD"
      ],
	"link_settings" : {
        "libraries" : ["-ldl"]
      },
      "libraries": [
        "-L/usr/local/lib",
	"-lwiringPi",
	"-llgpio"
      ]
    }
  ]
}
