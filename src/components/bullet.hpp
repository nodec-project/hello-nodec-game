#ifndef APP__COMPONENTS__BULLET_HPP_
#define APP__COMPONENTS__BULLET_HPP_

#include <nodec_scene_serialization/serializable_component.hpp>

namespace app {
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
} // namespace app

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(app::components::Bullet)
#endif