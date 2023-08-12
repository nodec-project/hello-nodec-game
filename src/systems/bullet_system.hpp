#ifndef SYSTEMS__BULLET_SYSTEM_HPP_
#define SYSTEMS__BULLET_SYSTEM_HPP_

#include "../app.hpp"
#include "../components/bullet.hpp"

namespace systems {

class BulletSystem {
    struct DestroyLater {};

public:
    BulletSystem(nodec_world::World &world, nodec_physics::systems::PhysicsSystem &physics_system) {
        using namespace nodec_physics;
        using namespace nodec;
        using namespace nodec_scene;
        using namespace ::components;

        world.stepped().connect([&](nodec_world::World &world) { on_stepped(world); });

        physics_system.collision_stay().connect([&](const CollisionInfo &info) {
            auto &scene_registry = world.scene().registry();

            auto process = [&](SceneEntity current) {
                if (!scene_registry.try_get_component<Bullet>(current)) return;
                scene_registry.emplace_component<DestroyLater>(current);
            };
            process(info.entity0);
            process(info.entity1);
        });
    }

private:
    void on_stepped(nodec_world::World &world) {
        using namespace nodec_scene;
        using namespace nodec_physics::components;
        using namespace ::components;

        auto &scene_registry = world.scene().registry();

        scene_registry.view<Bullet>().each([&](auto entt, Bullet &bullet) {
            bullet.lifetime -= world.clock().delta_time();

            if (bullet.lifetime < 0.f) {
                scene_registry.emplace_component<DestroyLater>(entt);
            }
        });

        {
            auto view = scene_registry.view<DestroyLater>();
            scene_registry.destroy_entities(view.begin(), view.end());
        }
    }
};
} // namespace systems

#endif