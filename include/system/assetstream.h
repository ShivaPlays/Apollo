#pragma once

#if defined(ANDROID) || defined(__ANDROID__)
#include <SDL3/SDL.h>
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

        bool open(std::string_view fn);
        void close();

        // Check if the file is currently open
        bool is_open() const { return m_io != nullptr; }

    protected:
        // Called when the internal get area is empty
        virtual int_type underflow() override;

        // Handles seeking (seekg, tellg)
        virtual pos_type seekoff(off_type off, std::ios_base::seekdir dir,
                                 std::ios_base::openmode which = std::ios_base::in) override;

        virtual pos_type seekpos(pos_type pos,
                                 std::ios_base::openmode which = std::ios_base::in) override;

    private:
        SDL_IOStream* m_io = nullptr;
        std::vector<char> m_buffer;
        static constexpr size_t BUFFER_SIZE = 8192; // 8KB is generally better for mobile I/O
    };

    /**
     * @brief A custom input stream for game assets.
     * On Android, this reads from the APK assets; on Desktop, it reads from the filesystem.
     */
    class assetistream : public std::istream
    {
    public:
        assetistream();
        explicit assetistream(std::string_view fn, std::ios_base::openmode mode = std::ios_base::in);
        virtual ~assetistream() override;

        void open(std::string_view fn);
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
	using assetistream = std::ifstream;
}
#endif