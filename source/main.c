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

static void draw_player(const char *songs[], int song_count, int selected, const char *status)
{
    printf("\x1b[47;95m");
    printf("\x1b[2J");
    printf("\x1b[2;0H");

    printf("\x1b[1;95m");
    printf("Wii MP3 Player Prototype\n\n");
    printf("\x1b[0;47;95m");

    for (int i = 0; i < song_count; i++) {
        if (i == selected) {
            printf("> %s\n", songs[i]);
        } else {
            printf("  %s\n", songs[i]);
        }
    }

    printf("\n");
    printf("UP/DOWN: Move\n");
    printf("A: Play\n");
    printf("B: Stop\n");
    printf("HOME: Exit\n");

    printf("\n");
    printf("Status: %s\n", status);
}

int main(void)
{
    initialise_video();
    WPAD_Init();

    const char *songs[] = {
        "song1.mp3",
        "song2.mp3",
        "song3.mp3",
        "my-favorite-beat.mp3"
    };

    int song_count = 4;
    int selected = 0;

    char status[128] = "Stopped";

    draw_player(songs, song_count, selected, status);

    while (1) {
        WPAD_ScanPads();

        u32 pressed = WPAD_ButtonsDown(0);

        if (pressed & WPAD_BUTTON_UP) {
            selected--;

            if (selected < 0) {
                selected = song_count - 1;
            }

            draw_player(songs, song_count, selected, status);
        }

        if (pressed & WPAD_BUTTON_DOWN) {
            selected++;

            if (selected >= song_count) {
                selected = 0;
            }

            draw_player(songs, song_count, selected, status);
        }

        if (pressed & WPAD_BUTTON_A) {
            snprintf(status, sizeof(status), "Playing %s", songs[selected]);
            draw_player(songs, song_count, selected, status);
        }

        if (pressed & WPAD_BUTTON_B) {
            snprintf(status, sizeof(status), "Stopped");
            draw_player(songs, song_count, selected, status);
        }

        if (pressed & WPAD_BUTTON_HOME) {
            break;
        }

        VIDEO_WaitVSync();
    }

    return 0;
}