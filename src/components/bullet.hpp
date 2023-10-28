#ifndef HELLO_NODEC_GAME__COMPONENTS__BULLET_HPP_
#define HELLO_NODEC_GAME__COMPONENTS__BULLET_HPP_

#include <nodec_scene_serialization/serializable_component.hpp>

namespace hello_nodec_game {
namespace components {

class Bullet : public nodec_scene_serialization::BaseSerializableComponent {
public:
    Bullet()
        : BaseSerializableComponent(this) {}

    float lifetime{10.f};

    template<class Archive>
    void serialize(Archive &archive) {
    }
};

} // namespace components
} // namespace hello_nodec_game

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(hello_nodec_game::components::Bullet)
#endif