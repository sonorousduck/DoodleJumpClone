#pragma once

/*
Adapted from Professor James Dean Mathias's code
*/

#include <string>
#include <vector>
#include "math.hpp"

// ---------------------------------------------------------
//
// Client Configuration Settings
//
// Note: This is a singleton
// 
// ---------------------------------------------------------

class Configuration
{
  public: 
	Configuration(const Configuration&) = delete;
    Configuration(Configuration&&) = delete;
    Configuration& operator=(const Configuration&) = delete;
    Configuration& operator=(Configuration&&) = delete;

    static Configuration& instance();

    bool initialize(const std::string_view jsonSettings, const std::string_view jsonDeveloper);
    void reset();
    std::string serialize();

    template <typename T>
    static T get(const std::vector<std::string>& path);

    template <typename T>
    static void set(const std::vector<std::string>& path, T value);

    class Graphics
    {
      public: 
        auto getScale() { return m_scale; }
        auto getScaleUI() { return m_scaleUI; }
        auto getViewCoordinates() { return m_viewCoordinates; }
        auto getResolution() { return m_resolution; }
        auto getBpp() { return m_bpp; }
        auto restart() { return m_restart; }

        void setViewCoordinates(math::Dimension2f coordinates)
        {
            m_viewCoordinates = coordinates;
            updateScale();
        }

        void setResolution(math::Dimension2u resolution) { m_resolution = resolution; }
        void setBpp(unsigned int bpp) { m_bpp = bpp; }
        void setRestart(bool state) { m_restart = state; }



      private:
        friend Configuration;

        bool m_uiScaled{false};
        math::Vector2f m_scale { 0.1f, 0.1f };
        math::Vector2f m_scaleUI{m_scale};
        math::Dimension2f m_viewCoordinates{100.0f, 100.0f};
        math::Dimension2u m_resolution{640, 480};
        unsigned int m_bpp{24};
        bool m_restart{false}; // Set to true when the window should be restarted due to a change in the graphics options
        
        void updateScale();
    };

    static Graphics& getGraphics() { return instance().getGraphics; }


    private:
    Configuration() {}

    Graphics m_graphics;
};

