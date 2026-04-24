#pragma once

#include <SDL3/SDL.h>
#ifdef SDL_PLATFORM_ANDROID
#include <streambuf>
#include <istream>
#include <string_view>
#include <array>

#include "../utility/utility.h"
namespace age
{
    class asset_streambuf : public std::streambuf
    {
    public:
        explicit asset_streambuf();
        virtual ~asset_streambuf() override;

        bool open(const char* fn);
        void close();

        // Check if the file is currently open
        bool is_open() const { return m_io != nullptr; }

    protected:
        // Called when the internal get area is empty
        int_type underflow() override;

        // Handles seeking (seekg, tellg)
        pos_type seekoff(off_type off, std::ios_base::seekdir dir,
                         std::ios_base::openmode which = std::ios_base::in) override;

        pos_type seekpos(pos_type pos,
                         std::ios_base::openmode which = std::ios_base::in) override;

        // Handles custom buffer
        std::streambuf* setbuf(char_type* s, std::streamsize n) override;

    private:
        static constexpr size_t BUFFER_SIZE = 8192; // 8KB is generally better for mobile I/O
        std::array<char_type, BUFFER_SIZE> m_internal_buffer;

        char_type* m_active_buffer = nullptr;
        size_t m_active_buffer_size = 0;

        SDL_IOStream* m_io = nullptr;
    };

    /**
     * @brief A custom input stream for game assets.
     * On Android, this reads from the APK assets; on Desktop, it reads from the filesystem.
     */
    class asset_istream : public std::istream
    {
    public:
        asset_istream();
        explicit asset_istream(std::string_view fn, std::ios_base::openmode mode = std::ios_base::in);
        explicit asset_istream(const char* fn, std::ios_base::openmode mode = std::ios_base::in);
        explicit asset_istream(const std::string& fn, std::ios_base::openmode mode = std::ios_base::in);
        ~asset_istream() override;

    public:

        void open(std::string_view fn);
        void open(const char* fn);
        void open(const std::string& fn);

        bool is_open() const;
        void close();

    private:
        asset_streambuf m_streambuf;
    };
}

#else
#include <fstream>
namespace age
{
	using asset_istream = std::ifstream;
}
#endif