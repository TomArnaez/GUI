#pragma once

namespace vb::err {
	enum error_enum : int {
		unknown = INT_MIN,
		win32,
		gpu_invalid,
		gpu_not_init,
		gpu_runtime,
		no_detector,
		detector_not_init,
		detector_runtime,
		timeout,
		insufficient_memory,
		already_streaming,
		already_init,
		invalid_parameter,
		
		success = 0,	// Failure codes are all negative
	};
}