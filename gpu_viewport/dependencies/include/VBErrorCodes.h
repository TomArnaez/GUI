#pragma once

namespace vb::err
{
	const int success				= 0;
	const int unknown				= -1;
	const int win32					= -2;
	const int gpu_invalid			= -3;
	const int gpu_not_init			= -4;
	const int no_detector			= -5;
	const int detector_not_init		= -6;
	const int timeout				= -7;
	const int insufficient_memory	= -8;
	const int already_streaming		= -9;
	const int invalid_parameter		= -10;
}