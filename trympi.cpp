#include <entt/entt.hpp>
#include <cstdint>
#include <string>
#include <iostream>
#include <boost/mpi.hpp>

#include "./data_prepare.h"

const float SAME_LINE_THRESHOLD = 0.1;
bool isSameLine(float2 point1, float2 point2, float2 point3)
{
    return (point1.x * point2.y - point1.y * point2.x) + (point2.x * point3.y - point2.y * point3.x) + (point3.x * point1.y - point3.y * point1.x) <= SAME_LINE_THRESHOLD;
}

void update(std::uint64_t dt, entt::registry &registry)
{
    auto belts = registry.view<belt>();
    auto objects = registry.view<object_data>();

    auto counter = 0;
    for (auto beltEntity : belts)
    {
        auto &beltComponent = belts.get(beltEntity);
        for (auto objectEntity : objects)
        {
            auto &objectComponent = objects.get(objectEntity);
            if (isSameLine(beltComponent.start, beltComponent.end, objectComponent.pos))
            {
                counter++;
            }
        }
    }
    std::cout << counter << " same line!\n";
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

    // game loop

    // for (auto i = 0; i < beltNum * objectPerBelt; ++i)
    // {
    //     std::cout << "x: " << objects[0].pos.x << " y: " << objects[0].pos.y << "\n";
    // }
    update(dt, registry);

    // for (auto i = 0; i < beltNum * objectPerBelt; ++i)
    // {
    //     std::cout << "x: " << objects[0].pos.x << " y: " << objects[0].pos.y << "\n";
    // }
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
    }
    else
    {
        worker(argc, argv, world, mpi_env);
    }
}