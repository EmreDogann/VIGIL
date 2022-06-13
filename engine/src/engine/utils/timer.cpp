#include "pch.h"
#include "timer.h"
#include <GLFW/glfw3.h>

void engine::timer::start()
{
	m_started = true;
	reset();
}

void engine::timer::reset()
{
	// current time in seconds
	m_start_time = glfwGetTime();
	m_last_frame = m_start_time;
}

double engine::timer::elapsed()
{
	if(!m_started)
		return 0.0;

	// current time in seconds
	const double currentFrame = glfwGetTime();

	m_delta_time = currentFrame - m_last_frame;
	m_last_frame = currentFrame;

	return m_delta_time;
}

double engine::timer::elapsed_millis()
{
	return elapsed() * 1000.f;
}

double engine::timer::total() const
{
	// current time in seconds
	const double current = glfwGetTime();
	return current - m_start_time;
}
