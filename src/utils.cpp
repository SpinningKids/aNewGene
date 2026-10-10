
#include "utils.h"

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#else
#include <SDL2/SDL.h>
#include <unistd.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include "embedded/dunesh_2.h"
#include "embedded/gcbitmap.h"
#include "Globals.h"
#include "minifmod/minifmod.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <utility>

constexpr int ORIGINAL_WIDTH = 640;
constexpr int ORIGINAL_HEIGHT = 480;
bool hiddenpart = false;

using namespace std;

#ifdef WIN32
HDC hDC;
HINSTANCE g_hinstance;
constexpr char WINDOW_CLASS_NAME[] = "SKCLASS";
#else
int done = 0;
SDL_Window* sdl_window = nullptr;
#endif /* WIN32 */

PlayerState* player_state = nullptr;

bool isMusicEnabled() {
    return !!player_state;
}

// Music Related

struct MEMFILE {
    size_t length;
    size_t pos;
    const void* data;
};

void* memopen(const char* name) {
    (void)name;
    MEMFILE* memfile = new MEMFILE{
        sizeof(dixiesmod),
        0,
        dixiesmod,
    };
    return memfile;
}

void memclose(void* handle) {
    delete (MEMFILE*)handle;
}

size_t memread(void* buffer, size_t size, void* handle) {
    MEMFILE* memfile = (MEMFILE*)handle;

    if (memfile->pos + size >= memfile->length) {
        size = memfile->length - memfile->pos;
    }
    memcpy(buffer, (char*)memfile->data + memfile->pos, size);
    memfile->pos += size;

    return size;
}

void memseek(void* handle, long pos, int mode) {
    MEMFILE* memfile = (MEMFILE*)handle;

    if (mode == SEEK_SET) {
        memfile->pos = pos;
    } else if (mode == SEEK_CUR) {
        memfile->pos += pos;
    } else if (mode == SEEK_END) {
        memfile->pos = memfile->length + pos;
    }
    if (memfile->pos > memfile->length) {
        memfile->pos = memfile->length;
    }
}

long memtell(void* handle) {
    MEMFILE* memfile = (MEMFILE*)handle;
    return memfile->pos;
}

static void skSwapBuffers() {
    glFinish();
    glFlush();
#ifdef WIN32
    SwapBuffers(hDC);
#else
    SDL_GL_SwapWindow(sdl_window);
#endif
}

// windows

#ifdef WIN32
static LRESULT CALLBACK skWinProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_KEYDOWN:
        if ((int)wp != VK_ESCAPE)
            break;
    case WM_CLOSE:
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_ERASEBKGND:
        return 1;
    }
    return DefWindowProc(wnd, msg, wp, lp);
}
#endif /* WIN32 */

void panViewOrtho() {
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIDTH, HEIGHT, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void panViewOrthoModified() {
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    int w = ORIGINAL_HEIGHT * WIDTH / HEIGHT;
    if (w >= ORIGINAL_WIDTH) {
        int margin = (w - ORIGINAL_WIDTH) / 2;
        glOrtho(-margin, ORIGINAL_WIDTH + margin, ORIGINAL_HEIGHT, 0, -1, 1);
    } else {
        int h = ORIGINAL_WIDTH * HEIGHT / WIDTH;
        int margin = (h - ORIGINAL_HEIGHT) / 2;
        glOrtho(0, WIDTH, ORIGINAL_HEIGHT + margin, -margin, -1, 1);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void panViewPerspective(double fov, double znear, double zfar) {
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, (double)WIDTH / HEIGHT, znear, zfar);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/* Timing vars */
static float timer_time;
#ifdef WIN32
static __int64 timer_start;
static __int64 timer_frq;
#else
static Uint32 timer_start;
static constexpr float timer_frq = 1000.f;
#endif
static float timer_lastup;
static float timer_fps;

/* Timing Functions */

static float skTimerFrame() {
    if (isMusicEnabled()) {
        timer_time = FMUSIC_GetTime() / 1000.f;
    } else {
#ifdef WIN32
        __int64 a;
        QueryPerformanceCounter((LARGE_INTEGER*)&a);
#else
        // TODO: Check - something is fishy here: skGetTime() already subtracts timer_start
        float a = SDL_GetTicks();
#endif

        timer_time = static_cast<float>(a - timer_start) / timer_frq;
    }

    /* do not change this pls */
    timer_fps = 1 / ((timer_time - timer_lastup) + 0.0001f);
    timer_lastup = timer_time;
    return timer_time;
}

void skInitTimer() {
#ifdef WIN32
    QueryPerformanceCounter((LARGE_INTEGER*)&timer_start);
    QueryPerformanceFrequency((LARGE_INTEGER*)&timer_frq);
#else
    timer_start = SDL_GetTicks();
#endif
    timer_lastup = 0;
    timer_time = 0;
}

float skGetTime() {
    return timer_time;
}

float skGetFPS() {
    return timer_fps;
}

void skStopMusic() {
    if (isMusicEnabled()) {
        FMUSIC_FreeSong(FMUSIC_StopSong(std::exchange(player_state, nullptr)));
        skInitTimer();
        skTimerFrame();
    }
}

// textureload

// The credits bitmap is embedded in the form we use (embedded/gcbitmap.h):
// the 4bpp pixel payload, 2 pixels per byte, rows stored bottom-up. Its
// 16-color palette is the gray ramp (entry i has value i * 0x11), so a
// pixel's luminance is its index * 0x11. Take that as RGB on the top 3/4
// of the image and as alpha on the bottom 1/4, exactly like the former
// GDI path did.
unique_ptr<GLTexture> skLoadTexture() {
    auto ret = make_unique<GLTexture>(8);
    constexpr int txsize = 256;
    constexpr int topsize = txsize * txsize * 3 / 4;
    GLfloat* tex = ret->getImage();
    const float un255 = 1.0f / 255.0f;
    for (int i = 0; i < txsize * txsize; i++) {
        const unsigned char pixelbyte = gcbitmap_4bpp[(i ^ 0xff00) >> 1];
        const unsigned char index = (i & 1) ? (pixelbyte & 0x0f) : (pixelbyte >> 4);
        const int luma = index * 0x11;
        if (i < topsize) {
            tex[i * 4] = tex[i * 4 + 1] = tex[i * 4 + 2] = luma * un255;
            tex[i * 4 + 3] = 1.0f;
        } else {
            tex[i * 4] = tex[i * 4 + 1] = tex[i * 4 + 2] = 1.0f;
            tex[i * 4 + 3] = 1.0f - luma * un255;
        }
    }
    ret->update();
    return ret;
}

bool skHiddenPart() {
    return hiddenpart;
}

#ifndef WIN32
/*
 * Switches:
 *
 * -w       force windowed mode (default is fullscreen)
 * -d bpp   force depth (bits per pixels, normally 32)
 * -PK      hidden part
 */
int flag_w = 0;
int flag_hidden = 0;
int flag_d_value = 0;

int main(int argc, char* argv[]) {
    int c;

    while ((c = getopt(argc, argv, "?PKgwd:")) != -1) {
        switch (c) {
        case 'P':
            flag_hidden = 1;
            break;
        case 'K':
            if (flag_hidden) flag_hidden = 2;
            break;
        case 'g':
            flag_hidden = 2;
            break;
        case 'w':
            flag_w = 1;
            break;
        case 'd': {
            char* tail;
            if (optarg != nullptr) flag_d_value = strtol(optarg, &tail, 0);
            if (optarg == nullptr || tail == optarg || *tail != '\0') {
                fprintf(stderr, "Invalid display depth (bits per pixel)\n");
                exit(1);
            }
        } break;
        case '?':
        default:
            fprintf(stderr,
                    "\n%s\n"
                    "  -w        force windowed mode\n"
                    "  -d bpp    set display depth (defaults to let SDL decide)\n\n",
                    argv[0]);
            exit(0);
            break;
        }
    }

    if (flag_hidden != 2) flag_hidden = 0;
    if (flag_hidden) hiddenpart = true;
    else
        hiddenpart = false;


    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Couldn't init SDL: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    if (flag_d_value > 0) {
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, flag_d_value);
    }
    sdl_window = SDL_CreateWindow("PK is dead", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
    if (!flag_w) {
        SDL_SetWindowFullscreen(sdl_window, SDL_TRUE);
    }
    if (sdl_window == nullptr) {
        fprintf(stderr, "Unable to create OpenGL screen: %s\n", SDL_GetError());
        SDL_Quit();
        exit(2);
    }
    if (SDL_GL_CreateContext(sdl_window) == nullptr) {
        fprintf(stderr, "Unable to create OpenGL context: %s\n", SDL_GetError());
        SDL_Quit();
        exit(3);
    }
    SDL_ShowCursor(0);

    done = 0;
    FSOUND_File_SetCallbacks(memopen, memclose, memread, memseek, memtell);
    Module* module = nullptr;
    if (FSOUND_Init(SAMPLERATE, 0)) {
        module = FMUSIC_LoadSong("cippa", nullptr);
    }

    skInitDemoStuff();
    if (module) {
        player_state = FMUSIC_PlaySong(module);
    }
    skInitTimer();
    while (!done) {
        skDraw();
        skSwapBuffers();
        skTimerFrame();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                done = 1;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE ||
                    event.key.keysym.sym == SDLK_SPACE ||
                    event.key.keysym.sym == 65 ||
                    event.key.keysym.sym == 9) { // strange things happen...
                    done = 1;
                }
            }
        }
    }
    if (isMusicEnabled()) {
        FMUSIC_FreeSong(FMUSIC_StopSong(std::exchange(player_state, nullptr)));
    }
    SDL_Quit();
}
#else
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow) {
    // lpcmdline arrives with a leading space (Windows convention); trim it so
    // that "/PK" and "/g" match no matter how they are typed.
    while (*lpcmdline == ' ') ++lpcmdline;
    hiddenpart = (_stricmp(lpcmdline, "/PK") == 0) || (_stricmp(lpcmdline, "/g") == 0);
    g_hinstance = hinstance;
    SetProcessDPIAware(); // ask for physical pixels, not DPI-virtualized ones
    HDC l_hDC = GetDC(GetDesktopWindow());

    WNDCLASSEX winclass{
        sizeof(WNDCLASSEX),
        CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
        (WNDPROC)(skWinProc),
        0,
        0,
        hinstance,
        LoadIcon(nullptr, IDI_APPLICATION),
        LoadCursor(nullptr, IDC_ARROW),
        (HBRUSH)(COLOR_APPWORKSPACE),
        nullptr,
        WINDOW_CLASS_NAME,
        LoadIcon(nullptr, IDI_APPLICATION)
    };

    if (!RegisterClassEx(&winclass)) {
        return 0;
    }
    DWORD windowStyle, windowExtendedStyle;

    DEVMODE previous_mode{};

    bool fullscreen = false;

    if constexpr (FULLSCREEN) {
        previous_mode.dmSize = sizeof(previous_mode);
        previous_mode.dmBitsPerPel = GetDeviceCaps(l_hDC, BITSPIXEL);
        previous_mode.dmPelsWidth = GetDeviceCaps(l_hDC, HORZRES);
        previous_mode.dmPelsHeight = GetDeviceCaps(l_hDC, VERTRES);
        previous_mode.dmDisplayFrequency = GetDeviceCaps(l_hDC, VREFRESH);
        previous_mode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

        int num = 0;
        int selected_num = -1;
        DEVMODE dmScreenSettings;
        size_t selected_bits = 0;
        while (EnumDisplaySettings(nullptr, num, &dmScreenSettings)) {
            if (size_t current_bits = dmScreenSettings.dmBitsPerPel * dmScreenSettings.dmPelsHeight * dmScreenSettings.dmPelsWidth; selected_bits < current_bits) {
                WIDTH = dmScreenSettings.dmPelsWidth;
                HEIGHT = dmScreenSettings.dmPelsHeight;
                BITSPERPIXEL = dmScreenSettings.dmBitsPerPel;
                selected_bits = current_bits;
                selected_num = num;
            }
            ++num;
        }

        EnumDisplaySettings(nullptr, selected_num, &dmScreenSettings);
        fullscreen = ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL;
        if (fullscreen) {
            ShowCursor(false);
            windowStyle = WS_POPUP;
            windowExtendedStyle = WS_EX_APPWINDOW | WS_EX_TOPMOST;
        }
    }
    if (!fullscreen) {
        // the mode switch was refused: stay in the current desktop mode and cover
        // it whole and borderless, rather than a framed window in the corner
        windowStyle = WS_POPUP;
        windowExtendedStyle = WS_EX_APPWINDOW;
        WIDTH = previous_mode.dmPelsWidth;
        HEIGHT = previous_mode.dmPelsHeight;
        BITSPERPIXEL = previous_mode.dmBitsPerPel;
    }
    if (HWND hWND = CreateWindowEx(windowExtendedStyle, WINDOW_CLASS_NAME, WINDOW_TITLE, windowStyle, 0, 0, WIDTH, HEIGHT, HWND_DESKTOP, nullptr, hinstance, nullptr)) {
        hDC = GetDC(hWND);
        PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, BITSPERPIXEL, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 16, 1, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 }; // but his is even longer....
        unsigned int iPixelFormat = ChoosePixelFormat(hDC, &pfd);

        // make that match the device context's current pixel format
        SetPixelFormat(hDC, iPixelFormat, &pfd);

        // if we can create a rendering context ...
        if (HGLRC hRC = wglCreateContext(hDC)) {
            // try to make it the thread's current rendering context
            wglMakeCurrent(hDC, hRC);
        }

        ShowWindow(hWND, SW_NORMAL); // Make The Window Visible
        FSOUND_File_SetCallbacks(memopen, memclose, memread, memseek, memtell);
        Module* module = nullptr;
        if (FSOUND_Init(SAMPLERATE, 0)) {
            module = FMUSIC_LoadSong("cippa", nullptr);
        }

        skInitDemoStuff();
        if (module) {
            player_state = FMUSIC_PlaySong(module);
        }
        skInitTimer();

        while (true) {
            MSG msg;
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            } else {
                skDraw();
                skSwapBuffers();
                skTimerFrame();
            }
        }
        skStopMusic();
    } else {
        MessageBox(GetDesktopWindow(), "Can't create window", "SKerror", MB_OK);
    }

    if (fullscreen) {
        ChangeDisplaySettings(&previous_mode, 0);
        ShowCursor(true);
    }

    skUnloadDemoStuff();

    ExitProcess(0);
    return 0;
}
#endif

void skQuitDemo() {
#ifdef WIN32
    PostQuitMessage(0);
#else
    done = 1;
#endif
}
