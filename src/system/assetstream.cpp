#include "system/assetstream.h"

#include <SDL3/SDL.h>

#if defined(ANDROID) || defined(__ANDROID__)
namespace age
{
	asset_streambuf::asset_streambuf()
	{
		
	}

	asset_streambuf::asset_streambuf(std::string_view fn)
	{
		open(fn);
	}

	asset_streambuf::~asset_streambuf()
	{
		sync();
		//Close the asset
		AAssetManager_close(asset);
	}

	void asset_streambuf::open(std::string_view fn)
	{
		/*
		auto activity = SDL_AndroidGetActivity();

		asset = AAssetManager_open(manager, filename.c_str(), AASSET_MODE_STREAMING);
		buffer.resize(1024);

		setg(0, 0, 0);
		setp(&buffer.front(), &buffer.front() + buffer.size());
		*/
	}

	std::streambuf::int_type asset_streambuf::underflow()
	{
		/*
		auto bufferPtr = &m_buffer.front();
		auto counter = AAsset_read(asset, bufferPtr, buffer.size());

		if (counter == 0)
			return traits_type::eof();
		if (counter < 0) //error, what to do now?
			return traits_type::eof();

		setg(bufferPtr, bufferPtr, bufferPtr + counter);

		return traits_type::to_int_type(*gptr());
		*/

		return traits_type::eof();
	}

	std::streambuf::pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which)
	{

	}

	std::streambuf::pos_type seekpos(std::streambuf::pos_type pos, std::ios_base::openmode which)
	{

	}

	//ToDo: Check if can pass in the streambuf as a membervariable. If this doesnt work it needs to be done via the heap
	assetistream::assetistream()
		: std::istream{ &m_streambuf }
	{}

	assetistream::assetistream(std::string_view fn)
		: std::istream{ &m_streambuf }
		, m_streambuf{ fn }
	{}

	assetistream::~assetistream()
	{}
}
#endif