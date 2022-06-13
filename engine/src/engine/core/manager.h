#pragma once

namespace engine
{
	/// \brief Interface from which each manager class should implement,
	/// so that they can all follow the initialization and shutdown routine.
	class manager
	{
	public:
		virtual void init() = 0;
		virtual void shutdown() = 0;

		virtual ~manager() {}
	};
}
