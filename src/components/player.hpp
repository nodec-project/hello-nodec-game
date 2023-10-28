#ifndef HELLO_NODEC_GAME__COMPONENTS__PLAYER_HPP_
#define HELLO_NODEC_GAME__COMPONENTS__PLAYER_HPP_

#include <nodec_scene_serialization/serializable_component.hpp>

namespace hello_nodec_game {
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
} // namespace hello_nodec_game

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(hello_nodec_game::components::PlayerControl)
#endif