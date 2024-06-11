#pragma once

namespace vb::err
{
	const int success				= 0;
	const int unknown				= -1;
	const int win32					= -2;
	const int gpu_invalid			= -3;
	const int gpu_not_init			= -4;
	const int gpu_runtime			= -5;
	const int no_detector			= -6;
	const int detector_not_init		= -7;
	const int detector_runtime		= -8;
	const int timeout				= -9;
	const int insufficient_memory	= -10;
	const int already_streaming		= -11;
	const int invalid_parameter		= -12;
}