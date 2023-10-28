#include <nodec_application/application.hpp>
#include <nodec_input/input_devices.hpp>
#include <nodec_physics/systems/physics_system.hpp>
#include <nodec_resources/resources.hpp>
#include <nodec_scene_editor/scene_editor.hpp>
#include <nodec_scene_serialization/entity_loader.hpp>
#include <nodec_screen/screen.hpp>
#include <nodec_world/world.hpp>

#include "components/bullet.hpp"
#include "components/player.hpp"
#include "systems/bullet_system.hpp"
#include "systems/light_particle_system.hpp"
#include "systems/player_control_system.hpp"

#ifdef EDITOR_MODE
#    include "editors/player_editor.hpp"
#endif

namespace hello_nodec_game {

class HelloNodecGameApplication {
public:
    HelloNodecGameApplication(nodec_application::Application &app)
        // Get engine services.
        : world_(app.get_service<nodec_world::World>()),
          screen_(app.get_service<nodec_screen::Screen>()),
          physics_system_(app.get_service<nodec_physics::systems::PhysicsSystem>()),
          resources_(app.get_service<nodec_resources::Resources>()),
          entity_loader_(app.get_service<nodec_scene_serialization::EntityLoader>()),
          scene_serialization_(app.get_service<nodec_scene_serialization::SceneSerialization>()),
          input_devices_(app.get_service<nodec_input::InputDevices>()) {
        using namespace nodec;
        using namespace nodec_input;
        using namespace nodec_scene;
        using namespace nodec_scene::components;
        using namespace nodec_screen;
        using namespace nodec_resources;
        using namespace nodec_scene_serialization;
        using namespace nodec_world;
        using namespace nodec_input::keyboard;
        using namespace nodec_input::mouse;
        using namespace nodec_physics::systems;
        using namespace systems;
        using namespace components;

        logging::InfoStream(__FILE__, __LINE__) << "[HelloNodecGameApplication::HelloNodecGameApplication] >>> Hello :)";

        // --- Get input devices ---
        auto keyboard = input_devices_.get_available_devices<Keyboard>().front();
        auto mouse = input_devices_.get_available_devices<Mouse>().front();

        // --- Setup screen ---
        {
            // screen.set_size({1920, 1080});
            // screen.set_resolution({1920, 1080});

            screen_.set_size({1280, 720});
            screen_.set_resolution({1280, 720});

            // screen.set_size({ 1368, 800 });
            // screen.set_resolution({ 1368, 800 });

            // screen.set_size({ 1600, 900 });
            // screen.set_resolution({ 1600, 900 });

            screen_.set_title("[ Hello Nodec ]");
        }

        {
            world_.initialized().connect([&](World &world) { on_initialized(world); });
            world_.stepped().connect([&](World &world) { on_stepped(world); });
        }

        // Register components in serialization.
        {
            scene_serialization_.register_component<Bullet>();
            scene_serialization_.register_component<PlayerControl>();
        }

        {
            light_particle = std::make_unique<LightParticleSystem>(world_, resources_.registry(), scene_serialization_);
            bullet_system_ = std::make_unique<BulletSystem>(world_, physics_system_);
            player_control_system_ = std::make_unique<PlayerControlSystem>(world_, resources_, keyboard, mouse, scene_serialization_);
        }

#ifdef EDITOR_MODE
        // Set up systems if editor mode enabled.
        {
            using namespace nodec_scene_editor;
            using namespace editors;
            auto &editor = app.get_service<SceneEditor>();

            editor.component_registry().register_component<PlayerControl, PlayerControlEditor>("Player Control");
            editor.component_registry().register_component<Bullet>("Bullet");
        }
#endif
    }

    ~HelloNodecGameApplication() {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[HelloNodecGameApplication::~HelloNodecGameApplication] >>> See you ;)";
    }

private:
    void on_initialized(nodec_world::World &world) {
        using namespace nodec;
        using namespace nodec_scene_serialization;
        using namespace nodec_input;
        using namespace nodec_input::keyboard;
        using namespace nodec_input::mouse;

        logging::InfoStream(__FILE__, __LINE__) << "[HelloNodecGameApplication::on_initialized]";

        // Load the main scene.
        {
            entity_loader_.load_async("org.nodec.hello-nodec-game/prefabs/main-scene.entity", world.scene().create_entity());
        }
    }

    void on_stepped(nodec_world::World &world) {
    }

private:
    // --- Game engine services ---
    nodec_world::World &world_;
    nodec_screen::Screen &screen_;
    nodec_physics::systems::PhysicsSystem &physics_system_;
    nodec_resources::Resources &resources_;
    nodec_scene_serialization::EntityLoader &entity_loader_;
    nodec_scene_serialization::SceneSerialization &scene_serialization_;
    nodec_input::InputDevices &input_devices_;

    // --- Sub systems ---
    std::unique_ptr<systems::PlayerControlSystem> player_control_system_;
    std::unique_ptr<systems::LightParticleSystem> light_particle;
    std::unique_ptr<systems::BulletSystem> bullet_system_;
};

} // namespace hello_nodec_game

void nodec_application::on_configure(nodec_application::Application &app) {
    using namespace hello_nodec_game;

    // Make this application instance, and append it to the nodec application.
    app.add_service<HelloNodecGameApplication>(std::make_shared<HelloNodecGameApplication>(app));
}
