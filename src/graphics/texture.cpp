#include "graphics/texture.h"

#include <exception>
#include <sstream>
#include <cassert>

#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>

#include "graphics/render_states_cache.h"
#include "engine.h"
#include "utility/gl_check.h"

namespace age
{
	texture::texture(const texture& other)
		: m_handle{ gen_handle() }
	{

	}

	texture& texture::operator = (const texture& other)
	{
		return *this;
	}

	bool texture::bind() const
	{
		auto handle = get_handle();

		if (handle != g_render_state.texture)
		{
			GL_CALL(glBindTexture(GL_TEXTURE_2D, handle));

			const glm::u32vec2& size = get_size();

			glm::mat4 tex_matrix = glm::scale(glm::mat4{ 1.0f }, glm::vec3(1.0f / static_cast<float>(size.x), 1.0f / static_cast<float>(size.y), 1.0f));
			age::engine::get_instance()->get_texture_matrix_ubo().buffer_data(sizeof(glm::mat4), &tex_matrix);

			g_render_state.texture = handle;
		}

		return handle != 0;
	}

	void texture::create(const glm::u32vec2& size)
	{
		if (size.x == 0 || size.y == 0)
		{
			std::stringstream message;
			message << "Invalid texture size (" << size.x << ", " << size.y << ")";

			throw std::runtime_error{ message.str() };
		}

		uint32_t max_size = get_maximum_size();

		if (size.x > max_size || size.y > max_size)
		{
			std::stringstream message;
			message << "Internal size of texture is too high (" << size.x << ", " << size.y << ") maximum is: " << max_size;

			throw std::runtime_error{ message.str() };
		}

		if (!m_handle) m_handle = gen_handle();

		if (bind())
		{
			m_size = size;

			GL_CALL(glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGBA,
				static_cast<GLsizei>(m_size.x),
				static_cast<GLsizei>(m_size.y),
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				nullptr));

			GL_CALL(glTexParameteri(
				GL_TEXTURE_2D,
				GL_TEXTURE_WRAP_S,
				m_repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE));

			GL_CALL(glTexParameteri(
				GL_TEXTURE_2D,
				GL_TEXTURE_WRAP_T,
				m_repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE));

			GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_smooth ? GL_LINEAR : GL_NEAREST));
			GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_smooth ? GL_LINEAR : GL_NEAREST));
		}

		m_has_mipmap = false;
	}

	void texture::load(std::string_view filename, const int_rect& area)
	{
		image img;
		img.load(filename);
		load(img, area);
	}

	void texture::load(const std::byte data[], std::size_t size, const int_rect& area)
	{
		image img;
		img.load(data, size);
		load(img, area);
	}

	void texture::load(std::istream& is, const int_rect& area)
	{
		image img;
		img.load(is);
		load(img, area);
	}

	void texture::load(const image& img, const int_rect& area)
	{
		int32_t width = static_cast<int32_t>(img.get_size().x);
		int32_t height = static_cast<int32_t>(img.get_size().y);

		if (area.width == 0 || area.height == 0 ||
			(area.left <= 0 && area.top <= 0 && area.width >= width && area.height >= height))
		{
			create(img.get_size());
			update(img);

			return;
		}

		int_rect rectangle = area;
		if(rectangle.left < 0)
			rectangle.left = 0;
		if (rectangle.top < 0)
			rectangle.top = 0;
		if (rectangle.left + rectangle.width > width)
			rectangle.width = width - rectangle.left;
		if (rectangle.top + rectangle.height > height)
			rectangle.height = height - rectangle.top;

		create(glm::u32vec2{ static_cast<uint32_t>(rectangle.width), static_cast<uint32_t>(rectangle.height) });

		auto pixels = img.get_pixel_ptr() + 4 * (rectangle.left + (width * rectangle.top));
		if (bind())
		{
			for (int32_t i = 0; i < rectangle.height; ++i)
			{
				GL_CALL(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, i, rectangle.width, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixels));
				pixels += 4 * width;
			}

			GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_smooth ? GL_LINEAR : GL_NEAREST));
			m_has_mipmap = false;

			//If there should be multithreading one day a glFlush will be necessary
			//glFlush();
		}
	}

	void texture::update(const uint8_t* pixels)
	{
		update(pixels, uint_rect{ glm::u32vec2{}, get_size() });
	}

	void texture::update(const uint8_t* pixels, const uint_rect& area)
	{
		assert(area.left + area.width <= m_size.x);
		assert(area.top + area.height <= m_size.y);

		if (pixels)
		{
			if (bind())
			{
				GL_CALL(glTexSubImage2D(GL_TEXTURE_2D,
					0,
					static_cast<GLint>(area.left),
					static_cast<GLint>(area.top),
					static_cast<GLsizei>(area.width),
					static_cast<GLsizei>(area.height),
					GL_RGBA,
					GL_UNSIGNED_BYTE,
					pixels));

				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_smooth ? GL_LINEAR : GL_NEAREST));
				m_has_mipmap = false;
			}

			//If there should be multithreading one day a glFlush will be necessary
			//glFlush();
		}
	}

	void texture::update(const texture& other_texture)
	{
		update(other_texture, glm::u32vec2{});
	}

	void texture::update(const texture& other_texture, const glm::u32vec2& dest)
	{
		assert(dest.x + other_texture.m_size.x <= m_size.x);
		assert(dest.y + other_texture.m_size.y <= m_size.y);

		update(other_texture.copy_to_image(), dest);
	}

	void texture::update(const image& img)
	{
		// Update the whole texture
		update(img.get_pixel_ptr(), uint_rect{ glm::u32vec2{}, img.get_size() });
	}

	void texture::update(const image& img, const glm::u32vec2& dest)
	{
		update(img.get_pixel_ptr(), uint_rect{ dest, img.get_size() });
	}

	void texture::update(const render_window& window)
	{
		update(window, { 0, 0 });
	}

	void texture::update(const render_window& window, const glm::u32vec2& dest)
	{
		if (bind())
		{
			//ToDo: Finish me
		}
	}

	const glm::u32vec2& texture::get_size() const
	{
		return m_size;
	}

	image texture::copy_to_image() const
	{
		image result{};

		GLuint framebuffer;
		GL_CALL(glGenFramebuffers(1, &framebuffer));

		if (framebuffer)
		{
			std::vector<std::uint8_t> pixels(static_cast<std::size_t>(m_size.x) * static_cast<std::size_t>(m_size.y) * 4);

			GLint previous_frame_buffer;
			GL_CALL(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previous_frame_buffer));

			GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));
			GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, get_handle(), 0));
			GL_CALL(glReadPixels(0, 0, m_size.x, m_size.y, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]));

			// This puts a marker in the GPU command stream right after the ReadPixels command.
			GLsync sync_fence = GL_CALL(glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0));

			if (sync_fence)
			{
				// GL_SYNC_FLUSH_COMMANDS_BIT ensures the GPU starts working on the queue immediately.
				// We set a timeout of 1 second (1,000,000,000 nanoseconds).
				GLenum wait_status = GL_CALL(glClientWaitSync(sync_fence, GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000));

				if (wait_status == GL_WAIT_FAILED)
				{
					// Handle error: The GPU or driver crashed
				}

				GL_CALL(glDeleteSync(sync_fence));
			}

			GL_CALL(glDeleteFramebuffers(1, &framebuffer));
			GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, previous_frame_buffer));

			result.create(m_size, pixels.data());
		}

		return result;
	}

	void texture::set_smooth(bool value)
	{
		if (value != m_smooth)
		{
			m_smooth = value;

			if (bind())
			{
				GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_smooth ? GL_LINEAR : GL_NEAREST));
				if (m_has_mipmap)
				{
					GL_CALL(glTexParameteri(GL_TEXTURE_2D,
						GL_TEXTURE_MIN_FILTER,
						m_smooth ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR));
				}
				else
				{
					GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_smooth ? GL_LINEAR : GL_NEAREST));
				}
			}
		}
	}

	bool texture::get_smooth() const
	{
		return m_smooth;
	}

	void texture::set_srgb(bool value)
	{
		m_srgb = value;
	}

	bool texture::get_srgb() const
	{
		return m_srgb;
	}

	void texture::set_repeat(bool value)
	{
		m_repeat = value;

		if (bind())
		{
			GL_CALL(glTexParameteri(GL_TEXTURE_2D,
				GL_TEXTURE_WRAP_S,
				m_repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE));

			GL_CALL(glTexParameteri(GL_TEXTURE_2D,
				GL_TEXTURE_WRAP_T,
				m_repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE));
		}
	}

	bool texture::get_repeat() const
	{
		return m_repeat;
	}

	void texture::generate_mipmap()
	{
		if (bind())
		{
			GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));

			GL_CALL(glTexParameteri(GL_TEXTURE_2D,
				GL_TEXTURE_MIN_FILTER,
				m_smooth ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR));
		}

		m_has_mipmap = true;
	}

	void texture::invalidate_mipmap()
	{
		if (bind())
		{
			GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_smooth ? GL_LINEAR : GL_NEAREST));
		}

		m_has_mipmap = false;
	}

	uint32_t texture::get_id() const
	{
		return get_handle();
	}

	void texture::invalidate()
	{
		delete_handle(get_handle());

		m_handle = 0;
		m_size = glm::uvec2{ 1, 1 };
	}

	bool texture::bind(const texture* tex)
	{
		if (tex)
		{
			return tex->bind();
		}

		if (g_render_state.texture != 0)
		{
			GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
			g_render_state.texture = 0;
		}

		return g_render_state.texture != 0;
	}

	uint32_t texture::get_maximum_size()
	{
		static bool checked = false;
		static uint32_t result = 0;

		if (!checked)
		{
			checked = true;

			GLint size;
			GL_CALL(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size));

			result = size;
		}

		return result;
	}

	void texture::reset_cache()
	{
		g_render_state.texture = 0;
	}

	uint32_t texture::gen_handle()
	{
		GLuint handle;
		GL_CALL(glGenTextures(1, &handle));

		return handle;
	}

	void texture::delete_handle(uint32_t handle)
	{
		if (handle && !engine::is_device_reset()) GL_CALL(glDeleteTextures(1, &handle));
		if (handle == g_render_state.texture) g_render_state.texture = 0;
	}
}