#include <gccore.h>
#include <stdio.h>
#include <wiiuse/wpad.h>

static void initialise_video(void)
{
    VIDEO_Init();

    GXRModeObj *video_mode = VIDEO_GetPreferredMode(NULL);
    void *framebuffer = MEM_K0_TO_K1(SYS_AllocateFramebuffer(video_mode));

    console_init(framebuffer, 20, 20, video_mode->fbWidth, video_mode->xfbHeight,
                 video_mode->fbWidth * VI_DISPLAY_PIX_SZ);

    VIDEO_Configure(video_mode);
    VIDEO_SetNextFramebuffer(framebuffer);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();

    if (video_mode->viTVMode & VI_NON_INTERLACE) {
        VIDEO_WaitVSync();
    }
}

int main(void)
{
    initialise_video();
    WPAD_Init();

    printf("\x1b[2;0H");
    printf("Hello World from Wii homebrew!\n\n");
    printf("Press A to show a message.\n");
    printf("Press HOME on a Wii Remote to exit.\n");

    while (1) {
        WPAD_ScanPads();

        u32 pressed = WPAD_ButtonsDown(0);

        if (pressed & WPAD_BUTTON_A) {
            printf("\x1b[8;0H");
            printf("You pressed A!        \n");
        }

        if (pressed & WPAD_BUTTON_HOME) {
            break;
        }

        VIDEO_WaitVSync();
    }

    return 0;
}