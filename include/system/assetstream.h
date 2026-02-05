#pragma once

#if defined(ANDROID) || defined(__ANDROID__)
#include <streambuf>
#include <istream>
#include <string_view>
#include <array>

#include "../utility/utility.h"
namespace age
{
	class asset_streambuf
		: public std::streambuf
	{
	public:
		asset_streambuf();
		asset_streambuf(std::string_view fn);

		virtual ~asset_streambuf() override;

		void open(std::string_view fn);

	public:

	protected:
		virtual std::streambuf::int_type underflow() override;
		virtual std::streambuf::pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override;
		virtual std::streambuf::pos_type seekpos(std::streambuf::pos_type pos, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override;

	private:
		std::array<char, 1024> m_buffer{};
	};

	class assetistream
		: public std::istream
	{
	public:
		assetistream();
		assetistream(std::string_view fn);

		virtual ~assetistream() override;
	public:

	protected:

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