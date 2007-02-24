#include "pngwriter.h"
#include "exception.h"

using namespace std;

PngWriter::PngWriter(const string& outputFile)
: m_FilePtr(NULL)
, m_PngPtr(NULL)
, m_InfoPtr(NULL)
{
	m_FilePtr = fopen(outputFile.c_str(), "wb");
	
	if (!m_FilePtr)
    {
       throw Exception(string("Failed to open output file: ") + outputFile);
    }
    
    m_PngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!m_PngPtr)
	{
		throw Exception("Failed to create png write structure");
	}
	
	m_InfoPtr = png_create_info_struct(m_PngPtr);
	if (!m_InfoPtr)
    {
		png_destroy_write_struct(&m_PngPtr, (png_infopp) NULL);
		throw Exception("Failed to create png info structure");
	}
}

PngWriter::~PngWriter()
{
	fclose(m_FilePtr);
	png_destroy_write_struct(&m_PngPtr, &m_InfoPtr);
}

void PngWriter::setPngText(const string& key, const string& value)
{
	png_text pngText;
		
	pngText.compression = -1;
	pngText.key = const_cast<char*>(key.c_str());
	pngText.text = const_cast<char*>(value.c_str());

	png_set_text(m_PngPtr, m_InfoPtr, &pngText, 1);	
}

void PngWriter::writeFrame(png_byte** rgbData, int width, int height)
{
	png_init_io(m_PngPtr, m_FilePtr);
    if (setjmp(png_jmpbuf(m_PngPtr)))
	{
		throw Exception("Writing png file failed");
	}
		
	png_set_IHDR(m_PngPtr, m_InfoPtr, width, height, 8,
				 PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
				 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
				 
	png_set_rows(m_PngPtr, m_InfoPtr, rgbData);
	png_write_png(m_PngPtr, m_InfoPtr, 0, NULL);	
}

void PngWriter::writeRowCallback(png_structp, png_uint_32, int)
{
	// not interested
}
