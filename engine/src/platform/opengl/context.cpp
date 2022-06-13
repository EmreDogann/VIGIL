#include "pch.h"
#include "platform/opengl/context.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "engine/core.h"

engine::opengl_context::opengl_context(GLFWwindow* window_handle)
	: m_window_handle(window_handle)
{
	CORE_ASSERT(m_window_handle, "[opengl_context] window handle is null!")
}

void engine::opengl_context::init()
{
	LOG_CORE_INFO("[opengl_context] Creating graphics context.");
	glfwMakeContextCurrent(m_window_handle);
	const int status = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
    CORE_ASSERT(status, "[opengl_context] Could not load Glad!");
}

void engine::opengl_context::swap_buffers()
{
	glfwSwapBuffers(m_window_handle);
}
