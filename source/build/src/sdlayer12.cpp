// SDL 1.2 compatibility.

#include <SDL/SDL_events.h>

#ifdef __PSP2__
	
#define NO_MAPPING -1
	
enum {
    VITA_PAD_TRIANGLE   = 0,
    VITA_PAD_CIRCLE     = 1,
    VITA_PAD_CROSS      = 2,
    VITA_PAD_SQUARE     = 3,
    VITA_PAD_L          = 4,
    VITA_PAD_R          = 5,
    VITA_PAD_DOWN       = 6,
    VITA_PAD_LEFT       = 7,
    VITA_PAD_UP         = 8,
    VITA_PAD_RIGHT      = 9,
    VITA_PAD_SELECT     = 10,
    VITA_PAD_START      = 11,
    VITA_LJOY_LEFT      = 12, // handled in vita_handle_analog_sticks()
    VITA_LJOY_RIGHT     = 13,
    VITA_LJOY_UP        = 14,
    VITA_LJOY_DOWN      = 15,
    VITA_RJOY_LEFT      = 16,
    VITA_RJOY_RIGHT     = 17,
    VITA_RJOY_UP        = 18,
    VITA_RJOY_DOWN      = 19,
    VITA_NUM_BUTTONS    = 20
};
	
static SDLKey map_vita_button_to_sdlk[VITA_NUM_BUTTONS] = 
{ 
    SDLK_w,         // VITA_PAD_TRIANGLE
	SDLK_d,         // VITA_PAD_CIRCLE
    SDLK_RETURN,    // VITA_PAD_CROSS
    SDLK_a,         // VITA_PAD_SQUARE
    SDLK_q,         // VITA_PAD_L
    SDLK_r,         // VITA_PAD_R
    SDLK_DOWN,      // VITA_PAD_DOWN
    SDLK_LEFT,      // VITA_PAD_LEFT
    SDLK_UP,        // VITA_PAD_UP
    SDLK_RIGHT,     // VITA_PAD_RIGHT
    SDLK_c,         // VITA_SELECT
    SDLK_ESCAPE,    // VITA_START
    SDLK_v,         // VITA_LJOY_LEFT
    SDLK_n,         // VITA_LJOY_RIGHT
    SDLK_KP8,       // VITA_LJOY_UP
    SDLK_KP2,       // VITA_LJOY_DOWN
    SDLK_KP4,       // VITA_RJOY_LEFT
    SDLK_KP6,       // VITA_RJOY_RIGHT
    SDLK_KP7,       // VITA_RJOY_UP
    SDLK_KP1        // VITA_RJOY_DOWN 
};
#include "osd.h"
extern osdmain_t *osd;
static int pressed_buttons[VITA_NUM_BUTTONS] = { 0 };
extern SDL_Joystick *joydev;
void vita_button_to_sdlkey_event(int vita_button, SDL_Event *event, uint32_t event_type);
void vita_preprocess_event(SDL_Event *event);
void vita_create_and_push_sdlkey_event(uint32_t event_type, SDLKey key);
void vita_handle_analog_sticks(void);
#endif

#ifdef _WIN32
HWND win_gethwnd(void)
{
    struct SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);

    if (SDL_GetWMInfo(&wmInfo) != 1)
    {
        initprintf("win_gethwnd: SDL_GetWindowWMInfo() failed: %s\n", SDL_GetError());
        return 0;
    }

    return wmInfo.window;
}
#endif

int32_t videoSetVsync(int32_t newSync)
{
    if (vsync_renderlayer == newSync)
        return newSync;

    vsync_renderlayer = newSync;

    videoResetMode();
    if (videoSetGameMode(fullscreen, xres, yres, bpp, upscalefactor))
        OSD_Printf("restartvid: Reset failed...\n");

    return newSync;
}

int32_t sdlayer_checkversion(void)
{
    const SDL_version *linked = SDL_Linked_Version();
    SDL_version compiled;

    SDL_VERSION(&compiled);

    initprintf("Initializing SDL system interface "
               "(compiled against SDL version %d.%d.%d, found version %d.%d.%d)\n",
               compiled.major, compiled.minor, compiled.patch, linked->major, linked->minor, linked->patch);

    if (SDL_VERSIONNUM(linked->major, linked->minor, linked->patch) < SDL_REQUIREDVERSION)
    {
        /*reject running under SDL versions older than what is stated in sdl_inc.h */
        initprintf("You need at least v%d.%d.%d of SDL to run this game\n", SDL_MIN_X, SDL_MIN_Y, SDL_MIN_Z);
        return -1;
    }

    return 0;
}

//
// initsystem() -- init SDL systems
//
int32_t initsystem(void)
{
#if defined NOSDLPARACHUTE
    const int sdlinitflags = SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE;
#else
    const int sdlinitflags = SDL_INIT_VIDEO;
#endif

    mutex_init(&m_initprintf);

#ifdef _WIN32
    win_init();
#endif

    if (sdlayer_checkversion())
        return -1;

    if (SDL_Init(sdlinitflags))
    {
        initprintf("Initialization failed! (%s)\nNon-interactive mode enabled\n", SDL_GetError());
        novideo = 1;
#ifdef USE_OPENGL
        nogl = 1;
#endif
    }

    atexit(uninitsystem);

    frameplace = 0;
    lockcount = 0;


    if (!novideo)
    {
        char drvname[32];

#ifdef USE_OPENGL
        if (SDL_GL_LoadLibrary(0))
        {
            initprintf("Failed loading OpenGL driver. GL modes will be unavailable.\n");
            nogl = 1;
        }
#ifdef POLYMER
        if (loadglulibrary(getenv("BUILD_GLULIB")))
        {
            initprintf("Failed loading GLU.  GL modes will be unavailable.\n");
            nogl = 1;
        }
#endif
#endif

        if (SDL_VideoDriverName(drvname, 32))
            initprintf("Using \"%s\" video driver\n", drvname);

        wm_setapptitle(apptitle);
    }

#if defined GEKKO
    SDL_ShowCursor(SDL_DISABLE);
#endif

    return 0;
}

#ifdef GEKKO
static const char *joynames[3][15] = {
        {
            "Left Stick X", "Left Stick Y", "Right Stick X", "Right Stick Y", "Axis 5", "Axis 6", "Axis 7", "Axis 8",
            "Axis 9", "Axis 10", "Axis 11", "Axis 12", "Axis 13", "Axis 14", "Axis 15",
        },
        {
            "Button A", "Button B", "Button 1", "Button 2", "Button -", "Button +", "Button HOME", "Button Z", "Button C",
            "Button X", "Button Y", "Trigger L", "Trigger R", "Trigger ZL", "Trigger ZR",
        },
        {
            "D-Pad Up", "D-Pad Right", "D-Pad Down", "D-Pad Left", "Hat 5", "Hat 6", "Hat 7", "Hat 8", "Hat 9", "Hat 10",
            "Hat 11", "Hat 12", "Hat 13", "Hat 14", "Hat 15",
        }
};
const char *joyGetName(int32_t what, int32_t num)
{
    switch (what)
    {
        case 0:	// axis
            if ((unsigned)num > (unsigned)joystick.numAxes) return NULL;
            return joynames[0][num];

        case 1: // button
            if ((unsigned)num > (unsigned)joystick.numButtons) return NULL;
            return joynames[1][num];

        case 2: // hat
            if ((unsigned)num > (unsigned)joystick.numHats) return NULL;
            return joynames[2][num];

        default:
            return NULL;
    }
}
#endif

//
// grabmouse_low() -- show/hide mouse cursor, lower level (doesn't check state).
//                    furthermore return 0 if successful.
//

static inline char grabmouse_low(char a)
{
#if !defined GEKKO
    SDL_ShowCursor(a ? SDL_DISABLE : SDL_ENABLE);
    return (SDL_WM_GrabInput(a ? SDL_GRAB_ON : SDL_GRAB_OFF) != (a ? SDL_GRAB_ON : SDL_GRAB_OFF));
#else
    UNREFERENCED_PARAMETER(a);
    return 0;
#endif
}

void videoGetModes(void)
{
    int32_t i, maxx = 0, maxy = 0;
    int32_t j;
    static int32_t cdepths[] = { 8,
#ifdef USE_OPENGL
                                 16, 24, 32,
#endif
                                 0 };
    SDL_Rect **modes;
    SDL_PixelFormat pf;

    pf.palette = NULL;
    pf.BitsPerPixel = 8;
    pf.BytesPerPixel = 1;

    if (modeschecked || novideo)
        return;

    validmodecnt = 0;
    //    initprintf("Detecting video modes:\n");

    // do fullscreen modes first
    for (j = 0; cdepths[j]; j++)
    {
#ifdef USE_OPENGL
        if (nogl && cdepths[j] > 8)
            continue;
#endif
        pf.BitsPerPixel = cdepths[j];
        pf.BytesPerPixel = cdepths[j] >> 3;

        // We convert paletted contents to non-paletted
        modes = SDL_ListModes((cdepths[j] == 8) ? NULL : &pf, SURFACE_FLAGS | SDL_FULLSCREEN);

        if (modes == (SDL_Rect **)0)
        {
            if (cdepths[j] > 8)
                cdepths[j] = -1;
            continue;
        }

        if (modes == (SDL_Rect **)-1)
        {
            for (i = 0; g_defaultVideoModes[i].x; i++)
                SDL_ADDMODE(g_defaultVideoModes[i].x, g_defaultVideoModes[i].y, cdepths[j], 1);
        }
        else
        {
            for (i = 0; modes[i]; i++)
            {
                if (!SDL_CHECKMODE(modes[i]->w, modes[i]->h))
                    continue;

                SDL_ADDMODE(modes[i]->w, modes[i]->h, cdepths[j], 1);

                if ((modes[i]->w > maxx) || (modes[i]->h > maxy))
                {
                    maxx = modes[i]->w;
                    maxy = modes[i]->h;
                }
            }
        }
    }

    SDL_CHECKFSMODES(maxx, maxy);

    // add windowed modes next
    for (j = 0; cdepths[j]; j++)
    {
#ifdef USE_OPENGL
        if (nogl && cdepths[j] > 8)
            continue;
#endif
        if (cdepths[j] < 0)
            continue;

        for (i = 0; g_defaultVideoModes[i].x; i++)
        {
            auto &mode = g_defaultVideoModes[i];
            if (mode.x > maxx || mode.y > maxy || !SDL_CHECKMODE(mode.x, mode.y))
                continue;

            SDL_ADDMODE(mode.x, mode.y, cdepths[j], 0);
        }
    }

    qsort((void *)validmode, validmodecnt, sizeof(struct validmode_t), &sortmodes);

    modeschecked = 1;
}

//
// setvideomode() -- set SDL video mode
//
int32_t videoSetMode(int32_t x, int32_t y, int32_t c, int32_t fs)
{
    int32_t regrab = 0, ret;
#ifdef USE_OPENGL
    static int32_t ovsync = 1;
#endif

    ret = setvideomode_sdlcommon(&x, &y, c, fs, &regrab);
    if (ret != 1)
    {
        if (ret == 0)
        {
            setvideomode_sdlcommonpost(x, y, c, fs, regrab);
        }
        return ret;
    }

    // restore gamma before we change video modes if it was changed
    if (sdl_surface && gammabrightness)
    {
        SDL_SetGammaRamp(sysgamma[0], sysgamma[1], sysgamma[2]);
        gammabrightness = 0;  // redetect on next mode switch
    }

    // deinit
    destroy_window_resources();

    initprintf("Setting video mode %dx%d (%d-bpp %s)\n", x, y, c, ((fs & 1) ? "fullscreen" : "windowed"));

#ifdef USE_OPENGL
    if (c > 8 || !nogl)
    {
        int32_t i, j, multisamplecheck = (glmultisample > 0);

        if (nogl)
            return -1;

# ifdef _WIN32
        win_setvideomode(c);
# endif

        struct glattribs
        {
            SDL_GLattr attr;
            int32_t value;
        } sdlayer_gl_attributes [] =
        {
            { SDL_GL_DOUBLEBUFFER, 1 },
            { SDL_GL_MULTISAMPLEBUFFERS, glmultisample > 0 },
            { SDL_GL_MULTISAMPLESAMPLES, glmultisample },
            { SDL_GL_STENCIL_SIZE, 8 },
            { SDL_GL_ACCELERATED_VISUAL, 1 },
            { SDL_GL_SWAP_CONTROL, vsync_renderlayer },
        };

        do
        {
            SDL_GL_ATTRIBUTES(i, sdlayer_gl_attributes);

            /* HACK: changing SDL GL attribs only works before surface creation,
               so we have to create a new surface in a different format first
               to force the surface we WANT to be recreated instead of reused. */
            if (vsync_renderlayer != ovsync)
            {
                if (sdl_surface)
                {
                    SDL_FreeSurface(sdl_surface);
                    sdl_surface =
                    SDL_SetVideoMode(1, 1, 8, SDL_NOFRAME | SURFACE_FLAGS | ((fs & 1) ? SDL_FULLSCREEN : 0));
                    SDL_FreeSurface(sdl_surface);
                }
                ovsync = vsync_renderlayer;
            }
            sdl_surface = SDL_SetVideoMode(x, y, c, SDL_OPENGL | ((fs & 1) ? SDL_FULLSCREEN : 0));
            if (!sdl_surface)
            {
                if (multisamplecheck)
                {
                    initprintf("Multisample mode not possible. Retrying without multisampling.\n");
                    glmultisample = 0;
                    continue;
                }
                initprintf("Unable to set video mode!\n");
                return -1;
            }
        } while (multisamplecheck--);

        gladLoadGLLoader(SDL_GL_GetProcAddress);
    }
    else
#endif  // defined USE_OPENGL
    {
        // We convert paletted contents to non-paletted
        sdl_surface = SDL_SetVideoMode(x, y, 0, SURFACE_FLAGS | ((fs & 1) ? SDL_FULLSCREEN : 0));

        if (!sdl_surface)
        {
            initprintf("Unable to set video mode!\n");
            return -1;
        }
    }

    setvideomode_sdlcommonpost(x, y, c, fs, regrab);

    return 0;
}

//
// showframe() -- update the display
//
void videoShowFrame(int32_t w)
{
    UNREFERENCED_PARAMETER(w);

#ifdef USE_OPENGL
    if (!nogl)
    {
        if (bpp > 8)
        {
            if (palfadedelta)
                fullscreen_tint_gl(palfadergb.r, palfadergb.g, palfadergb.b, palfadedelta);
            fullscreen_tint_gl_blood();

        }
        else
        {
            glsurface_blitBuffer();
        }

        SDL_GL_SwapBuffers();
        return;
    }
#endif

    if (offscreenrendering) return;
#ifdef __PSP2__
	memcpy(vita2d_texture_get_datap(gpu_texture),vita2d_texture_get_datap(fb_texture),vita2d_texture_get_stride(gpu_texture)*vita2d_texture_get_height(gpu_texture));
    vita2d_start_drawing();
    vita2d_draw_texture_scale(gpu_texture, 0, 0, 1.5f, 1.1333f);
    vita2d_end_drawing();
    vita2d_wait_rendering_done();
	vita2d_swap_buffers();
#else
    if (lockcount)
    {
        printf("Frame still locked %d times when showframe() called.\n", lockcount);
        while (lockcount) videoEndDrawing();
    }

    if (SDL_MUSTLOCK(sdl_surface)) SDL_LockSurface(sdl_surface);
    softsurface_blitBuffer((uint32_t*) sdl_surface->pixels, sdl_surface->format->BitsPerPixel);
    if (SDL_MUSTLOCK(sdl_surface)) SDL_UnlockSurface(sdl_surface);

    SDL_Flip(sdl_surface);
#endif
}

#ifdef __PSP2__
	void vita_button_to_sdlkey_event(int vita_button, SDL_Event *event, uint32_t event_type) 
	{
	    event->type = event->key.type = event_type;
	    event->key.keysym.sym = map_vita_button_to_sdlk[vita_button];
	    event->key.keysym.unicode = event->key.keysym.sym;
	    event->key.keysym.mod = 0;
	}
	
	void vita_create_and_push_sdlkey_event(uint32_t event_type, SDLKey key) 
	{
	    SDL_Event event;
	    event.type = event.key.type = event_type;
	    event.key.keysym.sym = key;
	    event.key.keysym.unicode = event.key.keysym.sym;
	    event.key.keysym.mod = 0;
	    SDL_PushEvent(&event);
	}
	
	void vita_preprocess_event(SDL_Event *event) 
	{
	    switch (event->type) {
	        case SDL_JOYBUTTONDOWN:
	            if (event->jbutton.which==0) { // Only Joystick 0 controls the game
					vita_button_to_sdlkey_event(event->jbutton.button, event, SDL_KEYDOWN);
	            }
	            break;
	        case SDL_JOYBUTTONUP:
	            if (event->jbutton.which==0) {// Only Joystick 0 controls the game
	                vita_button_to_sdlkey_event(event->jbutton.button, event, SDL_KEYUP);
	            }
	            break;
	        default:
	            break;
	    }
	}
	
	void vita_handle_analog_sticks(void)
	{
	    // joystick must be open to handle it here
	    if (!joydev) return;
	    int up = 0;
	    int down = 0;
	    int left = 0;
	    int right = 0;
	    float x, y;
	    float joy_dead_zone_squared = 10240.0*10240.0;
	    float slope = 0.414214f; // tangent of 22.5 degrees for size of angular zones
	
	    // map left stick to run
	    x = SDL_JoystickGetAxis(joydev, 0);
	    y = -1 * SDL_JoystickGetAxis(joydev, 1);
	
	    if ((x * x + y * y) >= joy_dead_zone_squared) {
	        // upper right quadrant
	        if (y > 0 && x > 0)
	        {
	            if (y > slope * x)
	                up = 1;
	            if (x > slope * y)
	                right = 1;
	        }
	        // upper left quadrant
	        else if (y > 0 && x <= 0)
	        {
	            if (y > slope * (-x))
	                up = 1;
	            if ((-x) > slope * y)
	                left = 1;
	        }
	        // lower right quadrant
	        else if (y <= 0 && x > 0)
	        {
	            if ((-y) > slope * x)
	                down = 1;
	            if (x > slope * (-y))
	                right = 1;
	        }
	        // lower left quadrant
	        else if (y <= 0 && x <= 0)
	        {
	            if ((-y) > slope * (-x))
	                down = 1;
	            if ((-x) > slope * (-y))
	                left = 1;
	        }
	    }
	    if (!pressed_buttons[VITA_LJOY_UP] && up) {
	        vita_create_and_push_sdlkey_event(SDL_KEYDOWN, map_vita_button_to_sdlk[VITA_LJOY_UP]);
	        pressed_buttons[VITA_LJOY_UP] = 1;
	    } else if (pressed_buttons[VITA_LJOY_UP] && !up) {
	        vita_create_and_push_sdlkey_event(SDL_KEYUP, map_vita_button_to_sdlk[VITA_LJOY_UP]);
	        pressed_buttons[VITA_LJOY_UP] = 0;
	    }
	
	    if (!pressed_buttons[VITA_LJOY_DOWN] && down) {
	        vita_create_and_push_sdlkey_event(SDL_KEYDOWN, map_vita_button_to_sdlk[VITA_LJOY_DOWN]);
	        pressed_buttons[VITA_LJOY_DOWN] = 1;
	    } else if (pressed_buttons[VITA_LJOY_DOWN] && !down) {
	        vita_create_and_push_sdlkey_event(SDL_KEYUP, map_vita_button_to_sdlk[VITA_LJOY_DOWN]);
	        pressed_buttons[VITA_LJOY_DOWN] = 0;
	    }
	    
	    if (!pressed_buttons[VITA_LJOY_LEFT] && left) {
	        vita_create_and_push_sdlkey_event(SDL_KEYDOWN, map_vita_button_to_sdlk[VITA_LJOY_LEFT]);
	        pressed_buttons[VITA_LJOY_LEFT] = 1;
	    } else if (pressed_buttons[VITA_LJOY_LEFT] && !left) {
	        vita_create_and_push_sdlkey_event(SDL_KEYUP, map_vita_button_to_sdlk[VITA_LJOY_LEFT]);
	        pressed_buttons[VITA_LJOY_LEFT] = 0;
	    }
	
	    if (!pressed_buttons[VITA_LJOY_RIGHT] && right) {
	        vita_create_and_push_sdlkey_event(SDL_KEYDOWN, map_vita_button_to_sdlk[VITA_LJOY_RIGHT]);
	        pressed_buttons[VITA_LJOY_RIGHT] = 1;
	    } else if (pressed_buttons[VITA_LJOY_RIGHT] && !right) {
	        vita_create_and_push_sdlkey_event(SDL_KEYUP, map_vita_button_to_sdlk[VITA_LJOY_RIGHT]);
	        pressed_buttons[VITA_LJOY_RIGHT] = 0;
	    }
	
	    // map right stick to camera
	    x = SDL_JoystickGetAxis(joydev, 2);
	    y = -1 * SDL_JoystickGetAxis(joydev, 3);
	
	    up = 0;
	    down = 0;
	    left = 0;
	    right = 0;
	
	    if ((x * x + y * y) >= joy_dead_zone_squared) {
	
	        // upper right quadrant
	        if (y > 0 && x > 0)
	        {
	            if (y > slope * x)
	                up = 1;
	            if (x > slope * y)
	                right = 1;
	        }
	        // upper left quadrant
	        else if (y > 0 && x <= 0)
	        {
	            if (y > slope * (-x))
	                up = 1;
	            if ((-x) > slope * y)
	                left = 1;
	        }
	        // lower right quadrant
	        else if (y <= 0 && x > 0)
	        {
	            if ((-y) > slope * x)
	                down = 1;
	            if (x > slope * (-y))
	                right = 1;
	        }
	        // lower left quadrant
	        else if (y <= 0 && x <= 0)
	        {
	            if ((-y) > slope * (-x))
	                down = 1;
	            if ((-x) > slope * (-y))
	                left = 1;
	        }
	    }
	    if (!pressed_buttons[VITA_RJOY_UP] && up) {
	        vita_create_and_push_sdlkey_event(SDL_KEYDOWN, map_vita_button_to_sdlk[VITA_RJOY_UP]);
	        pressed_buttons[VITA_RJOY_UP] = 1;
	    } else if (pressed_buttons[VITA_RJOY_UP] && !up) {
	        vita_create_and_push_sdlkey_event(SDL_KEYUP, map_vita_button_to_sdlk[VITA_RJOY_UP]);
	        pressed_buttons[VITA_RJOY_UP] = 0;
	    }
	
	    if (!pressed_buttons[VITA_RJOY_DOWN] && down) {
	        vita_create_and_push_sdlkey_event(SDL_KEYDOWN, map_vita_button_to_sdlk[VITA_RJOY_DOWN]);
	        pressed_buttons[VITA_RJOY_DOWN] = 1;
	    } else if (pressed_buttons[VITA_RJOY_DOWN] && !down) {
	        vita_create_and_push_sdlkey_event(SDL_KEYUP, map_vita_button_to_sdlk[VITA_RJOY_DOWN]);
	        pressed_buttons[VITA_RJOY_DOWN] = 0;
	    }
	    
	    if (!pressed_buttons[VITA_RJOY_LEFT] && left) {
	        vita_create_and_push_sdlkey_event(SDL_KEYDOWN, map_vita_button_to_sdlk[VITA_RJOY_LEFT]);
	        pressed_buttons[VITA_RJOY_LEFT] = 1;
	    } else if (pressed_buttons[VITA_RJOY_LEFT] && !left) {
	        vita_create_and_push_sdlkey_event(SDL_KEYUP, map_vita_button_to_sdlk[VITA_RJOY_LEFT]);
	        pressed_buttons[VITA_RJOY_LEFT] = 0;
	    }
	
	    if (!pressed_buttons[VITA_RJOY_RIGHT] && right) {
	        vita_create_and_push_sdlkey_event(SDL_KEYDOWN, map_vita_button_to_sdlk[VITA_RJOY_RIGHT]);
	        pressed_buttons[VITA_RJOY_RIGHT] = 1;
	    } else if (pressed_buttons[VITA_RJOY_RIGHT] && !right) {
	        vita_create_and_push_sdlkey_event(SDL_KEYUP, map_vita_button_to_sdlk[VITA_RJOY_RIGHT]);
	        pressed_buttons[VITA_RJOY_RIGHT] = 0;
	    }
	}
	#endif

// SDL 1.2 specific event handling
int32_t handleevents_pollsdl(void)
{
    int32_t code, rv = 0, j;
    SDL_Event ev;

	#ifdef __PSP2__
	    vita_handle_analog_sticks();
	#endif

    while (SDL_PollEvent(&ev))
    {
	#ifdef __PSP2__
	        vita_preprocess_event(&ev);
	#endif
        switch (ev.type)
        {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                code = keytranslation[ev.key.keysym.sym];
#ifdef KEY_PRINT_DEBUG
                printf("keytranslation[%d] = %s (%d)  %s\n", ev.key.keysym.sym, g_keyNameTable[code], code,
                       ev.key.type == SDL_KEYDOWN ? "DOWN" : "UP");
#endif
                if (code != OSD_OSDKey() && ev.key.keysym.unicode != 0 && ev.key.type == SDL_KEYDOWN &&
                    (ev.key.keysym.unicode & 0xff80) == 0 && !keyBufferFull())
                {
                    if (OSD_HandleChar(ev.key.keysym.unicode & 0x7f))
                        keyBufferInsert(ev.key.keysym.unicode & 0x7f);
                }

                // hook in the osd
                if ((j = OSD_HandleScanCode(code, (ev.key.type == SDL_KEYDOWN))) <= 0)
                {
                    if (j == -1)  // osdkey
                    {
                        for (j = 0; j < NUMKEYS; ++j)
                        {
                            if (keyGetState(j))
                            {
                                keySetState(j, 0);
                                if (keypresscallback)
                                    keypresscallback(j, 0);
                            }
                        }
                    }
                    break;
                }

                if (ev.key.type == SDL_KEYDOWN)
                {
                    if (!keyGetState(code))
                    {
                        keySetState(code, 1);
                        if (keypresscallback)
                            keypresscallback(code, 1);
                    }
                }
                else
                {
#ifdef __linux
                    if (code == 0x59)  // pause
                        break;
#endif
                    keySetState(code, 0);
                    if (keypresscallback)
                        keypresscallback(code, 0);
                }
                break;

            case SDL_ACTIVEEVENT:
                if (ev.active.state & SDL_APPINPUTFOCUS)
                {
                    appactive = ev.active.gain;
                    if (g_mouseGrabbed && g_mouseEnabled)
                        grabmouse_low(!!appactive);
# ifdef _WIN32
                    // Win_SetKeyboardLayoutUS(appactive);

                    if (backgroundidle)
                        SetPriorityClass(GetCurrentProcess(),
                                         appactive ? NORMAL_PRIORITY_CLASS : IDLE_PRIORITY_CLASS);
# endif
                    rv = -1;

                    if (ev.active.state & SDL_APPMOUSEFOCUS)
                        g_mouseInsideWindow = ev.active.gain;
                }
                break;

            // SDL_MOUSEMOTION needs to fall through to default... this is just GEKKO processing!
            case SDL_MOUSEMOTION:
#ifdef GEKKO
                // check if it's a wiimote pointer pretending to be a mouse
                if (ev.motion.state & SDL_BUTTON_X2MASK)
                {
                    // the absolute values are used to draw the crosshair
                    g_mouseAbs.x = ev.motion.x;
                    g_mouseAbs.y = ev.motion.y;
                    // hack: reduce the scale of the "relative" motions
                    // to make it act more like a real mouse
                    ev.motion.xrel /= 16;
                    ev.motion.yrel /= 12;
                }
#endif
            fallthrough__;
            default: // OSD_Printf("Got event (%d)\n", ev.type); break;
                rv = handleevents_sdlcommon(&ev);
                break;
        }
    }

    return rv;
}

// from SDL HG, modified
int32_t SDL_WaitEventTimeout(SDL_Event *event, int32_t timeout)
{
    uint32_t expiration = 0;

    if (timeout > 0)
        expiration = SDL_GetTicks() + timeout;

    for (;;)
    {
        SDL_PumpEvents();
        switch (SDL_PeepEvents(event, 1, SDL_GETEVENT, ~0))  // SDL_FIRSTEVENT, SDL_LASTEVENT)) {
        {
            case -1: return 0;
            case 1: return 1;
            case 0:
                if (timeout == 0)
                {
                    /* Polling and no events, just return */
                    return 0;
                }
                if (timeout > 0 && ((int32_t)(SDL_GetTicks() - expiration) >= 0))
                {
                    /* Timeout expired and no events */
                    return 0;
                }
                SDL_Delay(10);
                break;
        }
    }
}
