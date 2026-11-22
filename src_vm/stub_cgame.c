#include <string.h>
#include <math.h>
#include "../Quake3e/code/qcommon/q_shared.h"
#include "../Quake3e/code/renderercommon/tr_types.h"
#include "../Quake3e/code/game/bg_public.h"
#define COM_TRAP_GETVALUE 700
#include "../Quake3e/code/cgame/cg_public.h"

typedef intptr_t (*syscall_t)(intptr_t arg, ...);
static syscall_t syscall;
static glconfig_t glconfig;

void dllEntry(syscall_t syscallptr) {
    syscall = syscallptr;
}

static void DrawFrame(int serverTime) {
    refdef_t refdef;
    vec3_t angles;

    memset(&refdef, 0, sizeof(refdef));

    refdef.x = 0;
    refdef.y = 0;
    refdef.width = glconfig.vidWidth;
    refdef.height = glconfig.vidHeight;
    refdef.fov_x = 90.0f;
    refdef.fov_y = 65.0f;
    refdef.time = serverTime;

    /* Position camera standing on the wooden dock looking out over the sea */
    refdef.vieworg[0] = 0.0f;
    refdef.vieworg[1] = -128.0f;
    refdef.vieworg[2] = 74.0f;

    /* Looking North (+Y) and slightly down toward the dock & water horizon */
    angles[0] = 12.0f;
    angles[1] = 90.0f;
    angles[2] = 0.0f;

    AngleVectors(angles, refdef.viewaxis[0], refdef.viewaxis[1], refdef.viewaxis[2]);
    VectorInverse(refdef.viewaxis[1]); /* Quake 3 viewaxis[1] is LEFT, not right */

    syscall(CG_R_CLEARSCENE);
    syscall(CG_R_RENDERSCENE, &refdef);
}

intptr_t vmMain(int command, int arg0, int arg1, int arg2, int arg3, int arg4,
                int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11) {
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
        case CG_INIT:
            syscall(CG_GETGLCONFIG, &glconfig);
            syscall(CG_R_LOADWORLDMAP, "maps/flat_ocean.bsp");
            return 0;
        case CG_DRAW_ACTIVE_FRAME:
            DrawFrame(arg0);
            return 0;
    }
    return 0;
}
