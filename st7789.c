#include "st7789.h"

/* Colors in RGB565 format */
#define WHITE		(uint16_t)(0xFFFF)
#define BLACK		(uint16_t)(0x0000)
#define BLUE		(uint16_t)(0x001F)
#define RED			(uint16_t)(0xF800)
#define MAGENTA 	(uint16_t)(0xF81F)
#define GREEN		(uint16_t)(0x07E0)
#define CYAN		(uint16_t)(0x7FFF)
#define YELLOW		(uint16_t)(0xFFE0)
#define GRAY		(uint16_t)(0x8430)
#define BRED		(uint16_t)(0xF81F)
#define GRED		(uint16_t)(0xFFE0)
#define GBLUE		(uint16_t)(0x07FF)
#define BROWN		(uint16_t)(0xBC40)
#define BRRED		(uint16_t)(0xFC07)
#define DARKBLUE	(uint16_t)(0x01CF)
#define LIGHTBLUE	(uint16_t)(0x7D7C)
#define GRAYBLUE	(uint16_t)(0x5458)
#define LIGHTGREEN	(uint16_t)(0x841F)
#define LGRAY		(uint16_t)(0xC618)
#define LGRAYBLUE	(uint16_t)(0xA651)
#define LBBLUE		(uint16_t)(0x2B12)

/* Control registers and constant codes */
#define ST7789_NOP     0x00
#define ST7789_SWRESET 0x01
#define ST7789_RDDID   0x04
#define ST7789_RDDST   0x09
#define ST7789_SLPIN   0x10
#define ST7789_SLPOUT  0x11
#define ST7789_PTLON   0x12
#define ST7789_NORON   0x13
#define ST7789_INVOFF  0x20
#define ST7789_INVON   0x21
#define ST7789_DISPOFF 0x28
#define ST7789_DISPON  0x29
#define ST7789_CASET   0x2A
#define ST7789_RASET   0x2B
#define ST7789_RAMWR   0x2C
#define ST7789_RAMRD   0x2E
#define ST7789_PTLAR   0x30
#define ST7789_COLMOD  0x3A
#define ST7789_MADCTL  0x36
#define ST7789_PORCTRL 0xB2

#define ST7789_COLOR_MODE_RGB565 0x55
#define ST7789_COLOR_MODE_RGB666 0x66

/*
 * Memory Data Access Control Register (0x36H)
 * MAP:     D7  D6  D5  D4  D3  D2  D1  D0
 * param:   MY  MX  MV  ML  RGB MH  -   -
 **/

/* Page Address Order ('0': Top to Bottom, '1': the opposite) */
#define ST7789_MADCTL_MY	0x80
/* Column Address Order ('0': Left to Right, '1': the opposite) */
#define ST7789_MADCTL_MX	0x40
/* Page/Column Order ('0' = Normal Mode, '1' = Reverse Mode) */
#define ST7789_MADCTL_MV	0x20
/* Line Address Order ('0' = LCD Refresh Top to Bottom, '1' = the opposite) */
#define ST7789_MADCTL_ML	0x10
/* RGB/BGR Order ('0' = RGB, '1' = BGR) */
#define ST7789_MADCTL_RGB	0x00

#define ST7789_RDID1	0xDA
#define ST7789_RDID2	0xDB
#define ST7789_RDID3	0xDC
#define ST7789_RDID4	0xDD

static void write_command(st7789_dev_t *dev, uint8_t cmd)
{
	dev->ll->cs_pin_reset();
	dev->ll->dc_pin_reset();
	dev->ll->spi_send_data(&cmd, sizeof(cmd));
	dev->ll->cs_pin_set();
}

static void write_data(st7789_dev_t *dev, uint8_t *data, size_t len)
{
	dev->ll->cs_pin_reset();
	dev->ll->dc_pin_set();
	dev->ll->spi_send_data(data, len);
	dev->ll->cs_pin_set();
}

static void write_data_byte(st7789_dev_t *dev, uint8_t byte)
{
	dev->ll->cs_pin_reset();
	dev->ll->dc_pin_set();
	dev->ll->spi_send_data(&byte, sizeof(byte));
	dev->ll->cs_pin_set();
}

static void reset_lcd(st7789_dev_t *dev)
{
	dev->ll->delay_ms(25);
	dev->ll->rst_pin_reset();
	dev->ll->delay_ms(25);
	dev->ll->rst_pin_set();
	dev->ll->delay_ms(50);
}

void st7789_init(st7789_dev_t *dev,
				 st7789_ll_t *ll,
				 uint16_t width,
				 uint16_t heigh,
				 uint32_t rotation)
{
	if (!dev || !ll) {
		return;
	}

	dev->ll = ll;

	reset_lcd(dev);

	/* Set color mode */
	write_command(dev, ST7789_COLMOD);
	write_data_byte(dev, ST7789_COLOR_MODE_RGB565);

	/* Porch control */
	write_command(dev, ST7789_PORCTRL);
	{
		uint8_t data[] = { 0x0C, 0x0C, 0x00, 0x33, 0x33 };
		write_data(dev, data, sizeof(data));
	}

	/* MADCTL (Display Rotation) */
	write_command(dev, ST7789_MADCTL);
	// ??? set_rotation(dev, width, heigh, rotation);

	/* Internal LCD Voltage generator settings */
	write_command(dev, 0xB7);		// Gate Control
	write_data_byte(dev, 0x35);		// Default value
	write_command(dev, 0xBB);		// VCOM setting
	write_data_byte(dev, 0x19);		// 0.725v (default 0.75v for 0x20)
	write_command(dev, 0xC0);		// LCMCTRL
	write_data_byte(dev, 0x2C);		// Default value
	write_command(dev, 0xC2);		// VDV and VRH command Enable
	write_data_byte(dev, 0x01);		// Default value
	write_command(dev, 0xC3);		// VRH set
	write_data_byte(dev, 0x12);		// +-4.45v (defalut +-4.1v for 0x0B)
	write_command(dev, 0xC4);		// VDV set
	write_data_byte(dev, 0x20);		// Default value
	write_command(dev, 0xC6);		// Frame rate control in normal mode
	write_data_byte(dev, 0x0F);		// Default value (60HZ)
	write_command(dev, 0xD0);		// Power control
	write_data_byte(dev, 0xA4);		// Default value
	write_data_byte(dev, 0xA1);		// Default value

	/* Division line */
	write_command(dev, 0xE0);
	{
		uint8_t data[] = { 0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23 };
		write_data(dev, data, sizeof(data));
	}

	write_command(dev, 0xE1);
	{
		uint8_t data[] = { 0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23 };
		write_data(dev, data, sizeof(data));
	}

	/* Inversion ON */
	write_command(dev, ST7789_INVON);

	/* Out of sleep mode */
	write_command(dev, ST7789_SLPOUT);

	/* Normal Display on */
  	write_command(dev, ST7789_NORON);

	/* Main screen turned on */
  	write_command(dev, ST7789_DISPON);

	dev->ll->delay_ms(50);

	/* Fill with Black */

}

void st7789_set_pixel(st7789_dev_t *dev, uint16_t x, uint16_t y, uint16_t color)
{
	return;
}
