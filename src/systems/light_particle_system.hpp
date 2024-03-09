#ifndef APP__SYSTEMS__LIGHT_PARTICLE_HPP_
#define APP__SYSTEMS__LIGHT_PARTICLE_HPP_

#include <nodec/math/gfx.hpp>
#include <nodec/math/math.hpp>
#include <nodec/random.hpp>
#include <nodec/resource_management/resource_registry.hpp>
#include <nodec_scene_serialization/components/non_serialized.hpp>

#include <nodec_animation/animation_curve.hpp>
#include <nodec_scene_serialization/scene_serialization.hpp>
#include <nodec_world/world.hpp>

namespace app {
namespace components {
struct LightParticleSystemEnabler : nodec_scene_serialization::BaseSerializableComponent {
    LightParticleSystemEnabler()
        : BaseSerializableComponent{this} {
    }

    template<class Archive>
    void serialize(Archive &archive) {
    }
};

} // namespace components
} // namespace app

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(app::components::LightParticleSystemEnabler)

namespace app {
namespace systems {
class LightParticleSystem {
public:
    LightParticleSystem(nodec_world::World &world, nodec::resource_management::ResourceRegistry &resource_registry,
                        nodec_scene_serialization::SceneSerialization &serialization)
        : world_(world) {
        using namespace nodec;
        using namespace nodec_scene::components;
        using namespace nodec_scene_serialization;
        using namespace nodec_scene_serialization::components;
        using namespace nodec_animation;

        curve.add_keyframe(Keyframe{0, 0.0f});
        curve.add_keyframe(Keyframe{30 * 1000, 360.0f});
        curve.set_wrap_mode(WrapMode::Loop);

        center_entt = world.scene().create_entity("Particles");

        world.scene().registry().emplace_component<NonSerialized>(center_entt);

        auto proto_light = resource_registry.get_resource_direct<SerializableEntity>("org.nodec.hello-nodec-game/prefabs/particle-light.entity");

        constexpr int COUNT = 100;
        for (int i = 0; i < COUNT; ++i) {
            auto entt = world.scene().create_entity("light");
            EntityBuilder(serialization).build(proto_light.get(), entt, world.scene());
            world.scene().hierarchy_system().append_child(center_entt, entt);
            auto &trfm = world.scene().registry().get_component<LocalTransform>(entt);
            Vector3f vec{
                random::global().uniform<float>() * 2.0f - 1.0f,
                random::global().uniform<float>() * 2.0f - 1.0f,
                random::global().uniform<float>() * 2.0f - 1.0f};
            vec = math::normalize(vec);
            vec *= random::global().uniform<float>() * 10.0f;
            trfm.position = vec;
            trfm.dirty = true;
        }

        connections_.push_back(
            world.stepped().connect([&](nodec_world::World &world) {
                if (!world.scene().registry().is_valid(center_entt)) return;

                auto &trfm = world.scene().registry().get_component<LocalTransform>(center_entt);
                trfm.rotation = math::gfx::quaternion_from_angle_axis(curve.evaluate(world.clock().current_time() * 1000).second, Vector3f{0.0f, 1.0f, 0.0f});
                trfm.dirty = true;
            }));
    }

    ~LightParticleSystem() {
        if (world_.scene().registry().is_valid(center_entt)) {
            world_.scene().registry().destroy_entity(center_entt);
        }
    }

private:
    nodec_world::World &world_;
    nodec_scene::SceneEntity center_entt{nodec::entities::null_entity};
    nodec_animation::AnimationCurve curve;

    std::vector<nodec::signals::Connection> connections_;
};

} // namespace systems
} // namespace app

#endif