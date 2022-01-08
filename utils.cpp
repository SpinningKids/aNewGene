
#include "utils.h"

#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
#include <mmsystem.h>
#include "Resources/resource.h"
#else
#include "SDL.h"
#endif
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Globals.h"
#include "minifmod/minifmod.h"

constexpr int ORIGINAL_WIDTH = 640;
constexpr int ORIGINAL_HEIGHT = 480;
bool isMusicEnabled = false;
bool hiddenpart = false;

using namespace std;

#ifdef WIN32
HDC hDC;
HINSTANCE g_hinstance;
constexpr char WINDOW_CLASS_NAME[] = "SKCLASS";
#else
int done = 0;
#endif /* WIN32 */

FMUSIC_MODULE* fmodule;

// Music Related

struct MEMFILE {
    int length;
    int pos;
    void* data;
};

unsigned int memopen(char* name) {
#ifdef WIN32
    HRSRC rec = FindResourceEx(GetModuleHandle(nullptr), "RC_RTDATA", name, 0);
    if (rec) {
        MEMFILE* memfile = new MEMFILE{
            (int)SizeofResource(nullptr, rec),
            0,
            LockResource(LoadResource(nullptr, rec)),
        };
        return (unsigned int)memfile;
    }
    return -1;
#else
    MEMFILE* memfile = new MEMFILE{
        sizeof(dixiesmod),
        0,
        dixiesmod,
    };
    return (unsigned int)memfile;
#endif
}

void memclose(unsigned int handle) {
    MEMFILE* memfile = (MEMFILE*)handle;
#ifdef WIN32
    if (memfile) {
        UnlockResource((HGLOBAL)(memfile->data));
    }
#endif
    delete memfile;
}

int memread(void* buffer, int size, unsigned int handle) {
    MEMFILE* memfile = (MEMFILE*)handle;

    if (memfile->pos + size >= memfile->length) {
        size = memfile->length - memfile->pos;
    }
    memcpy(buffer, (char*)memfile->data + memfile->pos, size);
    memfile->pos += size;

    return size;
}

void memseek(unsigned int handle, int pos, signed char mode) {
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

int memtell(unsigned int handle) {
    MEMFILE* memfile = (MEMFILE*)handle;
    return memfile->pos;
}

static void skSwapBuffers() {
    glFinish();
    glFlush();
#ifdef WIN32
    SwapBuffers(hDC);
#else
    SDL_GL_SwapBuffers();
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
static float      timer_time;
#ifdef WIN32
static __int64    timer_start;
static __int64    timer_frq;
#else
static Uint32     timer_start;
static constexpr float timer_frq = 1000.f;
#endif
static float      timer_lastup;
static float      timer_fps;

/* Timing Functions */
extern "C" {
    HWAVEOUT FSOUND_WaveOutHandle;
}

static float skTimerFrame() {
    if (isMusicEnabled)
    {
        MMTIME mmtime;
        mmtime.wType = TIME_SAMPLES;
        waveOutGetPosition(FSOUND_WaveOutHandle, &mmtime, sizeof(mmtime));
        timer_time = mmtime.u.ticks / (float)SAMPLERATE;
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
    if (isMusicEnabled) {
        FMUSIC_FreeSong(fmodule);
        FSOUND_Close();
        isMusicEnabled = false;
        skInitTimer();
        skTimerFrame();
    }
}

// textureload

unique_ptr<GLTexture> skLoadTexture(int resid, int logsize) {
    static HDC displaydc = CreateDC("DISPLAY", nullptr, nullptr, nullptr);
    auto ret = make_unique<GLTexture>(logsize);
    int txsize = ret->getSize();
    HBITMAP h = (HBITMAP)LoadImage(g_hinstance, (const char*)resid, IMAGE_BITMAP, txsize, txsize, LR_CREATEDIBSECTION);
    DIBSECTION ds;
    GetObject(h, sizeof(ds), &ds);
    BITMAPINFOHEADER a = {
        sizeof(BITMAPINFOHEADER),
        txsize,
        -txsize,
        1,
        32,
        BI_RGB,
        (DWORD)(4 * txsize * txsize),
        0,
        0,
        0,
        0
    };
    unsigned char* texture = new unsigned char[a.biSizeImage];
    GetDIBits(displaydc, h, 0, txsize, texture, (BITMAPINFO*)&a, DIB_RGB_COLORS);
    GLfloat* tex = ret->getImage();
    const float un255 = 1.0f / 255.0f;
    int topsize = txsize * 3 / 4;
    for (int i = 0; i < txsize * topsize; i++) {
        tex[i * 4] = tex[i * 4 + 1] = tex[i * 4 + 2] = texture[i * 4] * un255;
        tex[i * 4 + 3] = 1.0f;
    }
    for (int i = txsize * topsize; i < txsize * txsize; i++) {
        tex[i * 4] = tex[i * 4 + 1] = tex[i * 4 + 2] = 1.0f;
        tex[i * 4 + 3] = 1.0f - texture[i * 4] * un255;
    }
    ret->update();
    delete[] texture;
    DeleteObject(h);
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
        case 'd':
        {
            char* tail;
            if (optarg != nullptr) flag_d_value = strtol(optarg, &tail, 0);
            if (optarg == nullptr || tail == optarg || *tail != '\0') {
                fprintf(stderr, "Invalid display depth (bits per pixel)\n");
                exit(1);
            }
        }
        break;
        case '?':
        default:
            fprintf(stderr,
                "\n%s\n"
                "  -w        force windowed mode\n"
                "  -d bpp    set display depth (defaults to let SDL decide)\n\n", argv[0]);
            exit(0);
            break;
        }
    }

    if (flag_hidden != 2) flag_hidden = 0;
    if (flag_hidden) hiddenpart = true; else hiddenpart = false;


    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Couldn't init SDL: %s\n", SDL_GetError());
        exit(1);
    }

    if (SDL_SetVideoMode(WIDTH, HEIGHT, flag_d_value, SDL_OPENGL | (!flag_w * SDL_FULLSCREEN)) == nullptr) {
        fprintf(stderr, "Unable to create OpenGL screen: %s\n", SDL_GetError());
        SDL_Quit();
        exit(2);
    }

    SDL_WM_SetCaption("PK is dead", nullptr);
    SDL_ShowCursor(0);

    done = 0;
    FSOUND_File_SetCallbacks(memopen, memclose, memread, memseek, memtell);
    if (FSOUND_Init(SAMPLERATE, 0))
    {
        fmodule = FMUSIC_LoadSong("cippa", nullptr);
        isMusicEnabled = (fmodule != nullptr);
    }

    skInitDemoStuff();
    if (isMusicEnabled)
        FMUSIC_PlaySong(fmodule);
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
    if (isMusicEnabled) {
        FMUSIC_FreeSong(fmodule);
        FSOUND_Close();
    }
    SDL_Quit();
}
#else
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow) {
    hiddenpart = (strcmp(lpcmdline, "/PK") == 0) || (_stricmp(lpcmdline, "/g") == 0);
    g_hinstance = hinstance;
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
        windowStyle = WS_OVERLAPPEDWINDOW;
        windowExtendedStyle = WS_EX_APPWINDOW;
        RECT windowRect = { 0, 0, WIDTH, HEIGHT };
        AdjustWindowRectEx(&windowRect, windowStyle, 0, windowExtendedStyle);
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

        ShowWindow(hWND, SW_NORMAL);								// Make The Window Visible
        FSOUND_File_SetCallbacks(memopen, memclose, memread, memseek, memtell);
        if (FSOUND_Init(SAMPLERATE, 0)) {
            fmodule = FMUSIC_LoadSong(MAKEINTRESOURCE(IDR_RC_MUSIC), nullptr);
            isMusicEnabled = (fmodule != nullptr);
        }

        skInitDemoStuff();
        if (isMusicEnabled) {
            FMUSIC_PlaySong(fmodule);
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
        if (isMusicEnabled) {
            FMUSIC_FreeSong(fmodule);
            FSOUND_Close();
        }
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
