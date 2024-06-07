#pragma once
#include <memory>
#include "VBErrorCodes.h"

#ifdef VBCORE_EXPORT
#define VBCORE_API __declspec(dllexport)
#else
#define VBCORE_API __declspec(dllimport)
#endif

namespace vb
{
	struct ROI {
		uint32_t x		= 0;
		uint32_t y		= 0;
		uint32_t width	= 0;
		uint32_t height	= 0;

		ROI() {}

		ROI(uint32_t width, uint32_t height)
			: width(width), height(height) {}

		ROI(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
			: x(x), y(y), width(width), height(height) {}
	};

	class VBCore {
	public:
		VBCORE_API VBCore();
		VBCORE_API ~VBCore() = default;

		VBCORE_API int InitDetector();
		VBCORE_API int InitRender(void* args);
		VBCORE_API ROI GetImageSize();
		VBCORE_API ROI GetROI();
		VBCORE_API int SetROI(ROI roi);
		VBCORE_API int WriteRegister(uint32_t address, uint32_t data);
		VBCORE_API int ReadRegister(uint32_t address);
		VBCORE_API int ConfigureStream(uint32_t exposure_time_10us);
		VBCORE_API int StartStream();
		VBCORE_API int StopStream();
		VBCORE_API int Refresh();
	private:
		class impl; std::shared_ptr<impl> pimpl;
	};
}