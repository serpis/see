#include <GL/glew.h>
#include <GL/glfw.h>

#include "engine.hpp"
#include "fswatch.hpp"
#include "renderl.hpp"
#include "renderm.hpp"
#include "renderh.hpp"
#include "audiol.hpp"
#include "resources.hpp"
#include "lua.hpp"

double time_now;
int window_width = 640;
int window_height = 480;
float window_aspect = (float)window_width/(float)window_height;
bool window_fullscreen = false;
bool running = true;

engine_t *engine_t::instance = NULL;

static void key_callback(int key, int action)
{
    if (action == GLFW_PRESS)
    {
        engine_t::instance->input_system.key_down(key);

        if (key == 'R')
        {
            lua_load_file("data/scripts/init.lua");
        }
    }
    else
    {
        engine_t::instance->input_system.key_up(key);
    }
}

static void button_callback(int button, int action)
{
    if (action == GLFW_PRESS)
    {
        engine_t::instance->input_system.button_down(button);
    }
    else
    {
        engine_t::instance->input_system.button_up(button);
    }
}

static void mouse_pos_callback(int x, int y)
{
    engine_t::instance->input_system.mouse_move(x, y);
}

static int window_close_callback()
{
    running = false;

    return GL_TRUE;
}

engine_t::engine_t()
{
    assert(engine_t::instance == NULL);
    if (engine_t::instance == NULL)
    {
        engine_t::instance = this;
    }
}

void engine_t::init()
{
    lua_init();
    lua_load_file("data/scripts/init.lua");

    glfwInit();

    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
    glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    assert(glfwOpenWindow(window_width, window_height, 8, 8, 8, 8, 24, 0, window_fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW) == GL_TRUE);

    assert(glfwGetWindowParam(GLFW_OPENGL_VERSION_MAJOR) == 3);
    assert(glfwGetWindowParam(GLFW_OPENGL_VERSION_MINOR) == 2);
    assert(glfwGetWindowParam(GLFW_OPENGL_FORWARD_COMPAT) == GL_TRUE);
    assert(glfwGetWindowParam(GLFW_OPENGL_PROFILE) == GLFW_OPENGL_CORE_PROFILE);

    // without the following line glGenFramebuffers and others are NULL on my computer:
    //Experimental Drivers
    //
    //GLEW obtains information on the supported extensions from the graphics driver. Experimental or pre-release drivers, however, might not report every available extension through the standard mechanism, in which case GLEW will report it unsupported. To circumvent this situation, the glewExperimental global switch can be turned on by setting it to GL_TRUE before calling glewInit(), which ensures that all extensions with valid entry points will be exposed.
    glewExperimental = GL_TRUE;
    assert(glewInit() == GLEW_OK);
    assert(GLEW_VERSION_3_2);
    render_print_errors("glewInit()");

    glfwSetKeyCallback(key_callback);
    glfwSetMouseButtonCallback(button_callback);
    glfwSetMousePosCallback(mouse_pos_callback);
    glfwSetWindowCloseCallback(window_close_callback);

    fswatch_init();
    renderl_init();
    //renderm_init();
    renderh_init();
    audiol_init();
    resource_init();
    render_system.init();
    input_system.init();
    audio_system.init();
    physics_system.init();
}

void engine_t::run()
{
    audio_system.unpause();

    double next_update_time = glfwGetTime();
    const int ticks_per_second = 60;

    double next_frame_time = 1.0;
    int frames = 0;

	// here we go!
	while (running && !input_system.keys[GLFW_KEY_ESC])
	{
		static double last_frame = glfwGetTime();
		double this_frame = glfwGetTime();
        time_now = this_frame;

		while (next_update_time < this_frame)
		{
			float t = next_update_time;
            float dt = 1.0f / ticks_per_second;

			next_update_time += 1.0/ticks_per_second;

            input_system.update(dt);
            physics_system.update(dt);
			//update_world(t, dt);
		}

        audio_system.update(this_frame - last_frame);
        render_system.update(this_frame - last_frame);

		last_frame = this_frame;

        glfwSwapBuffers();

		render_print_errors("end of main loop");

		if (next_frame_time <= this_frame)
		{
            char title[] = "SEE";
			char s[64];
			sprintf(s, "%s - fps: %d", title, frames);
            glfwSetWindowTitle(s);
			frames = 0;
			next_frame_time += 1.0;
		}
		frames += 1;

        fswatch_poll();

        //glfwSleep(0.01);
	}

    glfwCloseWindow();
}

/*
    int window_width = 640;
    int window_height = 480;
    int fullscreen = 0;

    assert(SDL_Init(SDL_INIT_VIDEO) == 0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_Surface* drawContext = SDL_SetVideoMode(window_width, window_height, 32, SDL_OPENGL | (fullscreen ? SDL_FULLSCREEN : 0));

    SDL_EnableUNICODE(1);

	unsigned int next_frame_time = 1000;
	unsigned int frames = 0;

	const int ticks_per_second = 100;

	unsigned int next_update_time = 1000/ticks_per_second;

    bool keys[65536] = { 0 };

	// here we go!
	bool running = true;
	while (running && !keys[SDLK_ESCAPE])
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
            switch (event.type)
            {
    watch.callback = callback;
    watch.user_data = user_data;
                case SDL_KEYDOWN:
                    keys[event.key.keysym.sym] = true;
                    //game_key_down(event.key.keysym.sym, event.key.keysym.unicode);
                    break;
                case SDL_KEYUP:
                    keys[event.key.keysym.sym] = false;
                    //game_key_up(event.key.keysym.sym, event.key.keysym.unicode);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    //game_mouse_down(event.button.x, event.button.y, event.button.button);
                    break;
                case SDL_MOUSEBUTTONUP:
                    //game_mouse_up(event.button.x, event.button.y, event.button.button);
                    break;
                case SDL_MOUSEMOTION:
                    //game_mouse_move(event.motion.x, event.motion.y);
                    break;
				case SDL_QUIT:
					running = false;
					break;
				default:
					break;
			}
		}

		static unsigned int last_frame = SDL_GetTicks();
		unsigned int this_frame = SDL_GetTicks();

		while (next_update_time < this_frame)
		{
			float t = next_update_time / 1000.0f;
			float dt = (1000/ticks_per_second) / 1000.0f;

			next_update_time += 1000/ticks_per_second;

			//update_world(t, dt);
		}
		//render();

		last_frame = this_frame;

		SDL_GL_SwapBuffers();

		render_print_errors("end of main loop");

		if (next_frame_time <= this_frame)
		{
            char title[] = "SEE";
			char s[64];
			sprintf(s, "%s - fps: %d", title, frames);
			SDL_WM_SetCaption(s, s);
			frames = 0;
			next_frame_time += 1000;
		}
		frames += 1;

		SDL_Delay(10);
	}
	
	// deinit subsystems in reverse order
	// TODO: take more care deiniting
	SDL_Quit();
    */

