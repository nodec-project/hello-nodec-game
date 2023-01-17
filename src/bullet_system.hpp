#ifndef BULLET_SYSTEM_HPP_
#define BULLET_SYSTEM_HPP_

#include "app.hpp"

struct Bullet {
    float lifetime{10.f};
};

class SerializableBullet : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableBullet()
        : BaseSerializableComponent(this) {}

    template<class Archive>
    void serialize(Archive &archive) {
    }
};

CEREAL_REGISTER_TYPE(SerializableBullet)
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, SerializableBullet)

class BulletSystem {
public:
    BulletSystem(nodec_world::World &world, nodec_scene_serialization::SceneSerialization &serialization, nodec_physics::systems::PhysicsSystem &physics_system) {
        using namespace nodec_physics;
        using namespace nodec;

        serialization.register_component<Bullet, SerializableBullet>(
            [&](const Bullet &bullet) {
                auto serializable = std::make_unique<SerializableBullet>();
                return serializable;
            },
            [&](const SerializableBullet &serializable, auto entt, auto &registry) {
                auto &bullet = registry.emplace_component<Bullet>(entt).first;
            });

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

#ifdef EDITOR_MODE
public:
    static void setup_editor(nodec_scene_editor::SceneEditor &editor) {
        editor.inspector_component_registry().register_component<Bullet>(
            "Bullet",
            [](Bullet &bullet) {

            });
    }
#endif

private:
    std::vector<nodec_scene::SceneEntity> to_deletes_;
};

#endif