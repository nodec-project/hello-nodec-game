#ifndef COMPONENTS__BULLET_HPP_
#define COMPONENTS__BULLET_HPP_

#include <nodec_scene_serialization/serializable_component.hpp>

#ifdef EDITOR_MODE
#    include <nodec_scene_editor/scene_editor.hpp>
#endif

namespace components {

class Bullet : public nodec_scene_serialization::BaseSerializableComponent {
public:
    Bullet()
        : BaseSerializableComponent(this) {}

    float lifetime{10.f};

    template<class Archive>
    void serialize(Archive &archive) {
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
};

} // namespace components

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(components::Bullet)
#endif