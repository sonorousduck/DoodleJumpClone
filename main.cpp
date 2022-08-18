#include "GameModel.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>


#include <algorithm>
#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>
#include <latch>
#include <memory>
#include <sstream>
#include <string>

const std::string CONFIG_SETTINGS_FILENAME = "client.settings.json";
const std::string CONFIG_DEVELOPER_FILENAME = "client.developer.json";
const std::string CONFIG_SCORES_FILENAME = "client.scores.json";


auto readConfiguration()
{
    // Reference: https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
    // Using Jerry's answer, because it was benchmarked to be quite fast, even though the config files are small.
    std::ifstream inSettings(CONFIG_SETTINGS_FILENAME);
    std::stringstream bufferSettings;
    bufferSettings << inSettings.rdbuf();
    inSettings.close();

    std::stringstream bufferDeveloper;
    std::ifstream inDeveloper(CONFIG_DEVELOPER_FILENAME);
    if (inDeveloper)
    {
        bufferDeveloper << inDeveloper.rdbuf();
        inDeveloper.close();
    }

    return Configuration::instance().initialize(bufferSettings.str(), bufferDeveloper.str());
}





std::shared_ptr<sf::RenderWindow> prepareWindow()
{
	// Settings lets you define a bunch of extra options see https://www.sfml-dev.org/documentation/2.5.1/structsf_1_1ContextSettings.php for more
    sf::ContextSettings settings;

	// Landscape windows
    //auto window = std::make_shared<sf::RenderWindow>(sf::VideoMode(640, 480), "DoodleJump", sf::Style::Titlebar | sf::Style::Close, settings);
    //auto window = std::make_shared<sf::RenderWindow>(sf::VideoMode(800, 600), "DoodleJump", sf::Style::Titlebar | sf::Style::Close, settings);
    auto window = std::make_shared<sf::RenderWindow>(sf::VideoMode(1024, 768), "DoodleJump", sf::Style::Titlebar | sf::Style::Close, settings);
    //auto window = std::make_shared<sf::RenderWindow>(sf::VideoMode(1920, 1080), "DoodleJump", sf::Style::Titlebar | sf::Style::Close, settings);

    // Vertical windows
    //auto window = std::make_shared<sf::RenderWindow>(sf::VideoMode(600, 800), "DoodleJump", sf::Style::Titlebar | sf::Style::Close, settings);
    //auto window = std::make_shared<sf::RenderWindow>(sf::VideoMode(1080, 1920), "DoodleJump", sf::Style::Titlebar | sf::Style::Close, settings);

    window->setVerticalSyncEnabled(true);

    return window;
}

void prepareView(std::shared_ptr<sf::RenderWindow> window)
{
    // Aspect ratio is used to organize viewport
    auto aspectRatio = static_cast<float>(window->getSize().x) / window->getSize().y;

    // Want the view to be a rectangular section of the window
    sf::View view(sf::Vector2f(0.0, 0.0), {1.0f, 1.0f});
    if (aspectRatio > 1.0)
    {
        auto extra = (1.0f - (1.0f / aspectRatio)) / 2.0f;
        view.setViewport(sf::FloatRect(sf::Vector2f(extra, 0.0f), sf::Vector2f(1.0f - extra * 2, 1.0f)));
    }
    else
    {
        auto extra = (1.0f - aspectRatio) / 2.0f;
        view.setViewport(sf::FloatRect(sf::Vector2f(0.0f, extra), sf::Vector2f(1.0f, 1.0f - extra * 2)));
    }

    window->setView(view);
}

int main()
{
    // Create and activate the window for rendering on main thread
    auto window = prepareWindow();
    prepareView(window);
    window->setActive(true);

    // Initialize game model
    GameModel model;
    if (!model.initialize(window->getView().getSize()))
    {
        std::cout << "Game model failed to initialize, terminating..." << std::endl;
        exit(0);
    }

    // Get the initial time-stamp
    auto previousTime = std::chrono::steady_clock::now();

    // Get the Window loop running, gameloop runs inside this loop

    bool running = true;
    while (running)
    {
        sf::Event event;
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                running = false;
            }

            if (event.type == sf::Event::KeyPressed)
            {
                model.signalKeyPressed(event.key);
            }
            if (event.type == sf::Event::KeyReleased)
            {
                model.signalKeyReleased(event.key);
            }
        }

        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - previousTime);
        previousTime = currentTime;

        // Update the game model
        model.update(elapsedTime, window);

        // Display the window finally after the update has finished
        window->display();
    }

    return 0;
}