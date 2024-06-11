#pragma once
#include <memory>
#include "VBErrorCodes.h"
#include "VBTypes.h"

#ifdef VBCORE_EXPORT
#define VBCORE_API __declspec(dllexport)
#else
#define VBCORE_API __declspec(dllimport)
#endif

namespace vb {
	class VBCore {
	public:
		VBCORE_API VBCore();
		VBCORE_API VBCore(CoreCreateInfo create_info);
		VBCORE_API ~VBCore() = default;

		VBCORE_API int InitDetector();
		VBCORE_API int InitDetectorProxy(uint32_t width, uint32_t height, uint16_t frames);
		VBCORE_API int InitRender(RenderCreateInfo create_info);
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