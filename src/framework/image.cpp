#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "GL/glew.h"
#include "../extra/picopng.h"
#include "image.h"
#include "utils.h"
#include "camera.h"
#include "mesh.h"


Image::Image() {
	width = 0; height = 0;
	pixels = NULL;
}

Image::Image(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new Color[width*height];
	memset(pixels, 0, width * height * sizeof(Color));
}

// Copy constructor
Image::Image(const Image& c)
{
	pixels = NULL;
	width = c.width;
	height = c.height;
	bytes_per_pixel = c.bytes_per_pixel;
	if(c.pixels)
	{
		pixels = new Color[width*height];
		memcpy(pixels, c.pixels, width*height*bytes_per_pixel);
	}
}

// Assign operator
Image& Image::operator = (const Image& c)
{
	if(pixels) delete[] pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	bytes_per_pixel = c.bytes_per_pixel;

	if(c.pixels)
	{
		pixels = new Color[width*height*bytes_per_pixel];
		memcpy(pixels, c.pixels, width*height*bytes_per_pixel);
	}
	return *this;
}

Image::~Image()
{
	if(pixels) 
		delete[] pixels;
}

void Image::Render()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glDrawPixels(width, height, bytes_per_pixel == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

// Change image size (the old one will remain in the top-left corner)
void Image::Resize(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for(unsigned int x = 0; x < min_width; ++x)
		for(unsigned int y = 0; y < min_height; ++y)
			new_pixels[ y * width + x ] = GetPixel(x,y);

	delete[] pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

// Change image size and scale the content
void Image::Scale(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];

	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++y)
			new_pixels[ y * width + x ] = GetPixel((unsigned int)(this->width * (x / (float)width)), (unsigned int)(this->height * (y / (float)height)) );

	delete[] pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

Image Image::GetArea(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height)
{
	Image result(width, height);
	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++y)
		{
			if( (x + start_x) < this->width && (y + start_y) < this->height) 
				result.SetPixelUnsafe( x, y, GetPixel(x + start_x,y + start_y) );
		}
	return result;
}

void Image::FlipY()
{
	int row_size = bytes_per_pixel * width;
	Uint8* temp_row = new Uint8[row_size];
#pragma omp simd
	for (int y = 0; y < height * 0.5; y += 1)
	{
		Uint8* pos = (Uint8*)pixels + y * row_size;
		memcpy(temp_row, pos, row_size);
		Uint8* pos2 = (Uint8*)pixels + (height - y - 1) * row_size;
		memcpy(pos, pos2, row_size);
		memcpy(pos2, temp_row, row_size);
	}
	delete[] temp_row;
}

bool Image::LoadPNG(const char* filename, bool flip_y)
{
	std::string sfullPath = absResPath(filename);
	std::ifstream file(sfullPath, std::ios::in | std::ios::binary | std::ios::ate);

	// Get filesize
	std::streamsize size = 0;
	if (file.seekg(0, std::ios::end).good()) size = file.tellg();
	if (file.seekg(0, std::ios::beg).good()) size -= file.tellg();

	if (!size){
		std::cerr << "--- Failed to load file: " << sfullPath.c_str() << std::endl;
		return false;
	}

	std::vector<unsigned char> buffer;

	// Read contents of the file into the vector
	if (size > 0)
	{
		buffer.resize((size_t)size);
		file.read((char*)(&buffer[0]), size);
	}
	else
		buffer.clear();

	std::vector<unsigned char> out_image;

	if (decodePNG(out_image, width, height, buffer.empty() ? 0 : &buffer[0], (unsigned long)buffer.size(), true) != 0){
		std::cerr << "--- Failed to load file: " << sfullPath.c_str() << std::endl;
		return false;
	}

	size_t bufferSize = out_image.size();
	unsigned int originalBytesPerPixel = (unsigned int)bufferSize / (width * height);
	
	// Force 3 channels
	bytes_per_pixel = 3;

	if (originalBytesPerPixel == 3) {
		if (pixels) delete[] pixels;
		pixels = new Color[bufferSize];
		memcpy(pixels, &out_image[0], bufferSize);
	}
	else if (originalBytesPerPixel == 4) {
		if (pixels) delete[] pixels;

		unsigned int newBufferSize = width * height * bytes_per_pixel;
		pixels = new Color[newBufferSize];

		unsigned int k = 0;
		for (unsigned int i = 0; i < bufferSize; i += originalBytesPerPixel) {
			pixels[k] = Color(out_image[i], out_image[i + 1], out_image[i + 2]);
			k++;
		}
	}

	// Flip pixels in Y
	if (flip_y)
		FlipY();

	std::cout << "+++ File loaded: " << sfullPath.c_str() << std::endl;

	return true;
}

// Loads an image from a TGA file
bool Image::LoadTGA(const char* filename, bool flip_y)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char TGAcompare[12];
	unsigned char header[6];
	unsigned int imageSize;
	unsigned int bytesPerPixel;

    std::string sfullPath = absResPath( filename );

	FILE * file = fopen( sfullPath.c_str(), "rb");
   	if ( file == NULL || fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare) ||
		memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0 ||
		fread(header, 1, sizeof(header), file) != sizeof(header))
	{
		std::cerr << "--- File not found: " << sfullPath.c_str() << std::endl;
		if (file == NULL)
			return NULL;
		else
		{
			fclose(file);
			return NULL;
		}
	}

	TGAInfo* tgainfo = new TGAInfo;
    
	tgainfo->width = header[1] * 256 + header[0];
	tgainfo->height = header[3] * 256 + header[2];
    
	if (tgainfo->width <= 0 || tgainfo->height <= 0 || (header[4] != 24 && header[4] != 32))
	{
		std::cerr << "--- Failed to load file: " << sfullPath.c_str() << std::endl;
		fclose(file);
		delete tgainfo;
		return NULL;
	}
    
	tgainfo->bpp = header[4];
	bytesPerPixel = tgainfo->bpp / 8;
	imageSize = tgainfo->width * tgainfo->height * bytesPerPixel;
    
	tgainfo->data = new unsigned char[imageSize];
    
	if (tgainfo->data == NULL || fread(tgainfo->data, 1, imageSize, file) != imageSize)
	{
		std::cerr << "--- Failed to load file: " << sfullPath.c_str() << std::endl;

		if (tgainfo->data != NULL)
			delete[] tgainfo->data;
            
		fclose(file);
		delete tgainfo;
		return false;
	}

	fclose(file);

	// Save info in image
	if(pixels)
		delete[] pixels;

	width = tgainfo->width;
	height = tgainfo->height;
	pixels = new Color[width*height];

	// Convert to float all pixels
	for (unsigned int y = 0; y < height; ++y) {
		for (unsigned int x = 0; x < width; ++x) {
			unsigned int pos = y * width * bytesPerPixel + x * bytesPerPixel;
			// Make sure we don't access out of memory
			if( (pos < imageSize) && (pos + 1 < imageSize) && (pos + 2 < imageSize))
				SetPixelUnsafe(x, height - y - 1, Color(tgainfo->data[pos + 2], tgainfo->data[pos + 1], tgainfo->data[pos]));
		}
	}

	// Flip pixels in Y
	if (flip_y)
		FlipY();

	delete[] tgainfo->data;
	delete tgainfo;

	std::cout << "+++ File loaded: " << sfullPath.c_str() << std::endl;

	return true;
}

// Saves the image to a TGA file
bool Image::SaveTGA(const char* filename)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	std::string fullPath = absResPath(filename);
	FILE *file = fopen(fullPath.c_str(), "wb");
	if ( file == NULL )
	{
		std::cerr << "--- Failed to save file: " << fullPath.c_str() << std::endl;
		return false;
	}

	unsigned short header_short[3];
	header_short[0] = width;
	header_short[1] = height;
	unsigned char* header = (unsigned char*)header_short;
	header[4] = 24;
	header[5] = 0;

	fwrite(TGAheader, 1, sizeof(TGAheader), file);
	fwrite(header, 1, 6, file);

	// Convert pixels to unsigned char
	unsigned char* bytes = new unsigned char[width*height*3];
	for(unsigned int y = 0; y < height; ++y)
		for(unsigned int x = 0; x < width; ++x)
		{
			Color c = pixels[y*width+x];
			unsigned int pos = (y*width+x)*3;
			bytes[pos+2] = c.r;
			bytes[pos+1] = c.g;
			bytes[pos] = c.b;
		}

	fwrite(bytes, 1, width*height*3, file);
	fclose(file);

	delete[] bytes;

	std::cout << "+++ File saved: " << fullPath.c_str() << std::endl;

	return true;
}

void Image::DrawRect(int x, int y, int w, int h, const Color& borderColor,
	int borderWidth, bool isFilled, const Color& fillColor) {
	
	if (borderWidth > w || borderWidth > h) borderWidth = std::min(w, h);

	if (isFilled) {
		for (unsigned int i = 0; i < w; ++i) {
			for (unsigned int j = 0; j < h; ++j) {
				if (i < borderWidth || i >= (w - borderWidth) || j < borderWidth || j >= (h - borderWidth)) {
					SetPixel(x + i, y + j, borderColor); // If x or y value is in border range
				}
				else {
					SetPixel(x + i, y + j, fillColor);
				}
			}
		}
	}
	else { // We have some repeated code, but this way we only check the "isFilled" once and not for every pixel
		for (unsigned int i = 0; i < w; ++i) {
			for (unsigned int j = 0; j < h; ++j) {
				if (i < borderWidth || i >= (w - borderWidth) || j < borderWidth || j >= (h - borderWidth)) {
					SetPixel(x + i, y + j, borderColor);
				}
			}
		}
	}
};

void Image::DrawLineDDA(int x0, int y0, int x1, int y1, const Color& c) {
	int dx = x1 - x0;
	int dy = y1 - y0;

	int d = std::max(abs(dx), abs(dy)); // Choose what distance we'll travel

	if (d == 0) { // Check extreme case
		return;
	}

	Vector2 v(float(dx) / d, float(dy) / d); // Define step increase

	float x = x0;
	float y = y0;

	for (int i = 0; i < d; ++i) {
		SetPixel(x, y, c);
		x += v.x;
		y += v.y;
	}
}

void Image::ScanLineDDA(int x0, int y0, int x1, int y1, std::vector<Cell>& table) {
	// Extremely similar to regular DDA, but instead of SetPixel
	// we update the table's cells min and max x

	int dx = x1 - x0;
	int dy = y1 - y0;

	int d = std::max(abs(dx), abs(dy));

	if (d == 0) {
		return;
	}

	Vector2 v(float(dx) / d, float(dy) / d);

	float x = x0 + 0.5f;
	float y = y0 + 0.5f;

	for (int i = 0; i < d; ++i) {
		int ix = int(std::floor(x));
		int iy = int(std::floor(y));
		if (iy >= 0 && iy < table.size()) {
			table[iy].minx = std::min(table[iy].minx, ix);	// If x value is less than cell's min, update
			table[iy].maxx = std::max(table[iy].maxx, ix);	// Same for max
		}

		x += v.x;
		y += v.y;
	}
}


void Image::DrawTriangle(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Color& borderColor, bool isFilled, const Color& fillColor) {
	std::vector<Cell> table;
	table.resize(height);
	
	ScanLineDDA(p0.x, p0.y, p1.x, p1.y, table); // Update table's cells values with line p0-p1
	ScanLineDDA(p1.x, p1.y, p2.x, p2.y, table); // For line p1-p2
	ScanLineDDA(p2.x, p2.y, p0.x, p0.y, table); // For line p2-p0


	// Useful for triangles that have their base parallel to screen rows.
	// If not included, the base would not get a border printed.

	int min_y = std::min(std::min(p0.y, p1.y), p2.y); 


	for (int i = 0; i < height; ++i) {
		for (int j = table[i].minx; j < table[i].maxx; ++j) {
			if (j <= table[i].minx || j >= table[i].maxx - 1 || i == min_y) {
				SetPixel(j, i, borderColor);
			}
			else if (isFilled) {
				SetPixel(j, i, fillColor);
			}
		}
	}
}

// void Image::DrawTriangleInterpolated(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Color& c0, const Color& c1, const Color& c2, FloatImage* zBuffer) {
/*void Image::DrawTriangleInterpolated(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Color& c0, const Color& c1, const Color& c2, FloatImage* zBuffer, Image* texture, const Vector2& uv0, const Vector2& uv1, const Vector2& uv2) {
	std::vector<Cell> table;
	table.resize(height);	

	ScanLineDDA(p0.x, p0.y, p1.x, p1.y, table); // Update table's cells values with line p0-p1
	ScanLineDDA(p1.x, p1.y, p2.x, p2.y, table); // For line p1-p2
	ScanLineDDA(p2.x, p2.y, p0.x, p0.y, table); // For line p2-p0

	float area = (p1 - p0).Cross(p2 - p0).z;
	if (fabs(area) == 1e-4f) return;

	for (int y = 0; y < height; ++y) {

		int startx = std::max(0, table[y].minx); // Extremely important in order to not charsh when we zoom
		int endx = std::min((int)width, table[y].maxx);


		for (int x = startx; x < endx; ++x) {
			Vector3 p(x + 0.5f, y + 0.5f, 0); // Need to calculate zeta depth
			

			float alpha = (p1 - p).Cross(p2 - p).z / area;
			float beta = (p2 - p).Cross(p0 - p).z / area;
			float gamma = 1.0f - alpha - beta;

			float sum = alpha + beta + gamma;
			alpha /= sum;
			beta /= sum;
			gamma /= sum;

			if (alpha < 0 || beta < 0 || gamma < 0) continue;

			Vector2 text_coordinate = 
				Vector2(alpha * uv0.x, alpha * uv0.y) +
				Vector2(beta * uv1.x, beta * uv1.y) +
				Vector2(gamma * uv2.x, gamma * uv2.y);

			text_coordinate.x *= (texture->width - 1);
			text_coordinate.y *= (texture->height - 1);

			float final_z = alpha * p0.z + beta * p1.z + gamma * p2.z;

			if (final_z < zBuffer->GetPixel(x, y)) {
				zBuffer->SetPixel(x, y, final_z);

				// Color final_color = alpha * c0 + beta * c1 + gamma * c2;
				Color final_color = texture->GetPixel(text_coordinate.x, text_coordinate.y);

				SetPixel(x, y, final_color);
			}
		}
	}
}*/

void Image::DrawTriangleInterpolated(
	const sTriangleInfo& t, FloatImage* zBuffer,
	bool use_texture, bool use_occlusion, bool use_interpolation
) {
	std::vector<Cell> table;
	table.resize(height);

	ScanLineDDA(t.p0.x, t.p0.y, t.p1.x, t.p1.y, table); // Update table's cells values with line p0-p1
	ScanLineDDA(t.p1.x, t.p1.y, t.p2.x, t.p2.y, table); // For line p1-p2
	ScanLineDDA(t.p2.x, t.p2.y, t.p0.x, t.p0.y, table); // For line p2-p0

	float area = (t.p1 - t.p0).Cross(t.p2 - t.p0).z;
	if (area == 0) return;

	for (int y = 0; y < height; ++y) {

		int startx = std::max(0, table[y].minx-1); // Extremely important in order to not charsh when we zoom
		int endx = std::min((int)width, table[y].maxx+1);


		for (int x = startx; x < endx; ++x) {
			Vector3 p(x + 0.5f, y + 0.5f, 0); // Need to calculate zeta depth


			float alpha = (t.p1 - p).Cross(t.p2 - p).z / area;
			float beta = (t.p2 - p).Cross(t.p0 - p).z / area;
			float gamma = 1.0f - alpha - beta;

			float sum = alpha + beta + gamma;
			alpha /= sum;
			beta /= sum;
			gamma /= sum;

			if (alpha < 0 || beta < 0 || gamma < 0) continue;

			Vector2 text_coordinate;

			float final_z = alpha * t.p0.z + beta * t.p1.z + gamma * t.p2.z;
			Color final_color;

			

			if (use_interpolation) { // INTERPOLATED

				if (use_texture) { // INTERPOLATED TEXTURE
					text_coordinate =
						Vector2(alpha * t.uv0.x, alpha * t.uv0.y) +
						Vector2(beta * t.uv1.x, beta * t.uv1.y) +
						Vector2(gamma * t.uv2.x, gamma * t.uv2.y);

					text_coordinate.x *= (t.texture->width - 1);
					text_coordinate.y *= (t.texture->height - 1);

					final_color = t.texture->GetPixel(text_coordinate.x, text_coordinate.y);
				}
				else { // INTERPOLATED COLORS
					final_color = alpha * t.c0 + beta * t.c1 + gamma * t.c2;
				}
			}
			else { // NO-INTER

				if (use_texture) { // NO-INTER TEXTURE
					final_color = t.texture->GetPixel(t.uv0.x * (t.texture->width - 1), t.uv0.y * (t.texture->height - 1));
				}
				else { // NO-INTER COLOR
					final_color = t.c0 ;
				}
			}

			if (use_occlusion) {
				if (final_z < zBuffer->GetPixel(x, y)) {
					zBuffer->SetPixel(x, y, final_z);
					SetPixel(x, y, final_color);
				}
			} else{
				SetPixel(x, y, final_color);
			}
			
		}
	}
}



// Used for both drawing button images and loaded bit map images
void Image::DrawImage(const FloatImage& image, int x, int y) {
	for (int i = x; i < x + image.width; ++i) {
		for (int j = y; j < y + image.height; ++j) {
			float z = image.GetPixel(i - x, j - y);
			SetPixel(i, j, Color(255 * z, 255 * z, 255 * z));
			// SetPixel(i, j, image.GetPixel(i - x, j - y)); // Print in curr image the pixel at (i-x, j-y) of given image
		}
	}
}



#ifndef IGNORE_LAMBDAS

// You can apply and algorithm for two images and store the result in the first one
// ForEachPixel( img, img2, [](Color a, Color b) { return a + b; } );
template <typename F>
void ForEachPixel(Image& img, const Image& img2, F f) {
	for(unsigned int pos = 0; pos < img.width * img.height; ++pos)
		img.pixels[pos] = f( img.pixels[pos], img2.pixels[pos] );
}

#endif

FloatImage::FloatImage(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new float[width * height];
	memset(pixels, 0, width * height * sizeof(float));
}

// Copy constructor
FloatImage::FloatImage(const FloatImage& c) {
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width * height];
		memcpy(pixels, c.pixels, width * height * sizeof(float));
	}
}

// Assign operator
FloatImage& FloatImage::operator = (const FloatImage& c)
{
	if (pixels) delete[] pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width * height * sizeof(float)];
		memcpy(pixels, c.pixels, width * height * sizeof(float));
	}
	return *this;
}

FloatImage::~FloatImage()
{
	if (pixels)
		delete[] pixels;
}

// Change image size (the old one will remain in the top-left corner)
void FloatImage::Resize(unsigned int width, unsigned int height)
{
	float* new_pixels = new float[width * height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for (unsigned int x = 0; x < min_width; ++x)
		for (unsigned int y = 0; y < min_height; ++y)
			new_pixels[y * width + x] = GetPixel(x, y);

	delete[] pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}
