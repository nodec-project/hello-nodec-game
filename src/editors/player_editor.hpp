#ifndef APP__EDITORS__PLAYER_EDITOR_HPP_
#define APP__EDITORS__PLAYER_EDITOR_HPP_

#include <imgui.h>

#include <nodec_scene_editor/component_editor.hpp>

#include "../components/player.hpp"

namespace app {
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
} // namespace app
#endif