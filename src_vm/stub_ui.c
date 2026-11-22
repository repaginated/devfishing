#include "../Quake3e/code/qcommon/q_shared.h"

typedef intptr_t (*syscall_t)(intptr_t arg, ...);
static syscall_t syscall;

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
        case 0: /* UI_GETAPIVERSION */
            return 6;
    }
    return 0;
}
