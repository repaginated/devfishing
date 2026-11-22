#include <math.h>
#include <stdint.h>

typedef intptr_t (*syscall_t)(intptr_t arg, ...);

typedef struct {
    float origin[3];
    float angles[3]; /* pitch, yaw, roll */
    float speed;
} fish_player_t;

enum {
    GAME_INIT = 0,
    GAME_SHUTDOWN = 1,
    GAME_CLIENT_CONNECT = 2,
    GAME_CLIENT_BEGIN = 3,
    GAME_CLIENT_USERINFO_CHANGED = 4,
    GAME_CLIENT_DISCONNECT = 5,
    GAME_CLIENT_COMMAND = 6,
    GAME_CLIENT_THINK = 7,
    GAME_RUN_FRAME = 8
};

static syscall_t syscall;

static fish_player_t player = {
    .origin = { 0.0f, 0.0f, 64.0f },
    .angles = { 0.0f, 0.0f, 0.0f },
    .speed  = 220.0f
};

/* forward_input, right_input in [-1.0f, 1.0f]; dt in seconds */
static void step_dock_movement(float forward_input, float right_input, float dt) {
    const float deg2rad = 0.017453292519943295f;
    float yaw_rad = player.angles[1] * deg2rad;

    float fwd_x = cosf(yaw_rad);
    float fwd_y = sinf(yaw_rad);
    /* Right vector in XY plane for right-handed / Quake yaw convention */
    float right_x = sinf(yaw_rad);
    float right_y = -cosf(yaw_rad);

    float move_x = fwd_x * forward_input + right_x * right_input;
    float move_y = fwd_y * forward_input + right_y * right_input;

    float len = sqrtf(move_x * move_x + move_y * move_y);
    if (len > 1.0f) {
        move_x /= len;
        move_y /= len;
    }

    player.origin[0] += move_x * player.speed * dt;
    player.origin[1] += move_y * player.speed * dt;
}

void dllEntry(syscall_t syscallptr) {
    syscall = syscallptr;
}

intptr_t vmMain(int command, int arg0, int arg1, int arg2, int arg3, int arg4,
                int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11) {
    (void)syscall;
    (void)arg0;
    (void)arg1;
    (void)arg2;
    (void)arg3;
    (void)arg4;
    (void)arg5;
    (void)arg6;
    (void)arg7;
    (void)arg8;
    (void)arg9;
    (void)arg10;
    (void)arg11;

    switch (command) {
        case GAME_INIT:
        case GAME_SHUTDOWN:
        case GAME_CLIENT_CONNECT:
        case GAME_CLIENT_BEGIN:
        case GAME_CLIENT_USERINFO_CHANGED:
        case GAME_CLIENT_DISCONNECT:
        case GAME_CLIENT_COMMAND:
        case GAME_CLIENT_THINK:
            return 0;

        case GAME_RUN_FRAME:
            /* arg0 is level time in ms; use a fixed 50ms tick until wired to real delta */
            step_dock_movement(0.0f, 0.0f, 0.05f);
            return 0;

        default:
            return 0;
    }
}
