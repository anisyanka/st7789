#include "st7789.h"

typedef enum {
	CMD_MADCTL_MY  = 0x80,
	CMD_MADCTL_MX  = 0x40,
	CMD_MADCTL_MV  = 0x20,
	CMD_MADCTL_ML  = 0x10,
	CMD_MADCTL_RGB = 0x00,
	CMD_MADCTL_BGR = 0x08,
	CMD_MADCTL_MH  = 0x04,
	CMD_NOP        = 0x00,
	CMD_SWRESET    = 0x01,
	CMD_RDDID      = 0x04,
	CMD_RDDST      = 0x09,
	CMD_SLPIN      = 0x10,
	CMD_SLPOUT     = 0x11,
	CMD_PTLON      = 0x12,
	CMD_NORON      = 0x13,
	CMD_INVOFF     = 0x20,
	CMD_INVON      = 0x21,
	CMD_GAMSET     = 0x26,
	CMD_DISPOFF    = 0x28,
	CMD_DISPON     = 0x29,
	CMD_CASET      = 0x2A,
	CMD_RASET      = 0x2B,
	CMD_RAMWR      = 0x2C,
	CMD_RAMRD      = 0x2E,
	CMD_PTLAR      = 0x30,
	CMD_MADCTL     = 0x36,
	CMD_IDMOFF     = 0x38,
	CMD_IDMON      = 0x39,
	CMD_COLMOD     = 0x3A,
	CMD_RAMCTRL    = 0xB0,
	CMD_FRMCTR1    = 0xB1,
	CMD_RGBCTRL    = 0xB1,
	CMD_FRMCTR2    = 0xB2,
	CMD_PORCTRL    = 0xB2,
	CMD_FRMCTR3    = 0xB3,
	CMD_FRCTRL1    = 0xB3,
	CMD_INVCTR     = 0xB4,
	CMD_PARCTRL    = 0xB5,
	CMD_DISSET5    = 0xB6,
	CMD_GCTRL      = 0xB7,
	CMD_VCOMS      = 0xBB,
	CMD_PWCTR1     = 0xC0,
	CMD_LCMCTRL    = 0xC0,
	CMD_PWCTR2     = 0xC1,
	CMD_IDSET      = 0xC1,
	CMD_PWCTR3     = 0xC2,
	CMD_VDVVRHEN   = 0xC2,
	CMD_PWCTR4     = 0xC3,
	CMD_VRHS       = 0xC3,
	CMD_PWCTR5     = 0xC4,
	CMD_VDVS       = 0xC4,
	CMD_VMCTR1     = 0xC5,
	CMD_VCMOFSET   = 0xC5,
	CMD_FRCTRL2    = 0xC6,
	CMD_PWCTRL1     = 0xD0,
	CMD_RDID1      = 0xDA,
	CMD_RDID2      = 0xDB,
	CMD_RDID3      = 0xDC,
	CMD_RDID4      = 0xDD,
	CMD_PWCTR6     = 0xFC,
	CMD_GMCTRP1    = 0xE0,
	CMD_GMCTRN1    = 0xE1,
	CMD_COLOR_MODE_16bit = 0x55,
	CMD_COLOR_MODE_18bit = 0x66,
} lcd_cmds;

#if defined(ST7789_LCD_135x240)
  #if (ST7789_LCD_ROTATION == 0) || (ST7789_LCD_ROTATION == 2)
    #define ST7789_LCD_WIDTH  135
    #define ST7789_LCD_HEIGHT 240
  #elif (ST7789_LCD_ROTATION == 1) || (ST7789_LCD_ROTATION == 3)
    #define ST7789_LCD_WIDTH  240
    #define ST7789_LCD_HEIGHT 135
   #endif
#elif defined(ST7789_LCD_240x240)
  #define ST7789_LCD_WIDTH  240
  #define ST7789_LCD_HEIGHT 240
#elif defined(ST7789_LCD_240x280)
  #if (ST7789_LCD_ROTATION == 0) || (ST7789_LCD_ROTATION == 2)
    #define ST7789_LCD_WIDTH  240
    #define ST7789_LCD_HEIGHT 280
  #elif (ST7789_LCD_ROTATION == 1) || (ST7789_LCD_ROTATION == 3)
    #define ST7789_LCD_WIDTH  280
    #define ST7789_LCD_HEIGHT 240
  #endif
#elif defined (ST7789_LCD_240x320)
  #if (ST7789_LCD_ROTATION == 0) || (ST7789_LCD_ROTATION == 2)
    #define ST7789_LCD_WIDTH  240
    #define ST7789_LCD_HEIGHT 320
  #elif (ST7789_LCD_ROTATION == 1) || (ST7789_LCD_ROTATION == 3)
    #define ST7789_LCD_WIDTH  320
    #define ST7789_LCD_HEIGHT 240
  #endif
#else
  #error "Set display type in build flags"
#endif

#ifndef ST7789_LCD_ROTATION
  #error "Set rotation in build flags"
#endif

#if (ST7789_LCD_ROTATION == 0)
  #define ROTATION_CMD (CMD_MADCTL_MX | CMD_MADCTL_MY | CMD_MADCTL_RGB)
#elif (ST7789_LCD_ROTATION == 1)
  #define ROTATION_CMD (CMD_MADCTL_MY | CMD_MADCTL_MV | CMD_MADCTL_RGB)
#elif (ST7789_LCD_ROTATION == 2)
  #define ROTATION_CMD (CMD_MADCTL_RGB)
#elif (ST7789_LCD_ROTATION == 3)
  #define ROTATION_CMD (CMD_MADCTL_MX | CMD_MADCTL_MV | CMD_MADCTL_RGB)
#endif

static const uint8_t init_cmd[] = {
	0,  CMD_SLPOUT,
	1,  CMD_COLMOD,  CMD_COLOR_MODE_16bit,
	5,  CMD_PORCTRL, 0x0C, 0x0C, 0x00, 0x33, 0x33,   // Standard porch
	//5,  CMD_PORCTRL, 0x01, 0x01, 0x00, 0x11, 0x11, // Minimum porch (7% faster screen refresh rate)
	1,  CMD_GCTRL,   0x35,                           // Gate Control, Default value
	1,  CMD_VCOMS,   0x19,                           // VCOM setting 0.725v (default 0.75v for 0x20)
	1,  CMD_LCMCTRL, 0X2C,                           // LCMCTRL, Default value
	1,  CMD_VDVVRHEN,0x01,                           // VDV and VRH command Enable, Default value
	1,  CMD_VRHS,    0x12,                           // VRH set, +-4.45v (default +-4.1v for 0x0B)
	1,  CMD_VDVS,    0x20,                           // VDV set, Default value
	1,  CMD_FRCTRL2, 0x0F,                           // Frame rate control in normal mode, Default refresh rate (60Hz)
	//1,  CMD_FRCTRL2, 0x01,                         // Frame rate control in normal mode, Max refresh rate (111Hz)
	2,  CMD_PWCTRL1, 0xA4, 0xA1,
	1,  CMD_MADCTL,  ROTATION_CMD,
	14, CMD_GMCTRP1, 0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23,
	14, CMD_GMCTRN1, 0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23,
	0,  CMD_INVOFF,
	0,  CMD_NORON
};

static void write_command(st7789_dev_t *dev, uint8_t cmd)
{
	dev->ll->cs_pin_reset();
	dev->ll->dc_pin_reset();
	dev->ll->spi_send_data(&cmd, sizeof(cmd));
	dev->ll->cs_pin_set();
}

static void write_command_with_data(st7789_dev_t *dev, uint8_t *cmd, uint8_t argc)
{
	dev->ll->cs_pin_reset();
	dev->ll->dc_pin_reset();
	dev->ll->spi_send_data(cmd, 1);
	if (argc) {
		dev->ll->dc_pin_set();
		dev->ll->spi_send_data(cmd + 1, argc);
	}
	dev->ll->cs_pin_set();
}

static void write_data(st7789_dev_t *dev, uint8_t *data, size_t len)
{
	dev->ll->cs_pin_reset();
	dev->ll->dc_pin_set();
	dev->ll->spi_send_data(data, len);
	dev->ll->cs_pin_set();
}

static void reset_lcd(st7789_dev_t *dev)
{
	dev->ll->rst_pin_set();
	dev->ll->delay_ms(200);
	dev->ll->rst_pin_reset();
	dev->ll->delay_ms(200);
	dev->ll->rst_pin_set();
	dev->ll->delay_ms(200);
}

void st7789_set_address_window(st7789_dev_t *dev,
							   uint16_t x0, uint16_t y0,
							   uint16_t x1, uint16_t y1)
{
	uint16_t x_start = x0 + dev->x_shift;
	uint16_t x_end = x1 + dev->x_shift;
	uint16_t y_start = y0 + dev->y_shift;
	uint16_t y_end = y1 + dev->y_shift;


	/* Column Address set */
	{
		uint8_t cmd[] = { CMD_CASET, x_start >> 8, x_start & 0xFF, x_end >> 8, x_end & 0xFF };
		write_command_with_data(dev, cmd, sizeof(cmd) - 1);
	}

	/* Row Address set */
	{
		uint8_t cmd[] = { CMD_RASET, y_start >> 8, y_start & 0xFF, y_end >> 8, y_end & 0xFF };
		write_command_with_data(dev, cmd, sizeof(cmd) - 1);
	}

	/* Write to RAM */
	{
		uint8_t cmd[] = { CMD_RAMWR };
		write_command_with_data(dev, cmd, sizeof(cmd) - 1);
	}
}

void st7789_init(st7789_dev_t *dev, st7789_ll_t *ll)
{
	if (!dev || !ll) {
		return;
	}

	dev->ll = ll;
	dev->width = ST7789_LCD_WIDTH;
	dev->heigh = ST7789_LCD_HEIGHT;
	dev->rotation = ST7789_LCD_ROTATION;
	dev->x_shift = 0;
	dev->y_shift = 0;

	reset_lcd(dev);

	for (uint16_t i = 0; i < sizeof(init_cmd); ) {
		write_command_with_data(dev, (uint8_t*)&init_cmd[i + 1], init_cmd[i]);
		i += init_cmd[i] + 2;
	}

	/* Enable power */
	write_command(dev, CMD_DISPON);
}

void st7789_fill_color(st7789_dev_t *dev, uint16_t color)
{
	uint16_t i, j;

	st7789_set_address_window(dev, 0, 0, dev->width - 1, dev->heigh - 1);

	for (i = 0; i < dev->width; i++) {
		for (j = 0; j < dev->heigh; j++) {
			uint8_t data[] = { color >> 8, color & 0xFF };
			write_data(dev, data, sizeof(data));
		}
	}
}

void st7789_set_pixel(st7789_dev_t *dev, uint16_t x, uint16_t y, uint16_t color)
{
	if ((x < 0) || (x > dev->width - 1) || (y < 0) || (y > dev->heigh - 1)) {
		return;
	}

	uint8_t data[2] = {color >> 8, color & 0xFF};

	st7789_set_address_window(dev, x, y, x, y);
	write_data(dev, data, sizeof(data));
}
