/**
 * @file RenderBuffer.h
 *
 * @author
 *
 * This file is a part of Awesomium, a library that makes it easy for 
 * developers to embed the web in their applications.
 *
 * Website: <http://www.awesomium.com>
 * 
 * Copyright (C) 2011 Khrona. All rights reserved. Awesomium is a 
 * trademark of Khrona.
 */

#ifndef __AWESOMIUM_RENDER_BUFFER_H__
#define __AWESOMIUM_RENDER_BUFFER_H__

#include <Awesomium/PlatformUtils.h>
#include <string>

namespace Awesomium {
	
struct Rect;
	
/**
 * The RenderBuffer class represents a 32-bit pixel buffer. The pixel format
 * is 4 BPP (bytes per pixel) with BGRA (Blue, Green, Red, Alpha) byte ordering.
 */
class _OSMExport RenderBuffer
{
public:
	
	/// The raw block of pixel data, BGRA format
	unsigned char* buffer;
	
	/// The width, in pixels
	int width;
	
	/// The height, in pixels
	int height;
	
	/// The number of bytes per row (is usually width * 4 but can be different)
	int rowSpan;
	
	/// Whether or not 'buffer' will be de-allocated upon destruction.
	bool ownsBuffer;
	
	/**
	 * Create a new RenderBuffer with a specified width and height.
	 */
	RenderBuffer(int width, 
				 int height);
	
	/**
	 * Create a new RenderBuffer to wrap around a specified pixel buffer.
	 */
	RenderBuffer(unsigned char* buffer, 
				 int width, 
				 int height, 
				 int rowSpan, 
				 bool autoDeleteBuffer = false);
	
	/**
	 * Destroy the RenderBuffer
	 */
	~RenderBuffer();
	
	/**
	 * Copy this buffer to a specified destination buffer.
	 * 
	 * @param	destBuffer	The destination buffer (should have same dimensions)
	 *
	 * @param	destDepth	The depth (either 3 BPP or 4 BPP)
	 *
	 * @param	convertToRGBA	Whether or not to convert to RGBA format.
	 *
	 * @param	flipY		Whether or not we should flip the image vertically.
	 */
	void copyTo(unsigned char* destBuffer, 
				int destRowSpan, 
				int destDepth, 
				bool convertToRGBA,
				bool flipY) const;
	
	/**
	 * Save this buffer to a PNG image.
	 *
	 * @param	filePath	The path to the file that will be written.
	 *
	 * @param	preserveTransparency	Whether or not transparency (alpha)
	 *									values should be preserved.
	 *
	 * @return	Whether or not the image was successfully saved.
	 */
	bool saveToPNG(const std::wstring& filePath, 
				   bool preserveTransparency = false) const;
	
	/**
	 * Save this buffer to a JPEG image.
	 *
	 * @param	filePath	The path to the file that will be written.
	 *
	 * @param	quality		The compression quality to use, the valid range is
	 *                      0 to 100, with 100 being the highest.
	 *
	 * @return	Whether or not the image was successfully saved.
	 */
	bool saveToJPEG(const std::wstring& filePath,
					int quality = 90) const;
	
	/**
	 * Get the alpha value at a certain point (origin is top-left). This is
	 * useful for alpha-picking.
	 *
	 * @param	x	The x-value of the point.
	 * @param	y	The y-value of the point.
	 *
	 * @return	Returns the alpha value at a certain point (255 is comppletely
	 *			opaque, 0 is completely transparent).
	 */
	unsigned char getAlphaAtPoint(int x, int y) const;
	
	/**
	 * Re-allocate the pixel buffer to be of the specified dimensions
	 */
	void reserve(int width, 
				 int height);
	
	/**
	 * Copy from a specified source buffer (assumed to have the same dimensions)
	 */
	void copyFrom(unsigned char* srcBuffer, 
				  int srcRowSpan);
	
	/** 
	 * Copy from a specified source buffer with certain dimensions
	 */
	void copyArea(unsigned char* srcBuffer, 
				  int srcRowSpan, 
				  const Rect& srcRect, 
				  bool forceOpaque = false);
	
	/**
	 * Copy from a specified source buffer with certain dimensions to a
	 * specific offset in our buffer.
	 */
	void copyArea(const RenderBuffer& srcBuffer, 
				  const Rect& srcRect, 
				  const Rect& destRect);
	
	/**
	 * Scroll the specified area by a relative amount of pixels.
	 */
	void scrollArea(int dx, 
					int dy, 
					Rect clipRect);

	/**
	 * Sets the alpha channel to completely opaque values.
	 */
	void flushAlpha();
};
	
/**
 * A simple rectangle class. Used with WebView::getDirtyBounds and various
 * RenderBuffer methods.
 */
struct _OSMExport Rect {
	
	/// The x-coordinate of the origin of the rectangle
	int x;

	/// The y-coordinate of the origin of the rectangle
	int y;

	/// The width of the rectangle
	int width;

	/// The height of the rectangle
	int height;
	
	/// Create an empty Rect
	Rect();
	
	/// Create a Rect with certain dimensions
	Rect(int x, int y, int width, int height);
	
	/// Check whether or not this Rect is empty
	bool isEmpty() const;
};

/**
 * Useful method to copy portions of two pixel buffers. Used internally
 * by the RenderBuffer class.
 */
void _OSMExport copyBuffers(int width, 
							int height, 
							unsigned char* src, 
							int srcRowSpan, 
							unsigned char* dest, 
							int destRowSpan, 
							int destDepth, 
							bool convertToRGBA,
							bool flipY);

}

#endif 