#pragma once

namespace engine
{
    /// \brief Delta time between one frame (or update) and the next one.
    class timestep final
    {
    public:
        timestep(float time = 0.f) 
            :m_time(time)
        {}

        /// \brief Implicit cast of timestep to float. Returns seconds by default.
        operator float() const { return m_time; }

        float seconds() const { return m_time; }
        float milliseconds() const { return m_time * 1000.f; }

    private:
        /// \brief Duration of the timestep in second.
        float m_time;
    };
}