#include <entt/entt.hpp>
#include <cstdint>
#include <string>
#include <iostream>
#include <boost/mpi.hpp>
#include <chrono>
#include <csignal>
#include <omp.h>

#include "./render/glRender.h"
#include "./data_prepare.h"

using namespace std::chrono_literals;

constexpr std::chrono::nanoseconds timestep(16ms);
constexpr int timestep_ms = std::chrono::duration_cast<std::chrono::milliseconds>(timestep).count();

int loopCount = 0;
bool handle_events()
{
    // poll for events
    loopCount += 1;
    if (loopCount > 5000000)
        return true;
    return false; // true if the user wants to quit the game
}

const float SAME_LINE_THRESHOLD = 0.1;
bool isSameLine(float2 point1, float2 point2, float2 point3)
{
    return (point1.x * point2.y - point1.y * point2.x) + (point2.x * point3.y - point2.y * point3.x) + (point3.x * point1.y - point3.y * point1.x) <= SAME_LINE_THRESHOLD;
}
bool pointInSideLineSegment(float2 lineStart, float2 lineEnd, float2 point)
{
    return isSameLine(lineStart, lineEnd, point) &&
           (((point.x >= lineStart.x && point.x <= lineEnd.x) && (point.y >= lineStart.y && point.y <= lineEnd.y)) ||
            ((point.x <= lineStart.x && point.x >= lineEnd.x) && (point.y <= lineStart.y && point.y >= lineEnd.y)));
}

void update(entt::registry &registry)
{
    auto belts = registry.view<belt>();
    auto objects = registry.view<object_data>();

#pragma omp parallel for schedule(guided)
    for (auto beltEntity : belts)
    {
        auto &beltComponent = belts.get(beltEntity);

        for (auto objectEntity : objects)
        {
            int xDirection = 0;
            int yDirection = 0;
            if (beltComponent.start.x - beltComponent.end.x > 0)
            {
                xDirection = 1;
            }
            else
            {
                xDirection = -1;
            }
            if (beltComponent.start.y - beltComponent.end.y > 0)
            {
                yDirection = 1;
            }
            else
            {
                yDirection = -1;
            }

            auto &objectComponent = objects.get(objectEntity);
            if (pointInSideLineSegment(beltComponent.start, beltComponent.end, objectComponent.pos))
            {
                objectComponent.pos.x += beltComponent.speed * timestep_ms / 1000 * xDirection;
                objectComponent.pos.y += beltComponent.speed * timestep_ms / 1000 * yDirection;
            }
        }
    }

    // for (auto objectEntity : objects)
    // {
    //     auto &objectComponent = objects.get(objectEntity);
    //     std::cout << "x: " << objectComponent.pos.x << " y: " << objectComponent.pos.y << "\n";
    // }
}

void render(glRender &glRenderer)
{

    if (!glfwWindowShouldClose(glRenderer.window))
    {
        glRenderer.render();
    }
}

void renderer(int argc, char *argv[], boost::mpi::communicator &world, boost::mpi::environment &mpi_env)
{
    // startup logs
    std::cout << "Renderer Thread started\n";
    if (argc > 1)
    {
        std::cout << "there are " << argc - 1 << " (more) arguments, they are:\n";
        std::copy(argv + 1, argv + argc, std::ostream_iterator<const char *>(std::cout, "\n"));
    }

    // state initialization
    object_data *objects;
    belt *belts;
    std::vector<float2> intersect;

    int beltNum = 3;
    int objectPerBelt = 5;
    objects = new object_data[beltNum * objectPerBelt];
    belts = new belt[beltNum];

    generate_belt_object(beltNum, objectPerBelt, objects, belts, intersect);

    entt::registry registry;
    std::uint64_t dt = 16;

    for (auto i = 0; i < beltNum; ++i)
    {
        auto beltEntity = registry.create();
        registry.assign<belt>(beltEntity, belts[i].start, belts[i].end, belts[i].speed, belts[i].length);
    }
    for (auto i = 0; i < beltNum * objectPerBelt; ++i)
    {
        auto objectEntity = registry.create();
        registry.assign<object_data>(objectEntity, objects[i].pos, objects[i].direction, objects[i].stopped);
    }

    // graphic initialization
    // renderParam param;
    // param.cameraPos = glm::vec3(1024 / 2, 768 / 2, -800);
    // param.cameraUp = glm::vec3(0, 1, 0);
    // param.cameraTarget = glm::vec3(1024 / 2, 768 / 2, -1);
    // param.lightColor = glm::vec3(1, 1, 1);
    // param.lightPos = param.cameraPos;
    // glRender glRenderer(param);
    // for (int i = 0; i < beltNum * objectPerBelt; i++)
    // {
    //     model_data data;
    //     data.init_box(i, &objects[i].pos, glm::vec3(5, 5, 5), glm::vec3(0, 1, 0));
    //     glRenderer.models.push_back(data);
    // }
    // for (int i = 0; i < intersect.size(); i++)
    // {
    //     model_data data;
    //     data.init_box(i + beltNum * objectPerBelt, &intersect[i], glm::vec3(5, 5, 5), glm::vec3(1, 0, 0));
    //     glRenderer.models.push_back(data);
    // }

    // game loop

    using clock = std::chrono::high_resolution_clock;
    std::chrono::nanoseconds lag(0ns);
    auto time_start = clock::now();
    bool quit_game = false;

    while (!quit_game)
    {
        auto delta_time = clock::now() - time_start;
        time_start = clock::now();
        lag += std::chrono::duration_cast<std::chrono::nanoseconds>(delta_time);

        quit_game = handle_events();

        // update game logic as lag permits
        while (lag >= timestep)
        {
            lag -= timestep;
            std::cout << "lag is " << lag.count() << " \n";
            update(registry);
        }

        // render(glRenderer);
    }
}

void worker(int argc, char *argv[], boost::mpi::communicator &world, boost::mpi::environment &mpi_env)
{
    std::cout << "Worker Thread " << world.rank() << " started"
              << "\n";
}

int main(int argc, char *argv[])
{
    boost::mpi::environment mpi_env{argc, argv};
    boost::mpi::communicator world;
    // enter main thread
    if (world.rank() == 0)
    {
        renderer(argc, argv, world, mpi_env);

#pragma omp parallel for schedule(guided)
        const auto n = 8;
        for (auto i = 0; i < n; i++)
        {
            std::cout << i << ' ';
        }
    }
    else
    {
        worker(argc, argv, world, mpi_env);
    }
}
