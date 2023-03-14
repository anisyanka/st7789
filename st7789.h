#ifndef __LCD_ST7789_H
#define __LCD_ST7789_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

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
	void (*spi_send_data)(uint8_t *buf, size_t len);
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

void st7789_init(st7789_dev_t *dev, st7789_ll_t *ll,
				 uint16_t width, uint16_t heigh, uint32_t rotation);
void st7789_fill_color(st7789_dev_t *dev, uint16_t color);
void st7789_set_pixel(st7789_dev_t *dev, uint16_t x, uint16_t y, uint16_t color);

#ifdef __cplusplus
}
#endif

#endif  // __LCD_ST7789_H
