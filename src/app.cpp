#include "app.hpp"
#include "components/bullet.hpp"
#include "components/player_control.hpp"
#include "systems/bullet_system.hpp"
#include "systems/light_particle_system.hpp"
#include "systems/player_control_system.hpp"

//struct TestComponent : public nodec_scene_serialization::BaseSerializableComponent {
//    TestComponent()
//        : BaseSerializableComponent(this) {}
//};
//NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(TestComponent)

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
        using namespace nodec_rendering::components;
        using namespace nodec_rendering::resources;
        using namespace nodec_scene_serialization;
        using namespace nodec_scene_audio::resources;
        using namespace nodec_scene_audio::components;
        using namespace nodec_world;
        using namespace nodec_input::keyboard;
        using namespace nodec_input::mouse;
        using namespace nodec_physics::systems;
        using namespace ::systems;
        using namespace ::components;

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
            //scene_serialization_.register_component<TestComponent>();
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
            auto &editor = app.get_service<SceneEditor>();
            Bullet::setup_editor(editor);
            PlayerControl::setup_editor(editor);
            //editor.inspector_component_registry().register_component<TestComponent>("Test Component",
            //                                                                        [](TestComponent &) {});
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
        //using namespace nodec_scene;
        //using namespace nodec_scene::components;
        //using namespace ::components;

        //auto &scene_registry = world.scene().registry();

        //scene_registry.view<PlayerControl, LocalTransform>().each([&](SceneEntity, PlayerControl&, LocalTransform &player_trfm) {
        //    scene_registry.view<TestComponent, LocalTransform>().each([&](SceneEntity entity, TestComponent &test, LocalTransform &trfm) {
        //        auto dir = player_trfm.position - trfm.position;
        //        auto look_rotation = nodec::math::gfx::look_rotation(dir);

        //        trfm.rotation = look_rotation;
        //        trfm.dirty = true;
        //    });
        //});
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

void nodec_application::on_configure(nodec_application::Application &app) {
    // Make this application instance, and append it to the nodec application.
    app.add_service<HelloNodecGameApplication>(std::make_shared<HelloNodecGameApplication>(app));
}
