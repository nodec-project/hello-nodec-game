#ifndef SYSTEMS__BULLET_SYSTEM_HPP_
#define SYSTEMS__BULLET_SYSTEM_HPP_

#include "../app.hpp"
#include "../components/bullet.hpp"

namespace systems {

class BulletSystem {
public:
    BulletSystem(nodec_world::World &world, nodec_physics::systems::PhysicsSystem &physics_system) {
        using namespace nodec_physics;
        using namespace nodec;

        world.stepped().connect([&](nodec_world::World &world) { on_stepped(world); });

        // physics_system.collision_stay().connect([&](const CollisionInfo &info) {
        //     if (world.scene().registry().try_get_component<Bullet>(info.entity0)) {
        //         world.scene().registry().destroy_entity(info.entity0);
        //     }
        //     if (world.scene().registry().try_get_component<Bullet>(info.entity1)) {
        //         world.scene().registry().destroy_entity(info.entity1);
        //     }
        // });
    }

    void on_stepped(nodec_world::World &world) {
        using namespace nodec_scene;
        using namespace nodec_physics::components;
        using namespace ::components;

        world.scene().registry().view<Bullet>().each([&](auto entt, Bullet &bullet) {
            bullet.lifetime -= world.clock().delta_time();

            if (bullet.lifetime < 0.f) {
                to_deletes_.emplace_back(entt);
            }
        });

        world.scene().registry().destroy_entities(to_deletes_.begin(), to_deletes_.end());
        to_deletes_.clear();

        world.scene().registry().view<Bullet, CollisionStay>().each([&](auto entt, Bullet, CollisionStay) {
            to_deletes_.emplace_back(entt);
        });

        world.scene().registry().destroy_entities(to_deletes_.begin(), to_deletes_.end());
        to_deletes_.clear();
    }

private:
    std::vector<nodec_scene::SceneEntity> to_deletes_;
};
} // namespace systems

#endif