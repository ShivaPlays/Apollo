#include "graphics/image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <iterator>
#include <stdexcept>
#include <sstream>
#include <memory>
#include <algorithm>

#include "system/asset_istream.h"
#include "utility/utility.h"

namespace
{
	// stb_image callbacks that operate on a agl::assetistream
	int read(void* user, char* data, int size)
	{
		auto* stream = static_cast<std::istream*>(user);
		stream->read(data, size);

		auto bytes_read = stream->gcount();

		return static_cast<int>(bytes_read);
	}

	void skip(void* user, int size)
	{
		auto* stream = static_cast<std::istream*>(user);

		stream->seekg(stream->tellg() + std::streampos{ size });
	}

	int eof(void* user)
	{
		auto* stream = static_cast<std::istream*>(user);
		auto val = stream->peek();

		return val == EOF;
	}

	// stb_image callback for constructing a buffer
	void buffer_from_callback(void* context, void* data, int size)
	{
		auto* source = static_cast<std::uint8_t*>(data);
		auto* dest = static_cast<std::vector<std::uint8_t>*>(context);
		std::copy(source, source + size, std::back_inserter(*dest));
	}

	void load_image_from_stream(std::istream& stream, std::vector<uint8_t>& pixels, glm::u32vec2& size)
	{
		pixels.clear();

		// Setup the stb_image callbacks
		stbi_io_callbacks callbacks;

		callbacks.read = &read;
		callbacks.skip = &skip;
		callbacks.eof = &eof;

		// Load the image and get a pointer to the pixels in memory
		int            width = 0;
		int            height = 0;
		int            channels = 0;
		unsigned char* tmp_ptr = stbi_load_from_callbacks(&callbacks, &stream, &width, &height, &channels, STBI_rgb_alpha);

		if (tmp_ptr)
		{
			//unique_ptr takes care of freeing memory
			std::unique_ptr<unsigned char, age::deleter<unsigned char, stbi_image_free>> ptr{ tmp_ptr };

			// Assign the image properties
			size.x = static_cast<unsigned int>(width);
			size.y = static_cast<unsigned int>(height);

			if (width && height)
			{
				// Copy the loaded pixels to the pixel buffer
				pixels.resize(static_cast<std::size_t>(width * height * 4));
				memcpy(pixels.data(), ptr.get(), pixels.size());
			}
		}
		else
		{
			// Error, failed to load the image
			std::stringstream ss;
			ss << "Failed to load image from stream. Reason: " << stbi_failure_reason();

			throw std::runtime_error{ ss.str() };
		}
	}

	void load_image_from_memory(const void* data, std::size_t data_size, std::vector<std::uint8_t>& pixels, glm::u32vec2& size)
	{
		// Check input parameters
		if (data && data_size)
		{
			// Clear the array (just in case)
			pixels.clear();

			// Load the image and get a pointer to the pixels in memory
			int         width = 0;
			int         height = 0;
			int         channels = 0;
			const auto* buffer = static_cast<const unsigned char*>(data);
			unsigned char* tmp_ptr = stbi_load_from_memory(buffer, static_cast<int>(data_size), &width, &height, &channels, STBI_rgb_alpha);

			if (tmp_ptr)
			{
				//unique_ptr takes care of freeing memory
				std::unique_ptr<unsigned char, age::deleter<unsigned char, stbi_image_free>> ptr{ tmp_ptr };

				// Assign the image properties
				size.x = static_cast<unsigned int>(width);
				size.y = static_cast<unsigned int>(height);

				if (width > 0 && height > 0)
				{
					// Copy the loaded pixels to the pixel buffer
					pixels.resize(static_cast<std::size_t>(width * height * 4));
					memcpy(pixels.data(), ptr.get(), pixels.size());
				}
			}
			else
			{
				std::stringstream ss;
				ss << "Failed to load image from memory. Reason: " << stbi_failure_reason();

				throw std::runtime_error{ ss.str() };
			}
		}
		else
		{
			std::stringstream ss;
			ss << "Failed to load image from memory, no data provided";

			throw std::runtime_error{ ss.str() };
		}
	}
} // namespace


namespace age
{
	void image::create(const glm::u32vec2& size, const color& the_color)
	{
		if (size.x && size.y)
		{
			// Create a new pixel buffer first for exception safety's sake
			std::vector<std::uint8_t> newPixels(static_cast<std::size_t>(size.x) * static_cast<std::size_t>(size.y) * 4);

			// Fill it with the specified color
			std::uint8_t* ptr = newPixels.data();
			std::uint8_t* end = ptr + newPixels.size();
			while (ptr < end)
			{
				*ptr++ = the_color.r;
				*ptr++ = the_color.g;
				*ptr++ = the_color.b;
				*ptr++ = the_color.a;
			}

			// Commit the new pixel buffer
			m_pixels.swap(newPixels);

			// Assign the new size
			m_size = size;
		}
		else
		{
			// Dump the pixel buffer
			//std::vector<std::uint8_t>().swap(m_pixels);
			m_pixels.clear();
			m_pixels.shrink_to_fit();

			// Assign the new size
			m_size.x = 0;
			m_size.y = 0;
		}
	}

	void image::create(const glm::u32vec2& size, const uint8_t* pixels)
	{
		if (pixels && size.x && size.y)
		{
			// Create a new pixel buffer first for exception safety's sake
			std::vector<std::uint8_t> newPixels(pixels, pixels + size.x * size.y * 4);

			// Commit the new pixel buffer
			m_pixels.swap(newPixels);

			// Assign the new size
			m_size = size;
		}
		else
		{
			// Dump the pixel buffer
			//std::vector<std::uint8_t>().swap(m_pixels);
			m_pixels.clear();
			m_pixels.shrink_to_fit();

			// Assign the new size
			m_size.x = 0;
			m_size.y = 0;
		}
	}

	void image::load(std::string_view fn)
	{
		asset_istream is{ fn.data(), std::ios::binary };
		load(is);
	}

	void image::load(const std::byte data[], size_t size)
	{
		load_image_from_memory(data, size, m_pixels, m_size);
	}

	void image::load(std::istream& is)
	{
		load_image_from_stream(is, m_pixels, m_size);
	}

	void image::save(const std::string_view& value)
	{
		auto errfunc = [&value]() {
			std::stringstream ss;

			ss << "Error saving image. " << value;
			throw std::runtime_error{ ss.str() };
		};

		auto pos = value.find_last_of('.');
		if (pos == value.npos)
			errfunc();

		if (value.substr(pos) == ".bmp")
		{
			// BMP format
			if (!stbi_write_bmp(value.data(), static_cast<int>(m_size.x), static_cast<int>(m_size.y), 4, m_pixels.data()))
				errfunc();

			return;
		}

		if (value.substr(pos) == ".tga")
		{
			// TGA format
			if (!stbi_write_tga(value.data(), static_cast<int>(m_size.x), static_cast<int>(m_size.y), 4, m_pixels.data()))
				errfunc();

			return;
		}

		if (value.substr(pos) == ".png")
		{
			// PNG format
			if (!stbi_write_png(value.data(), static_cast<int>(m_size.x), static_cast<int>(m_size.y), 4, m_pixels.data(), 0))
				errfunc();

			return;
		}

		if (value.substr(pos) == ".jpg" || value.substr(pos) == ".jpeg")
		{
			// JPG format
			if (!stbi_write_jpg(value.data(), static_cast<int>(m_size.x), static_cast<int>(m_size.y), 4, m_pixels.data(), 90))
				errfunc();

			return;
		}

		errfunc();
	}

	void image::save(std::vector<uint8_t>& data, const std::string_view& format)
	{

	}

	void image::create_mask_from_color(const color& color_key, uint8_t alpha)
	{
		// Make sure that the image is not empty
		if (!m_pixels.empty())
		{
			// Replace the alpha of the pixels that match the transparent color
			std::uint8_t* ptr = m_pixels.data();
			std::uint8_t* end = ptr + m_pixels.size();
			while (ptr < end)
			{
				if ((ptr[0] == color_key.r) && (ptr[1] == color_key.g) && (ptr[2] == color_key.b) && (ptr[3] == color_key.a))
					ptr[3] = alpha;
				ptr += 4;
			}
		}
	}

	void image::set_pixel(const glm::u32vec2& coords, const color& pixel_color)
	{
		std::uint8_t* pixel = &m_pixels[(coords.x + coords.y * m_size.x) * 4];
		*pixel++ = pixel_color.r;
		*pixel++ = pixel_color.g;
		*pixel++ = pixel_color.b;
		*pixel++ = pixel_color.a;
	}

	const glm::u32vec2& image::get_size() const
	{
		return m_size;
	}

	color image::get_pixel(const glm::u32vec2& coords) const
	{
		const std::uint8_t* pixel = &m_pixels[(coords.x + coords.y * m_size.x) * 4];
		return color(pixel[0], pixel[1], pixel[2], pixel[3]);
	}

	const uint8_t* image::get_pixel_ptr() const
	{
		return m_pixels.data();
	}

	void image::flip_horizontal()
	{
		if (!m_pixels.empty())
		{
			std::size_t rowSize = m_size.x * 4;

			for (std::size_t y = 0; y < m_size.y; ++y)
			{
				auto left = m_pixels.begin() + static_cast<std::vector<std::uint8_t>::iterator::difference_type>(y * rowSize);
				auto right = m_pixels.begin() +
					static_cast<std::vector<std::uint8_t>::iterator::difference_type>((y + 1) * rowSize - 4);

				for (std::size_t x = 0; x < m_size.x / 2; ++x)
				{
					std::swap_ranges(left, left + 4, right);

					left += 4;
					right -= 4;
				}
			}
		}
	}

	void image::flip_vertical()
	{
		if (!m_pixels.empty())
		{
			auto rowSize = static_cast<std::vector<std::uint8_t>::iterator::difference_type>(m_size.x * 4);

			auto top = m_pixels.begin();
			auto bottom = m_pixels.end() - rowSize;

			for (std::size_t y = 0; y < m_size.y / 2; ++y)
			{
				std::swap_ranges(top, top + rowSize, bottom);

				top += rowSize;
				bottom -= rowSize;
			}
		}
	}

	std::istream& operator >> (std::istream& in, image& im)
	{
		im.load(in);
		return in;
	}
}