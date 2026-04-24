#include "system/mem_istream.h"

namespace age
{
	mem_streambuf::mem_streambuf()
		: m_data{}
	{}

	mem_streambuf::mem_streambuf(std::byte data[], size_t size)
		: m_data{ data }
	{
		setg(reinterpret_cast<char*>(data), reinterpret_cast<char*>(data), reinterpret_cast<char*>(data + size));
	}

	void mem_streambuf::open(std::byte data[], size_t size)
	{
		m_data = data;

		setg(reinterpret_cast<char*>(data), reinterpret_cast<char*>(data), reinterpret_cast<char*>(data + size));
	}

	std::streambuf::int_type mem_streambuf::underflow()
	{
		//As we passed in all the data when opening or in constructor, an underflow can only mean eof
		return traits_type::eof();
	}

	std::streambuf::pos_type mem_streambuf::seekoff(std::streambuf::off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which)
	{
		char* const begin = reinterpret_cast<char*>(m_data);
		char* const end = egptr();
		char* nextptr = nullptr;

		switch (dir)
		{
			case std::ios_base::beg:  nextptr = begin + off; break;
			case std::ios_base::end:  nextptr = end + off; break;
			default: nextptr = gptr() + off;
		}

		// BOUNDS CHECK: Ensure we are within [begin, end]
		if (nextptr < begin || nextptr > end)
			return pos_type{ off_type(-1) };

		setg(begin, nextptr, end);
		return pos_type{ nextptr - begin };
	}

	std::streambuf::pos_type mem_streambuf::seekpos(std::streambuf::pos_type pos, std::ios_base::openmode which)
	{
		setg(reinterpret_cast<char*>(m_data), reinterpret_cast<char*>(m_data + pos), reinterpret_cast<char*>(egptr()));
		return pos;
	}

	// --- asset_istream Implementation ---

	mem_istream::mem_istream()
		: std::istream{ &m_streambuf }
	{}

	mem_istream::mem_istream(std::byte data[], size_t size)
		: std::istream{ &m_streambuf }
		, m_streambuf{ data, size }
	{}
}