#include "system/assetstream.h"

#include <SDL3/SDL.h>

#if defined(ANDROID) || defined(__ANDROID__)
namespace age
{
    asset_streambuf::asset_streambuf()
            : m_io(nullptr)
    {
        // Reserve buffer memory but don't fill it until underflow()
        m_buffer.resize(BUFFER_SIZE);
    }

    asset_streambuf::~asset_streambuf()
    {
        close();
    }

    bool asset_streambuf::open(std::string_view fn)
    {
        close();

        // Use SDL_IOFromFile for standard assets/files
        // .data() converts the string_view to a null-terminated const char*
        m_io = SDL_IOFromFile(fn.data(), "rb");

        if (!m_io)
        {
            SDL_Log("Failed to open asset: %s, Error: %s", fn.data(), SDL_GetError());
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
        if (!m_io || gptr() < egptr())
            return (m_io && gptr() < egptr()) ? traits_type::to_int_type(*gptr()) : traits_type::eof();

        // Read from SDL into the internal buffer
        size_t bytesRead = SDL_ReadIO(m_io, m_buffer.data(), m_buffer.size());

        if (bytesRead == 0)
            return traits_type::eof();

        // Set the streambuf pointers: (beginning, current, end)
        setg(m_buffer.data(), m_buffer.data(), m_buffer.data() + bytesRead);

        return traits_type::to_int_type(*gptr());
    }

    std::streambuf::pos_type asset_streambuf::seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which)
    {
        if (!m_io) return pos_type(off_type(-1));

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

    // --- assetistream Implementation ---

    assetistream::assetistream()
            : std::istream(&m_streambuf)
    {}

    assetistream::assetistream(std::string_view fn, std::ios_base::openmode mode)
        : std::istream(&m_streambuf)
    {
        open(fn);
    }

    assetistream::~assetistream()
    {}

    void assetistream::open(std::string_view fn)
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

    bool assetistream::is_open() const
    {
        return m_streambuf.is_open();
    }

    void assetistream::close()
    {
        m_streambuf.close();
    }
}
#endif