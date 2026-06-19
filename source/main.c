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

static void draw_player(const char *songs[], int song_count, int selected,
                        int playing, const char *status, const char *current_song)
{
    printf("\x1b[47;95m");
    printf("\x1b[2J");
    printf("\x1b[2;0H");

    printf("========================================\n");
    printf("        Wii MP3 Player Prototype\n");
    printf("========================================\n\n");

    printf("Now Playing\n");
    printf("----------------------------------------\n");
    printf("  Song   : %s\n", current_song);
    printf("  Status : %s\n\n", status);

    printf("Playlist\n");
    printf("----------------------------------------\n");

    for (int i = 0; i < song_count; i++) {
        if (i == selected) {
            printf("> ");
        } else {
            printf("  ");
        }

        printf("%02d. %s", i + 1, songs[i]);

        if (i == playing) {
            printf(" [PLAYING]");
        }

        printf("\n");
    }

    printf("\n");
    printf("Controls\n");
    printf("----------------------------------------\n");
    printf("  UP/DOWN : Move\n");
    printf("  A       : Play\n");
    printf("  B       : Stop\n");
    printf("  HOME    : Exit\n");
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
    int playing = -1;

    char status[64] = "STOPPED";
    char current_song[128] = "None";

    draw_player(songs, song_count, selected, playing, status, current_song);

    while (1) {
        WPAD_ScanPads();

        u32 pressed = WPAD_ButtonsDown(0);

        if (pressed & WPAD_BUTTON_UP) {
            selected--;

            if (selected < 0) {
                selected = song_count - 1;
            }

            draw_player(songs, song_count, selected, playing, status, current_song);
        }

        if (pressed & WPAD_BUTTON_DOWN) {
            selected++;

            if (selected >= song_count) {
                selected = 0;
            }

            draw_player(songs, song_count, selected, playing, status, current_song);
        }

        if (pressed & WPAD_BUTTON_A) {
            playing = selected;
            snprintf(status, sizeof(status), "PLAYING");
            snprintf(current_song, sizeof(current_song), "%s", songs[selected]);

            draw_player(songs, song_count, selected, playing, status, current_song);
        }

        if (pressed & WPAD_BUTTON_B) {
            playing = -1;
            snprintf(status, sizeof(status), "STOPPED");

            draw_player(songs, song_count, selected, playing, status, current_song);
        }

        if (pressed & WPAD_BUTTON_HOME) {
            break;
        }

        VIDEO_WaitVSync();
    }

    return 0;
}