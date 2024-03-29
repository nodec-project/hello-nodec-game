#ifndef APP__SYSTEMS__PLAYER_CONTROL_SYSTEM_HPP_
#define APP__SYSTEMS__PLAYER_CONTROL_SYSTEM_HPP_

#include <nodec/math/gfx.hpp>
#include <nodec/math/math.hpp>

#include <nodec_input/keyboard/keyboard.hpp>
#include <nodec_input/mouse/mouse.hpp>
#include <nodec_physics/components/impluse_force.hpp>
#include <nodec_resources/resources.hpp>
#include <nodec_scene_serialization/scene_serialization.hpp>
#include <nodec_scene_serialization/serializable_component.hpp>
#include <nodec_world/world.hpp>

#include "../components/bullet.hpp"
#include "../components/player.hpp"

namespace app {
namespace components {
struct PlayerControlSystemEnabler : nodec_scene_serialization::BaseSerializableComponent {
    PlayerControlSystemEnabler()
        : BaseSerializableComponent{this} {
    }

    template<class Archive>
    void serialize(Archive &archive) {
    }
};

} // namespace components
} // namespace app

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(app::components::PlayerControlSystemEnabler)

namespace app {
namespace systems {

class PlayerControlSystem {
public:
    PlayerControlSystem(nodec_world::World &world,
                        nodec_resources::Resources &resources,
                        std::shared_ptr<nodec_input::keyboard::Keyboard> keyboard,
                        std::shared_ptr<nodec_input::mouse::Mouse> mouse,
                        nodec_scene_serialization::SceneSerialization &serialization)
        : serialization_(serialization), resources_(resources) {
        using namespace nodec;
        using namespace nodec_scene;
        using namespace nodec_input::keyboard;
        using namespace nodec_input::mouse;
        using namespace nodec_scene_serialization;
        using namespace nodec_scene::components;
        using namespace components;

        connections_.push_back(
            world.stepped().connect([&](nodec_world::World &world) { on_stepped(world); }));

        connections_.push_back(
            keyboard->key_event().connect([&](const nodec_input::keyboard::KeyEvent &event) {
                // logging::InfoStream(__FILE__, __LINE__) << event;
                if (event.key == Key::W) {
                    w_pressed = (event.type == KeyEvent::Type::Press);
                }
                if (event.key == Key::A) {
                    a_pressed = (event.type == KeyEvent::Type::Press);
                }
                if (event.key == Key::S) {
                    s_pressed = (event.type == KeyEvent::Type::Press);
                }
                if (event.key == Key::D) {
                    d_pressed = (event.type == KeyEvent::Type::Press);
                }
            }));

        connections_.push_back(
            mouse->mouse_event().connect([&](const nodec_input::mouse::MouseEvent &event) {
                // logging::InfoStream(__FILE__, __LINE__) << event;
                static Vector2i prev_pos;

                if (event.type == MouseEvent::Type::Press && event.button == MouseButton::Right) {
                    prev_pos = event.position;
                }
                if (event.type == MouseEvent::Type::Move && (event.buttons & MouseButton::Right)) {
                    rotation_delta += event.position - prev_pos;
                    prev_pos = event.position;
                }

                if (event.button & MouseButton::Left) {
                    left_pressed_ = (event.type == MouseEvent::Type::Press);
                }
            }));

        connections_.push_back(
            world.initialized().connect([=](nodec_world::World &world) {
                // Resources module is invalid in the configuration phase.
                bullet_prototype_ = resources_.registry().get_resource_direct<SerializableEntity>("org.nodec.hello-nodec-game/prefabs/bullet.entity");
                if (!bullet_prototype_) {
                    logging::warn(__FILE__, __LINE__) << "Failed to get bullet scene.";
                }
            }));
    }

private:
    void on_stepped(nodec_world::World &world) {
        using namespace nodec;
        using namespace nodec::entities;
        using namespace nodec_scene;
        using namespace nodec_scene::components;
        using namespace nodec_scene_serialization::components;
        using namespace nodec_scene_serialization;
        using namespace nodec_physics::components;
        using namespace components;

        SceneEntity player_entt{null_entity};

        {
            auto view = world.scene().registry().view<LocalTransform, PlayerControl>();
            player_entt = *view.begin();

            // Nothing to do more.
            if (player_entt == null_entity) return;

            auto &trfm = view.get<LocalTransform>(player_entt);
            auto &control = view.get<PlayerControl>(player_entt);

            const float delta_time = world.clock().delta_time();

            auto forward = math::gfx::rotate(Vector3f(0, 0, 1), trfm.rotation);
            auto right = math::gfx::rotate(Vector3f(1, 0, 0), trfm.rotation);

            Vector2f move_vec;

            if (w_pressed) move_vec.y += 1;
            if (s_pressed) move_vec.y -= 1;
            if (a_pressed) move_vec.x -= 1;
            if (d_pressed) move_vec.x += 1;
            if (math::norm(move_vec) > 0.001f) {
                move_vec = math::normalize(move_vec);

                trfm.position += move_vec.y * forward * control.speed * delta_time;
                trfm.position += move_vec.x * right * control.speed * delta_time;

                trfm.dirty = true;
            }

            if (math::norm(rotation_delta) > 1) {
                constexpr float SCALE_FACTOR = 0.1f;

                // Apply rotation around the local right vector after current rotation.
                trfm.rotation = math::gfx::quaternion_from_angle_axis(rotation_delta.y * SCALE_FACTOR, right) * trfm.rotation;

                // And apply rotation around the world up vector.
                trfm.rotation = math::gfx::quaternion_from_angle_axis(rotation_delta.x * SCALE_FACTOR, Vector3f(0.f, 1.f, 0.f)) * trfm.rotation;

                trfm.dirty = true;
                rotation_delta.set(0, 0);
            }
        }

        [&]() {
            if (!left_pressed_) return;
            if (world.clock().current_time() - prev_fire_time_ < 0.25f) return;

            prev_fire_time_ = world.clock().current_time();

            logging::info(__FILE__, __LINE__) << "fire!";

            if (!bullet_prototype_) return;

            auto entt = world.scene().create_entity();
            EntityBuilder(serialization_).build(bullet_prototype_.get(), entt, world.scene());

            auto &force = world.scene().registry().emplace_component<ImpulseForce>(entt).first;
            auto &trfm = world.scene().registry().get_component<LocalTransform>(entt);

            auto &player_trfm = world.scene().registry().get_component<LocalTransform>(player_entt);
            trfm.position = player_trfm.position;
            trfm.dirty = true;

            auto forward = math::gfx::rotate(Vector3f(0, 0, 1), player_trfm.rotation);
            force.force = forward * 100.f;
        }();
    }

private:
    nodec_scene_serialization::SceneSerialization &serialization_;
    nodec_resources::Resources &resources_;

    bool w_pressed{false};
    bool a_pressed{false};
    bool s_pressed{false};
    bool d_pressed{false};

    nodec::Vector2i rotation_delta;
    bool left_pressed_{false};
    float prev_fire_time_{0.0f};

    std::shared_ptr<nodec_scene_serialization::SerializableEntity> bullet_prototype_;

    std::vector<nodec::signals::Connection> connections_;
};

} // namespace systems
} // namespace app

#endif