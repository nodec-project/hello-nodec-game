#ifndef APP__COMPONENTS__PLAYER_HPP_
#define APP__COMPONENTS__PLAYER_HPP_

#include <nodec_scene_serialization/serializable_component.hpp>

namespace app {
namespace components {

struct PlayerControl : public nodec_scene_serialization::BaseSerializableComponent {
public:
    PlayerControl()
        : BaseSerializableComponent{this} {
    }

    float speed{1.0f};

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("speed", speed));
    }
};

} // namespace components
} // namespace app

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(app::components::PlayerControl)
#endif