#pragma once
#include <memory>
#include "error_codes.h"
#include "types.h"

#ifdef VBCORE_EXPORT
#define VBCORE_API __declspec(dllexport)
#else
#define VBCORE_API __declspec(dllimport)
#endif

namespace vb {
	class Core {
	public:
		VBCORE_API Core();
		VBCORE_API Core(CoreCreateInfo create_info);
		VBCORE_API ~Core() = default;

		VBCORE_API err::error_enum InitDetector();
		VBCORE_API err::error_enum InitDetectorProxy(uint32_t width, uint32_t height, uint16_t frames);
		VBCORE_API err::error_enum InitRender(RenderCreateInfo create_info);
		VBCORE_API ROI GetImageSize();
		VBCORE_API ROI GetROI();
		VBCORE_API err::error_enum SetROI(ROI roi);
		VBCORE_API err::error_enum WriteRegister(uint32_t address, uint32_t data);
		VBCORE_API err::error_enum ReadRegister(uint32_t address);
		VBCORE_API err::error_enum ConfigureStream(uint32_t exposure_time_10us);
		VBCORE_API err::error_enum StartStream();
		VBCORE_API err::error_enum StopStream();
		VBCORE_API void SetAveraging(bool enable);
		VBCORE_API void Refresh();
		VBCORE_API bool Streaming() const;
	private:
		class impl; std::shared_ptr<impl> pimpl;
	};
}