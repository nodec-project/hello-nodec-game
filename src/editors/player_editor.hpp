#ifndef HELLO_NODEC_GAME__EDITORS__PLAYER_EDITOR_HPP_
#define HELLO_NODEC_GAME__EDITORS__PLAYER_EDITOR_HPP_

#include <imgui.h>

#include <nodec_scene_editor/component_editor.hpp>

#include "../components/player.hpp"

namespace hello_nodec_game {
namespace editors {

class PlayerControlEditor
    : public nodec_scene_editor::BasicComponentEditor<components::PlayerControl> {
public:
    void on_inspector_gui(components::PlayerControl &control,
                          const nodec_scene_editor::InspectorGuiContext &context) override {
        ImGui::DragFloat("Speed", &control.speed);
    }
};

} // namespace editors
} // namespace hello_nodec_game
#endif