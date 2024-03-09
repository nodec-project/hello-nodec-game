#ifndef APP__SYSTEMS__BULLET_SYSTEM_HPP_
#define APP__SYSTEMS__BULLET_SYSTEM_HPP_

#include <nodec_physics/systems/physics_system.hpp>
#include <nodec_scene_serialization/serializable_component.hpp>
#include <nodec_world/world.hpp>

#include "../components/bullet.hpp"

namespace app {
namespace components {

struct BulletSystemEnabler : nodec_scene_serialization::BaseSerializableComponent {
    BulletSystemEnabler()
        : BaseSerializableComponent{this} {
    }

    template<class Archive>
    void serialize(Archive &archive) {
    }
};

} // namespace components
} // namespace app

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(app::components::BulletSystemEnabler)

namespace app {
namespace systems {

class BulletSystem {
    struct DestroyLater {};

public:
    BulletSystem(nodec_world::World &world, nodec_physics::systems::PhysicsSystem &physics_system)
        : physics_system_(physics_system) {
        connections_.push_back(
            world.stepped().connect([&](nodec_world::World &world) { on_stepped(world); }));
    }

private:
    void on_stepped(nodec_world::World &world) {
        using namespace nodec_scene;
        using namespace components;
        using namespace nodec_physics;

        auto &scene_registry = world.scene().registry();

        scene_registry.view<Bullet>().each([&](SceneEntity entity, Bullet &bullet) {
            bullet.lifetime -= world.clock().delta_time();

            if (bullet.lifetime < 0.f) {
                scene_registry.emplace_component<DestroyLater>(entity);
            }

            physics_system_.contact_test(entity, [&](CollisionInfo &info) {
                scene_registry.emplace_component<DestroyLater>(entity);
            });
        });

        {
            auto view = scene_registry.view<DestroyLater>();
            scene_registry.destroy_entities(view.begin(), view.end());
        }
    }

private:
    nodec_physics::systems::PhysicsSystem &physics_system_;

    std::vector<nodec::signals::Connection> connections_;
};
} // namespace systems
} // namespace app

#endif