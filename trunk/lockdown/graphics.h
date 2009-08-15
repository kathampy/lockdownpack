#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <psptypes.h>

#define	PSP_LINE_SIZE 512
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272

typedef u32 Color;
#define A(color) ((u8)(color >> 24 & 0xFF))
#define B(color) ((u8)(color >> 16 & 0xFF))
#define G(color) ((u8)(color >> 8 & 0xFF))
#define R(color) ((u8)(color & 0xFF))
#define RGB(r, g, b) ((r)|((g)<<8)|((b)<<16))

typedef struct
{
	int textureWidth;  // the real width of data, 2^n with n>=0
	int textureHeight;  // the real height of data, 2^n with n>=0
	int imageWidth;  // the image width
	int imageHeight;
	Color* data;
} Image;

/**
 * Load a PNG image.
 *
 * @pre filename != NULL
 * @param filename - filename of the PNG image to load
 * @return pointer to a new allocated Image struct, or NULL on failure
 */
extern Image* loadImage(const char* filename);

/**
 * Load a PNG image from in-memory data.
 *
 * @pre data != NULL && len >0
 * @param data - the in-memory bytes of the image
 * @param len - the number of valid bytes at the data pointer
 * @return pointer to a new allocated Image struct, or NULL on failure
 */
extern Image* loadImageFromMemory(const unsigned char* data, int len);

/**
 * Blit a rectangle part of an image to screen.
 *
 * @pre source != NULL && destination != NULL &&
 *      sx >= 0 && sy >= 0 &&
 *      width > 0 && height > 0 &&
 *      sx + width <= source->width && sy + height <= source->height &&
 *      dx + width <= SCREEN_WIDTH && dy + height <= SCREEN_HEIGHT
 * @param sx - left position of rectangle in source image
 * @param sy - top position of rectangle in source image
 * @param width - width of rectangle in source image
 * @param height - height of rectangle in source image
 * @param source - pointer to Image struct of the source image
 * @param dx - left target position in destination image
 * @param dy - top target position in destination image
 */
extern void blitImageToScreen(int sx, int sy, int width, int height, Image* source, int dx, int dy);

/**
 * Blit a rectangle part of an image to screen without alpha pixels in source image.
 *
 * @pre source != NULL && destination != NULL &&
 *      sx >= 0 && sy >= 0 &&
 *      width > 0 && height > 0 &&
 *      sx + width <= source->width && sy + height <= source->height &&
 *      dx + width <= SCREEN_WIDTH && dy + height <= SCREEN_HEIGHT
 * @param sx - left position of rectangle in source image
 * @param sy - top position of rectangle in source image
 * @param width - width of rectangle in source image
 * @param height - height of rectangle in source image
 * @param source - pointer to Image struct of the source image
 * @param dx - left target position in destination image
 * @param dy - top target position in destination image
 */
extern void blitAlphaImageToScreen(int sx, int sy, int width, int height, Image* source, int dx, int dy);

/**
 * Frees an allocated image.
 *
 * @pre image != null
 * @param image a pointer to an image struct
 */
extern void freeImage(Image* image);

/**
 * Initialize all pixels of the screen with a color.
 *
 * @param color - new color for the pixels
 */
extern void clearScreen(Color color);

/**
 * Print a text (pixels out of the screen or image are clipped).
 *
 * @param x - left position of text
 * @param y - top position of text
 * @param text - the text to print
 * @param color - new color for the pixels
 */
extern void printTextScreen(int x, int y, const char* text, u32 color);

/**
 * Exchange display buffer and drawing buffer.
 */
extern void flipScreen();

/**
 * Initialize the graphics.
 */
extern void initGraphics();

/**
 * Disable graphics, used for debug text output.
 */
extern void disableGraphics();

/**
 * Get the current draw buffer for fast unchecked access.
 *
 * @return the start address of the current draw buffer
 */
extern Color* getVramDrawBuffer();

/**
 * Get the current display buffer for fast unchecked access.
 *
 * @return the start address of the current display buffer
 */
extern Color* getVramDisplayBuffer();

extern void guStart();

#endif
