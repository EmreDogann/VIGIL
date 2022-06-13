#pragma once 

namespace engine
{
	/// \brief Graphics context (platform agnostic) interface
	class graphics_context
	{
	public:
		virtual void init() = 0;
		virtual void swap_buffers() = 0;
	};
}
