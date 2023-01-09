#include "app.hpp"
#include "player_control_system.hpp"
#include "light_particle.hpp"
#include "object_spawn_system.hpp"
#include "bullet_system.hpp"
#include "scene_transition_system.hpp"

class HelloNodecGameApplication {
public:
    HelloNodecGameApplication(nodec_application::Application &app)
        // Get engine services.
        : resources_(app.get_service<nodec_resources::Resources>()),
          scene_loader_(app.get_service<nodec_scene_serialization::SceneLoader>()),
          scene_serialization_(app.get_service<nodec_scene_serialization::SceneSerialization>()) {
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

        logging::InfoStream(__FILE__, __LINE__) << "[HelloNodecGameApplication::HelloNodecGameApplication] >>> Hello :)";

        // --- Get services ---
        auto &world = app.get_service<World>();
        auto &input_devices = app.get_service<InputDevices>();
        auto &screen = app.get_service<Screen>();

        auto keyboard = input_devices.get_available_devices<Keyboard>().front();
        auto mouse = input_devices.get_available_devices<Mouse>().front();

        // --- Setup screen ---
        {
            // screen.set_size({1920, 1080});
            // screen.set_resolution({1920, 1080});

            screen.set_size({1280, 720});
            screen.set_resolution({1280, 720});

            // screen.set_size({ 1368, 800 });
            // screen.set_resolution({ 1368, 800 });

            // screen.set_size({ 1600, 900 });
            // screen.set_resolution({ 1600, 900 });

            screen.set_title("[ Hello Nodec ]");
        }

        {
            world.initialized().connect([&](World &world) { on_initialized(world); });
            world.stepped().connect([&](World &world) { on_stepped(world); });
        }

        {
            player_control_system_ = std::make_shared<PlayerControlSystem>(world, keyboard, mouse, scene_serialization_);
            light_particle = std::make_shared<LightParticle>(world, resources_.registry(), scene_serialization_);
            object_spawn_system_ = std::make_shared<ObjectSpawnSystem>(keyboard, world, scene_serialization_, scene_loader_);
            scene_transition_system_ = std::make_shared<SceneTransitionSystem>(world, scene_serialization_, scene_loader_);
            bullet_system_ = std::make_shared<BulletSystem>(world, scene_serialization_);
        }

        // Set up systems if editor mode enabled.
        {
#ifdef EDITOR_MODE
            using namespace nodec_scene_editor;
            auto &editor = app.get_service<SceneEditor>();

            PlayerControlSystem::setup_editor(editor);
            ObjectSpawnSystem::setup_editor(editor);
            SceneTransitionSystem::setup_editor(editor);
            BulletSystem::setup_editor(editor);
#endif
        }

        //{
        //    auto &input = engine.get_module<Input>();
        //    input.keyboard().key_event().connect([=, &engine](const keyboard::KeyEvent &event) {
        //        // logging::InfoStream(__FILE__, __LINE__) << event;

        //        auto &scene = engine.get_module<Scene>();

        //        if (event.key == keyboard::Key::A && event.type == keyboard::KeyEvent::Type::Release) {
        //            logging::InfoStream(__FILE__, __LINE__) << "AAA";
        //            auto &source = scene.registry().get_component<AudioSource>(audioEntity);

        //            source.is_playing = true;
        //        }

        //        if (event.key == keyboard::Key::D && event.type == keyboard::KeyEvent::Type::Release) {
        //            logging::InfoStream(__FILE__, __LINE__) << "DDD";
        //            auto &source = scene.registry().get_component<AudioSource>(audioEntity);

        //            source.is_playing = false;
        //        }
        //    });
    }

    ~HelloNodecGameApplication() {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[HelloNodecGameApplication::~HelloNodecGameApplication] >>> See you ;)";
    }

private:
    void on_initialized(nodec_world::World &world) {
        using namespace nodec;
        using namespace nodec_scene_serialization;

        logging::InfoStream(__FILE__, __LINE__) << "[HelloNodecGameApplication::on_initialized]";

        // Load the main scene.
        {
            auto main_scene = resources_.registry().get_resource_direct<SerializableSceneGraph>("org.nodec.hello-nodec-game/scenes/main.scene");
            SceneEntityEmplacer{main_scene, world.scene(), entities::null_entity, scene_serialization_}.emplace_all();
        }

        //{
        //    using namespace nodec_physics::components;

        //    auto entt = world.scene().create_entity();

        //    auto &rb = world.scene().registry().emplace_component<RigidBody>(entt).first;

        //    rb.mass = 10;

        //    world.scene().registry().emplace_component<PhysicsShape>(entt);

        //    temp = entt;
        //}
        //{
        //    target_material = resources.registry().get_resource<Material>("models/primitives/Default.material").get();

        //    auto dodon_clip = resources.registry().get_resource<AudioClip>("audios/dodon.wav").get();
        //    auto miku_clip = resources.registry().get_resource<AudioClip>("audios/miku-activated.wav").get();

        //    audioEntity = scene.create_entity("Audio Source Test");
        //    scene.registry().emplace_component<AudioSource>(audioEntity);
        //    auto &source = scene.registry().get_component<AudioSource>(audioEntity);
        //    source.clip = dodon_clip;
        //    // source.clip = miku_clip;
        //    // source.loop = true;
        //    source.is_playing = true;
        //}
    }

    void on_stepped(nodec_world::World &world) {
    }

private:
    // --- Game engine services ---
    nodec_resources::Resources &resources_;
    nodec_scene_serialization::SceneLoader &scene_loader_;
    nodec_scene_serialization::SceneSerialization &scene_serialization_;

    // --- Sub systems ---
    std::shared_ptr<PlayerControlSystem> player_control_system_;
    std::shared_ptr<LightParticle> light_particle;
    std::shared_ptr<ObjectSpawnSystem> object_spawn_system_;
    std::shared_ptr<SceneTransitionSystem> scene_transition_system_;
    std::shared_ptr<BulletSystem> bullet_system_;
};

void nodec_application::on_configure(nodec_application::Application &app) {
    // Make this application instance, and append it to the nodec application.
    app.add_service<HelloNodecGameApplication>(std::make_shared<HelloNodecGameApplication>(app));
}
