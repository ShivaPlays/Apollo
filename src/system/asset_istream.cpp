#include "system/asset_istream.h"

#include <SDL3/SDL.h>

#ifdef SDL_PLATFORM_ANDROID

#include "utility/fixed_string.h"

namespace age
{
    asset_streambuf::asset_streambuf()
            : m_active_buffer{ m_internal_buffer.data() }
            , m_active_buffer_size{ m_internal_buffer.size() }
            , m_io{ nullptr }
    {}

    asset_streambuf::~asset_streambuf()
    {
        close();
    }

    bool asset_streambuf::open(const char* fn)
    {
        close();

        // Use SDL_IOFromFile for standard assets/files
        m_io = SDL_IOFromFile(fn, "rb");

        if (!m_io)
        {
            SDL_Log("Failed to open asset: %s, Error: %s", fn, SDL_GetError());
            return false;
        }

        setg(0, 0, 0);
        return true;
    }

    void asset_streambuf::close()
    {
        if (m_io)
        {
            SDL_CloseIO(m_io);
            m_io = nullptr;
        }
    }

    std::streambuf::int_type asset_streambuf::underflow()
    {
        if (!m_io)
            return traits_type::eof();

        if (gptr() < egptr())
            return traits_type::to_int_type(*gptr());

        // Read from SDL into the internal buffer
        size_t bytesRead = SDL_ReadIO(m_io, m_active_buffer, m_active_buffer_size);

        if (bytesRead == 0)
            return traits_type::eof();

        // Set the streambuf pointers: (beginning, current, end)
        setg(m_active_buffer, m_active_buffer, m_active_buffer + bytesRead);

        return traits_type::to_int_type(*gptr());
    }

    std::streambuf::pos_type asset_streambuf::seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which)
    {
        if (!m_io || !(which & std::ios_base::in)) return pos_type(off_type(-1));

        SDL_IOWhence whence;
        if (dir == std::ios_base::beg) whence = SDL_IO_SEEK_SET;
        else if (dir == std::ios_base::cur) whence = SDL_IO_SEEK_CUR;
        else whence = SDL_IO_SEEK_END;

        // If seeking relative to current, we must account for the buffered data
        if (dir == std::ios_base::cur) {
            off -= static_cast<off_type>(egptr() - gptr());
        }

        Sint64 result = SDL_SeekIO(m_io, off, whence);
        if (result < 0) return pos_type(off_type(-1));

        // Invalidate buffer because file position changed
        setg(0, 0, 0);

        return pos_type(result);
    }

    std::streambuf::pos_type asset_streambuf::seekpos(pos_type pos, std::ios_base::openmode which)
    {
        return seekoff(off_type(pos), std::ios_base::beg, which);
    }

    std::streambuf* asset_streambuf::setbuf(char_type* s, std::streamsize n)
    {
        if (s && n > 0)
        {
            m_active_buffer = s;
            m_active_buffer_size = static_cast<size_t>(n);
        }
        else
        {
            // Reset to internal if input is invalid
            m_active_buffer = m_internal_buffer.data();
            m_active_buffer_size = m_internal_buffer.size();
        }
        setg(0, 0, 0); // Force underflow to use the new buffer

        return this;
    }

    // --- asset_istream Implementation ---

    asset_istream::asset_istream()
            : std::istream(&m_streambuf)
    {}

    asset_istream::asset_istream(std::string_view fn, std::ios_base::openmode mode)
        : std::istream(&m_streambuf)
    {
        open(fn);
    }

    asset_istream::asset_istream(const char* fn, std::ios_base::openmode mode)
            : std::istream(&m_streambuf)
    {
        open(fn);
    }

    asset_istream::asset_istream(const std::string& fn, std::ios_base::openmode mode)
            : std::istream(&m_streambuf)
    {
        open(fn);
    }

    asset_istream::~asset_istream()
    {}

    void asset_istream::open(std::string_view fn)
    {
        constexpr size_t STRING_SIZE = 256;

        if(fn.size() > STRING_SIZE - 1)
        {
            open(std::string{fn}.c_str());
        }
        else
        {
            //We ensure that the string is \0 terminated
            fixed_string<STRING_SIZE> fn_str{fn};
            open(fn_str.data());
        }
    }

    void asset_istream::open(const char *fn)
    {
        if (!m_streambuf.open(fn))
        {
            setstate(std::ios::failbit);
        }
        else
        {
            clear(); // Clear error bits if successful
        }
    }

    void asset_istream::open(const std::string& fn)
    {
        open(fn.c_str());
    }

    bool asset_istream::is_open() const
    {
        return m_streambuf.is_open();
    }

    void asset_istream::close()
    {
        m_streambuf.close();
    }
}
#endif