#pragma once

namespace vb {
	struct ROI {
		uint32_t x = 0;
		uint32_t y = 0;
		uint32_t width = 0;
		uint32_t height = 0;

		ROI() {}

		ROI(uint32_t width, uint32_t height)
			: width(width), height(height) {}

		ROI(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
			: x(x), y(y), width(width), height(height) {}
	};

	enum class LogLevel {
		Trace,
		Debug,
		Info,
		Warn,
		Error,
		Critical
	};

	struct CoreCreateInfo {
		LogLevel file;
		LogLevel console;
		const char* filepath;
	};

	struct RenderCreateInfo {
		void* window_handle;
		const char* shader_path;
		const char* dark_map_path;
		const char* gain_map_path;
		const char* defect_map_path;
		bool enable_validation_layers;
	};
}