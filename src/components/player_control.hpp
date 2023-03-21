#ifndef COMPONENTS__PLAYER_CONTROL_HPP_
#define COMPONENTS__PLAYER_CONTROL_HPP_

#include <nodec_scene_serialization/serializable_component.hpp>

#ifdef EDITOR_MODE
#    include <nodec_scene_editor/scene_editor.hpp>
#endif

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
#ifdef EDITOR_MODE
    static void setup_editor(nodec_scene_editor::SceneEditor &editor) {
        editor.inspector_component_registry().register_component<PlayerControl>(
            "Player Control",
            [](PlayerControl &control) {
                ImGui::DragFloat("Speed", &control.speed);
            });
    }
#endif
};

} // namespace components

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(components::PlayerControl)
#endif