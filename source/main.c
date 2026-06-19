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

static void draw_menu(int selected)
{
    const char *items[] = {
        "Hello",
        "Button Test",
        "Future MP3 Player"
    };

    int item_count = 3;

    printf("\x1b[2J");
    printf("\x1b[2;0H");

    printf("Wii Homebrew Practice\n\n");

    for (int i = 0; i < item_count; i++) {
        if (i == selected) {
            printf("> %s\n", items[i]);
        } else {
            printf("  %s\n", items[i]);
        }
    }

    printf("\n");
    printf("Press UP/DOWN to move.\n");
    printf("Press A to select.\n");
    printf("Press HOME to exit.\n");
}

int main(void)
{
    initialise_video();
    WPAD_Init();

    int selected = 0;
    int item_count = 3;

    draw_menu(selected);

    while (1) {
        WPAD_ScanPads();

        u32 pressed = WPAD_ButtonsDown(0);

        if (pressed & WPAD_BUTTON_UP) {
            selected--;

            if (selected < 0) {
                selected = item_count - 1;
            }

            draw_menu(selected);
        }

        if (pressed & WPAD_BUTTON_DOWN) {
            selected++;

            if (selected >= item_count) {
                selected = 0;
            }

            draw_menu(selected);
        }

        if (pressed & WPAD_BUTTON_A) {
            printf("\x1b[12;0H");

            if (selected == 0) {
                printf("Selected: Hello             \n");
            } else if (selected == 1) {
                printf("Selected: Button Test       \n");
            } else if (selected == 2) {
                printf("Selected: Future MP3 Player \n");
            }
        }

        if (pressed & WPAD_BUTTON_HOME) {
            break;
        }

        VIDEO_WaitVSync();
    }

    return 0;
}