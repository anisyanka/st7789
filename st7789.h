#ifndef __LCD_ST7789_H
#define __LCD_ST7789_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Colors in RGB565 format */
#define ST7789_WHITE_RGB565			(uint16_t)(0xFFFF)
#define ST7789_BLACK_RGB565			(uint16_t)(0x0000)
#define ST7789_BLUE_RGB565			(uint16_t)(0x001F)
#define ST7789_RED_RGB565			(uint16_t)(0xF800)
#define ST7789_MAGENTA_RGB565	 	(uint16_t)(0xF81F)
#define ST7789_GREEN_RGB565			(uint16_t)(0x07E0)
#define ST7789_CYAN_RGB565			(uint16_t)(0x7FFF)
#define ST7789_YELLOW_RGB565		(uint16_t)(0xFFE0)
#define ST7789_GRAY_RGB565			(uint16_t)(0x8430)
#define ST7789_BRED_RGB565			(uint16_t)(0xF81F)
#define ST7789_GRED_RGB565			(uint16_t)(0xFFE0)
#define ST7789_GBLUE_RGB565			(uint16_t)(0x07FF)
#define ST7789_BROWN_RGB565			(uint16_t)(0xBC40)
#define ST7789_BRRED_RGB565			(uint16_t)(0xFC07)
#define ST7789_DARKBLUE_RGB565		(uint16_t)(0x01CF)
#define ST7789_LIGHTBLUE_RGB565		(uint16_t)(0x7D7C)
#define ST7789_GRAYBLUE_RGB565		(uint16_t)(0x5458)
#define ST7789_LIGHTGREEN_RGB565	(uint16_t)(0x841F)
#define ST7789_LGRAY_RGB565			(uint16_t)(0xC618)
#define ST7789_LGRAYBLUE_RGB565		(uint16_t)(0xA651)
#define ST7789_LBBLUE_RGB565		(uint16_t)(0x2B12)

typedef enum
{
	ST7789_ROTATION_0,
	ST7789_ROTATION_1,
	ST7789_ROTATION_2,
	ST7789_ROTATION_3,
} st7789_rotation_t;

/* Low Level functions which must be implemented by user */
typedef struct
{
	void (*delay_ms)(uint32_t ms);

	/* reset LCD with hardware pin (optional) */
	void (*rst_pin_set)(void);
	void (*rst_pin_reset)(void);

	/* Manage chip select pin */
	void (*cs_pin_set)(void);
	void (*cs_pin_reset)(void);

	/*
	 * Manage D/C pin in case of 4-lines serial interface.
	 *
	 * Pin low -> data is treated as command
	 * Pin high -> data is treated as parameter or data
	 */
	void (*dc_pin_set)(void);
	void (*dc_pin_reset)(void);

	/* SPI data transfer */
	void (*spi_send_data8)(uint8_t *buf, size_t len);

	/* SPI data transfer in 16 bit frame mode */
	void (*spi_send_data16)(uint16_t *buf, size_t len);
} st7789_ll_t;

typedef struct
{
	uint16_t width;
	uint16_t heigh;
	uint16_t x_shift;
	uint16_t y_shift;
	uint8_t rotation;
	st7789_ll_t *ll;
} st7789_dev_t;

void st7789_init(st7789_dev_t *dev, st7789_ll_t *ll);
void st7789_fill_color(st7789_dev_t *dev, uint16_t color);
void st7789_set_pixel(st7789_dev_t *dev, uint16_t x, uint16_t y, uint16_t color);
void st7789_fill_area_with_raw_data(st7789_dev_t *dev,
									uint16_t x0, uint16_t y0,
									uint16_t x1, uint16_t y1,
									uint16_t *color_data, size_t len);
void st7789_set_rotation(st7789_dev_t *dev, st7789_rotation_t new_rot);

#ifdef __cplusplus
}
#endif

#endif  // __LCD_ST7789_H
