#pragma once

namespace engine
{
	class timer
	{
	public:
		/// \brief Starts the timer
		void start();
		/// \brief Resets the timer.
		void reset();
		/// \brief Returns the seconds passed since 
		/// the last Elapsed() call or since the Start().
		double elapsed();
		/// \brief Returns the milliseconds passed since 
		/// the last Elapsed() call or since the Start().
		double elapsed_millis();
		/// \brief Returns the seconds passed since 
		/// the first Start() call.
		double total() const;

	private:
		double m_start_time{0.0};
		double m_delta_time{0.0};
		double m_last_frame{0.0};
		bool m_started = false;
	};
}
