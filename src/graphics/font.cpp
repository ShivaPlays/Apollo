#include "graphics/font.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_BITMAP_H
#include FT_STROKER_H

#include <exception>
#include <sstream>
#include <limits>
#include <codecvt>

//Only temporary as long as there is no proper errorhandling
#include <iostream>

#include "graphics/image.h"
#include "graphics/rect.h"
#include "system/asset_istream.h"
#include "utility/utility.h"

//Freetype callbacks
unsigned long read(FT_Stream rec, unsigned long offset, unsigned char* buffer, unsigned long count)
{
	auto converted_offset = static_cast<int64_t>(offset);
	auto stream = static_cast<std::istream*>(rec->descriptor.pointer);

	if (stream->seekg(converted_offset).tellg() == converted_offset)
	{
		if (count > 0)
		{
			return static_cast<unsigned long>(stream->read(reinterpret_cast<char*>(buffer), count).gcount());
		}

		return 0;
	}

	return count > 0 ? 0 : 1;
}

void close(FT_Stream)
{

}

inline uint64_t combine(float outline_thickness, bool bold, std::uint32_t index)
{
	uint32_t* u_outline_thickness = reinterpret_cast<uint32_t*>(&outline_thickness);

	return (static_cast<std::uint64_t>(*u_outline_thickness) << 32) |
		(static_cast<std::uint64_t>(bold) << 31) | index;
}

namespace age
{
	class font::font_handles
	{
	private:
		struct deleter
		{
			void operator()(FT_Library theLibrary)
			{
				FT_Done_FreeType(theLibrary);
			}
			void operator()(FT_Face theFace)
			{
				FT_Done_Face(theFace);
			}
			void operator()(FT_Stroker theStroker)
			{
				FT_Stroker_Done(theStroker);
			}
		};

	public:
		std::unique_ptr<std::remove_pointer_t<FT_Library>, deleter> library;	//< Pointer to the internal library interface
		std::unique_ptr<FT_StreamRec>                               stream_rec;	//< Pointer to the stream rec instance
		std::unique_ptr<std::remove_pointer_t<FT_Face>, deleter>    face;		//< Pointer to the internal font face
		std::unique_ptr<std::remove_pointer_t<FT_Stroker>, deleter> stroker;	//< Pointer to the stroker
	};

	class glyph_handle
	{
	public:
		glyph_handle() = default;
		glyph_handle(const glyph_handle& copy) = delete;
		glyph_handle(glyph_handle&& other) = delete;

		glyph_handle& operator = (const glyph_handle& other) = delete;
		glyph_handle& operator = (glyph_handle&& other) = delete;

		inline ~glyph_handle()
		{
			FT_Done_Glyph(m_glyph);
		}

	public:
		inline FT_Glyph& get_glyph() { return m_glyph; }
		inline const FT_Glyph& get_glyph() const { return m_glyph; };

		inline operator FT_Glyph& () { return m_glyph; }
		inline operator const FT_Glyph& () const { return m_glyph; }

	protected:

	private:
		
		FT_Glyph m_glyph;
	};

	font::font()
		: m_font_handles{}
	{}

	font::font(font&& other) noexcept = default;
	font::~font() = default;

	font& font::operator = (font&& other) noexcept = default;

	void font::load(std::string_view fn)
	{
		load(std::make_unique<asset_istream>(fn.data(), std::ios::binary));
	}

	void font::load(const std::byte data[], size_t size_in_bytes)
	{
		m_stream.reset();
		cleanup();

		auto handles = std::make_unique<font_handles>();

		FT_Library library;
		if (FT_Init_FreeType(&library) != 0)
		{
			std::stringstream ss;
			ss << "Failed to load font from memory(failed to initialize FreeType)";

			throw std::runtime_error{ ss.str() };
		}
		handles->library.reset(library);

		FT_Face face;
		if (FT_New_Memory_Face(library, reinterpret_cast<const FT_Byte*>(data), static_cast<FT_Long>(size_in_bytes), 0, &face) != 0)
		{
			std::stringstream ss;
			ss << "Failed to load font from memory (failed to create the font face)";

			throw std::runtime_error{ ss.str() };
		}
		handles->face.reset(face);

		FT_Stroker stroker;
		if (FT_Stroker_New(library, &stroker) != 0)
		{
			std::stringstream ss;
			ss << "Failed to load font from memory (failed to create the stroker)";

			throw std::runtime_error{ ss.str() };
		}
		handles->stroker.reset(stroker);

		if (FT_Select_Charmap(face, FT_ENCODING_UNICODE) != 0)
		{
			std::stringstream ss;
			ss << "Failed to load font from memory (failed to set the Unicode character set)";

			throw std::runtime_error{ ss.str() };
		}

		m_font_handles = std::move(handles);
		m_info.family = face->family_name ? face->family_name : std::string{};
	}

	void font::load(std::unique_ptr<std::istream> in_stream)
	{
		m_stream = std::move(in_stream);
		load_from_stream(*m_stream);
	}

	void font::load(std::istream& in_stream)
	{
		m_stream.reset();
		load_from_stream(in_stream);
	}

	void font::load_from_stream(std::istream& in_stream)
	{
		cleanup();

		auto handles = std::make_unique<font_handles>();

		FT_Library library;
		if (FT_Init_FreeType(&library) != 0)
		{
			std::stringstream ss;
			ss << "Failed to load font (failed to initialize FreeType)";

			throw std::runtime_error{ ss.str() };
		}
		handles->library.reset(library);

		auto cur_stream_pos = in_stream.tellg();

		//Lets try if an undefined 0x7FFFFFFF streamsize will do. If not give it the real size
		/*
		in_stream.ignore(std::numeric_limits<std::streamsize>::max());
		auto size = in_stream.gcount() - cur_stream_pos;
		in_stream.clear();
		in_stream.seekg(cur_stream_pos);
		*/
		
		handles->stream_rec = std::make_unique<FT_StreamRec>();
		handles->stream_rec->base = nullptr;
		//handles->stream_rec->size = size;
		handles->stream_rec->size = 0x7FFFFFFF;
		handles->stream_rec->pos = static_cast<decltype(handles->stream_rec->pos)>(cur_stream_pos);
		handles->stream_rec->descriptor.pointer = &in_stream;
		handles->stream_rec->read = &read;
		handles->stream_rec->close = &close;

		FT_Open_Args args;
		args.flags = FT_OPEN_STREAM;
		args.stream = handles->stream_rec.get();
		args.driver = nullptr;

		FT_Face face;
		if (FT_Open_Face(library, &args, 0, &face) != 0)
		{
			std::stringstream ss;
			ss << "Failed to load font (failed to create the font face)";

			throw std::runtime_error{ ss.str() };
		}
		handles->face.reset(face);

		FT_Stroker stroker;
		if (FT_Stroker_New(library, &stroker) != 0)
		{
			std::stringstream ss;
			ss << "Failed to load font (failed to create the stroker)";

			throw std::runtime_error{ ss.str() };
		}
		handles->stroker.reset(stroker);

		if (FT_Select_Charmap(face, FT_ENCODING_UNICODE) != 0)
		{
			std::stringstream ss;
			ss << "Failed to load font (failed to set the Unicode character set)";

			throw std::runtime_error{ ss.str() };
		}

		m_font_handles = std::move(handles);

		m_info.family = face->family_name ? face->family_name : std::string();
	}

	const font::info& font::get_info() const
	{
		return m_info;
	}

	const font::glyph& font::get_glyph(uint32_t code_point, uint32_t character_size, bool bold, float outline_thickness) const
	{
		auto& glyphs = load_page(character_size).glyphs;
		auto key = combine(outline_thickness, bold, code_point);

		if (auto it = glyphs.find(key); it != glyphs.end())
			return it->second;

		glyph new_glyph = load_glyph(code_point, character_size, bold, outline_thickness);
		return glyphs.emplace(key, new_glyph).first->second;
	}

	bool font::has_glyph(uint32_t code_point) const
	{
		return FT_Get_Char_Index(m_font_handles ? m_font_handles->face.get() : nullptr, code_point) != 0;
	}

	float font::get_kerning(uint32_t first, uint32_t second, uint32_t character_size, bool bold) const
	{
		// Special case where first or second is 0 (null character)
		if (first == 0 || second == 0)
			return 0.0f;

		auto face = m_font_handles ? m_font_handles->face.get() : nullptr;

		if (face)
		{
			try
			{
				set_current_size(character_size);
			}
			catch (...)
			{
				//set_current_size failed
				return 0.0f;
			}
			
			// Convert the characters to indices
			FT_UInt index1 = FT_Get_Char_Index(face, first);
			FT_UInt index2 = FT_Get_Char_Index(face, second);

			// Retrieve position compensation deltas generated by FT_LOAD_FORCE_AUTOHINT flag
			auto firstRsbDelta = static_cast<float>(get_glyph(first, character_size, bold).rsb_delta);
			auto secondLsbDelta = static_cast<float>(get_glyph(second, character_size, bold).lsb_delta);

			// Get the kerning vector if present
			FT_Vector kerning;
			kerning.x = kerning.y = 0;
			if (FT_HAS_KERNING(face))
				FT_Get_Kerning(face, index1, index2, FT_KERNING_UNFITTED, &kerning);

			// X advance is already in pixels for bitmap fonts
			if (!FT_IS_SCALABLE(face))
				return static_cast<float>(kerning.x);

			// Combine kerning with compensation deltas and return the X advance
			// Flooring is required as we use FT_KERNING_UNFITTED flag which is not quantized in 64 based grid
			return std::floor(
				(secondLsbDelta - firstRsbDelta + static_cast<float>(kerning.x) + 32) / static_cast<float>(1 << 6));
		}
		else
		{
			// Invalid font
			return 0.0f;
		}
	}

	float font::get_line_spacing(uint32_t character_size) const
	{
		auto face = m_font_handles ? m_font_handles->face.get() : nullptr;

		if (face)
		{
			try
			{
				set_current_size(character_size);
			}
			catch (...)
			{
				return 0.0f;
			}

			return static_cast<float>(face->size->metrics.height) / static_cast<float>(1 << 6);
		}
		else
		{
			return 0.0f;
		}
	}

	float font::get_underline_position(uint32_t character_size) const
	{
		auto face = m_font_handles ? m_font_handles->face.get() : nullptr;

		if (face)
		{
			try
			{
				set_current_size(character_size);
			}
			catch (...)
			{
				return 0.0f;
			}

			// Return a fixed position if font is a bitmap font
			if (!FT_IS_SCALABLE(face))
				return static_cast<float>(character_size) / 10.0f;

			return -static_cast<float>(FT_MulFix(face->underline_position, face->size->metrics.y_scale)) /
				static_cast<float>(1 << 6);
		}
		else
		{
			return 0.0f;
		}
	}

	float font::get_underline_thickness(uint32_t character_size) const
	{
		auto face = m_font_handles ? m_font_handles->face.get() : nullptr;

		if (face)
		{
			try
			{
				set_current_size(character_size);
			}
			catch (...)
			{
				return 0.0f;
			}

			// Return a fixed thickness if font is a bitmap font
			if (!FT_IS_SCALABLE(face))
				return static_cast<float>(character_size) / 14.0f;

			return static_cast<float>(FT_MulFix(face->underline_thickness, face->size->metrics.y_scale)) /
				static_cast<float>(1 << 6);
		}
		else
		{
			return 0.0f;
		}
	}

	const texture& font::get_texture(uint32_t character_size) const
	{
		return load_page(character_size).page_texture;
	}

	void font::set_smooth(bool value)
	{
		if (value != m_smooth)
		{
			for (auto& [key, page] : m_pages)
			{
				page.page_texture.set_smooth(value);
			}

			m_smooth = value;
		}
	}

	void font::pre_cache_glyphs(const std::u32string_view& letters, uint32_t character_size, bool bold, float outline_thickness)
	{
		for (auto& u32_c : letters)
			get_glyph(u32_c, character_size, bold, outline_thickness);
	}

	bool font::get_smooth() const
	{
		return m_smooth;
	}

	font::page& font::load_page(uint32_t character_size) const
	{
		return m_pages.try_emplace(character_size, m_smooth).first->second;
	}

	font::glyph font::load_glyph(uint32_t code_point, uint32_t character_size, bool bold, float outline_thickness) const
	{
		glyph result;

		if (!m_font_handles)
			return result;

		auto face = m_font_handles->face.get();
		if (!face)
			return result;

		set_current_size(character_size);

		// Load the glyph corresponding to the code point
		FT_Int32 flags = FT_LOAD_TARGET_NORMAL | FT_LOAD_FORCE_AUTOHINT;
		if (outline_thickness != 0)
			flags |= FT_LOAD_NO_BITMAP;

		if (FT_Load_Char(face, code_point, flags) != 0)
			return result;

		glyph_handle glyph_desc{};
		if (FT_Get_Glyph(face->glyph, &glyph_desc.get_glyph()) != 0)
			return result;

		FT_Pos weight = 1 << 6;

		bool outline = glyph_desc.get_glyph()->format == FT_GLYPH_FORMAT_OUTLINE;
		if (outline)
		{
			if (bold)
			{
				auto outline_glyph = reinterpret_cast<FT_OutlineGlyph>(glyph_desc.get_glyph());
				FT_Outline_Embolden(&outline_glyph->outline, weight);

				if (outline_thickness != 0.0f)
				{
					auto stroker = m_font_handles->stroker.get();

					FT_Stroker_Set(stroker, 
						static_cast<FT_Fixed>(outline_thickness * static_cast<float>(1 << 6)), 
						FT_STROKER_LINECAP_ROUND,
						FT_STROKER_LINEJOIN_ROUND,
						0);

					FT_Glyph_Stroke(&glyph_desc.get_glyph(), stroker, true);
				}
			}
		}

		FT_Glyph_To_Bitmap(&glyph_desc.get_glyph(), FT_RENDER_MODE_NORMAL, nullptr, 1);
		auto bitmap_glyph = reinterpret_cast<FT_BitmapGlyph>(glyph_desc.get_glyph());
		FT_Bitmap& bitmap = bitmap_glyph->bitmap;

		if (!outline)
		{
			if (bold)
				FT_Bitmap_Embolden(m_font_handles->library.get(), &bitmap, weight, weight);

			if (outline_thickness != 0)
			{
				//ToDo: Add proper error-handling here
				std::cout << "Failed to outline glyph(no fallback available)" << std::endl;
			}
		}

		result.advance = static_cast<float>(bitmap_glyph->root.advance.x >> 16);
		if (bold)
		{
			result.advance += static_cast<float>(weight) / static_cast<float>(1 << 6);
		}

		result.lsb_delta = static_cast<int>(face->glyph->lsb_delta);
		result.rsb_delta = static_cast<int>(face->glyph->rsb_delta);

		glm::u32vec2 size{bitmap.width, bitmap.rows};

		if ((size.x > 0) && (size.y > 0))
		{
			const uint32_t padding = 2;

			size.x += 2 * padding;
			size.y += 2 * padding;

			page& cur_page = load_page(character_size);

			result.texture_rect = find_glyph_rect(cur_page, size);

			// Make sure the texture data is positioned in the center
			// of the allocated texture rectangle
			result.texture_rect.left += static_cast<int>(padding);
			result.texture_rect.top += static_cast<int>(padding);
			result.texture_rect.width -= static_cast<int>(2 * padding);
			result.texture_rect.height -= static_cast<int>(2 * padding);

			// Compute the glyph's bounding box
			result.bounds.left = static_cast<float>(bitmap_glyph->left);
			result.bounds.top = static_cast<float>(-bitmap_glyph->top);
			result.bounds.width = static_cast<float>(bitmap.width);
			result.bounds.height = static_cast<float>(bitmap.rows);

			// Resize the pixel buffer to the new size and fill it with transparent white pixels
			m_pixel_buffer.resize(static_cast<std::size_t>(size.x) * static_cast<std::size_t>(size.y) * 4);

			uint8_t* current = m_pixel_buffer.data();
			uint8_t* end = current + size.x * size.y * 4;

			while (current != end)
			{
				(*current++) = 255;
				(*current++) = 255;
				(*current++) = 255;
				(*current++) = 0;
			}

			// Extract the glyph's pixels from the bitmap
			const std::uint8_t* pixels = bitmap.buffer;
			if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
			{
				// Pixels are 1 bit monochrome values
				for (uint32_t y = padding; y < size.y - padding; ++y)
				{
					for (uint32_t x = padding; x < size.x - padding; ++x)
					{
						// The color channels remain white, just fill the alpha channel
						std::size_t index = x + y * size.x;
						m_pixel_buffer[index * 4 + 3] = ((pixels[(x - padding) / 8]) & (1 << (7 - ((x - padding) % 8)))) ? 255 : 0;
					}
					pixels += bitmap.pitch;
				}
			}
			else
			{
				// Pixels are 8 bits gray levels
				for (uint32_t y = padding; y < size.y - padding; ++y)
				{
					for (uint32_t x = padding; x < size.x - padding; ++x)
					{
						// The color channels remain white, just fill the alpha channel
						std::size_t index = x + y * size.x;
						m_pixel_buffer[index * 4 + 3] = pixels[x - padding];
					}
					pixels += bitmap.pitch;
				}
			}

			// Write the pixels to the texture
			uint32_t x = static_cast<unsigned int>(result.texture_rect.left) - padding;
			uint32_t y = static_cast<unsigned int>(result.texture_rect.top) - padding;
			uint32_t w = static_cast<unsigned int>(result.texture_rect.width) + 2 * padding;
			uint32_t h = static_cast<unsigned int>(result.texture_rect.height) + 2 * padding;
			cur_page.page_texture.update(m_pixel_buffer.data(), uint_rect{ glm::u32vec2{ x ,y }, glm::u32vec2{w, h} });
		}

		// Delete the FT glyph is done by glyph_handle
		//FT_Done_Glyph(glyph_desc);

		return result;
	}

	int_rect font::find_glyph_rect(page& page, const glm::u32vec2& size) const
	{
		row* matching_row = nullptr;
		float best_ratio = 0.0f;

		int_rect result{ glm::i32vec2{ 0, 0 }, glm::i32vec2{ 2, 2 } };

		for (auto& r : page.rows)
		{
			float ratio = static_cast<float>(size.y) / static_cast<float>(r.height);

			// Ignore rows that are either too small or too high
			if ((ratio < 0.7f) || (ratio > 1.f))
				continue;

			// Check if there's enough horizontal space left in the row
			if (size.x > page.page_texture.get_size().x - r.width)
				continue;

			// Make sure that this new row is the best found so far
			if (ratio < best_ratio)
				continue;

			matching_row = &r;
			best_ratio = ratio;
		}

		if (!matching_row)
		{
			uint32_t row_height = size.y + size.y / 10;
			auto texture_size = page.page_texture.get_size();

			bool texture_needs_resizing = false;

			while ((page.next_row + row_height >= texture_size.y) || (size.x >= texture_size.x))
			{
				// Not enough space: resize the texture if possible
				// Make the texture 2 times bigger
				texture_size.x *= 2u;
				texture_size.y *= 2u;
				texture_needs_resizing = true;
				uint32_t max_tex_size = texture::get_maximum_size();

				if ((texture_size.x > max_tex_size) || (texture_size.y > max_tex_size))
				{
					std::cout << "Failed to add a new character to the font: the maximum texture size has been reached" << std::endl;
					return result;
				}
			}

			if (texture_needs_resizing)
			{
				texture new_texture;
				try
				{
					new_texture.create(glm::uvec2{ texture_size.x, texture_size.y });
				}
				catch (const std::exception& e)
				{
					std::cout << "Failed to create new page texture: " << e.what() << std::endl;
					return result;
				}

				new_texture.set_smooth(m_smooth);
				new_texture.update(page.page_texture);
				/*
				* Move assigment should do actually
				page.texture.swap(newTexture);
				*/
				page.page_texture = std::move(new_texture);
			}

			// We can now create the new row
			page.rows.emplace_back(page.next_row, row_height);
			page.next_row += row_height;
			matching_row = &page.rows.back();
		}

		result.left = matching_row->width;
		result.top = matching_row->top;
		result.width = size.x;
		result.height = size.y;
	
		// Update the row informations
		matching_row->width += size.x;

		return result;
	}

	void font::set_current_size(uint32_t character_size) const
	{
		auto face = m_font_handles->face.get();
		FT_UShort current_size = face->size->metrics.x_ppem;

		if (current_size != character_size)
		{
			FT_Error result = FT_Set_Pixel_Sizes(face, 0, character_size);

			if (result == FT_Err_Invalid_Pixel_Size)
			{
				if (!FT_IS_SCALABLE(face))
				{
					std::stringstream ss;

					ss << "Failed to set bitmap font size to " << character_size << '\n';
					ss << "Availabel size are: ";

					for (int i = 0; i < face->num_fixed_sizes; ++i)
					{
						const long size = (face->available_sizes[i].y_ppem + 32) >> 6;
						ss << size << " ";
					}

					throw std::runtime_error{ ss.str() };
				}
				else
				{
					std::stringstream ss;

					ss << "Failed to set font size to " << character_size;

					throw std::runtime_error{ ss.str() };
				}
			}

			if (result != FT_Err_Ok)
			{
				std::stringstream ss;

				ss << "Failed to set font size to " << character_size;

				throw std::runtime_error{ ss.str() };
			}
		}
	}

	void font::cleanup()
	{
		m_font_handles.reset();
		m_pages.clear();
		m_pixel_buffer.clear();
		m_pixel_buffer.shrink_to_fit();
	}

	font::page::page(bool smooth)
		: next_row{ 3 }
	{
		image tex_image;
		tex_image.create(glm::u32vec2{ 128, 128 }, color{ 255, 255, 255, 0 });

		for (uint32_t x = 0; x < 2; ++x)
			for (uint32_t y = 0; y < 2; ++y)
				tex_image.set_pixel(glm::u32vec2{ x, y }, color{ 255, 255, 255, 255 });

		page_texture.load(tex_image);
		page_texture.set_smooth(smooth);
	}
}