#include <gccore.h>
#include <stdio.h>
#include <wiiuse/wpad.h>
#include <fat.h>
#include <dirent.h>
#include <string.h>

#define MAX_SONGS 16
#define SONG_NAME_LENGTH 128
#define SONG_PATH_LENGTH 256

typedef struct {
    char name[SONG_NAME_LENGTH];
    char path[SONG_PATH_LENGTH];
} Song;

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

static int has_mp3_extension(const char *filename)
{
    int length = strlen(filename);

    if (length < 4) {
        return 0;
    }

    const char *extension = filename + length - 4;

    if (strcmp(extension, ".mp3") == 0) {
        return 1;
    }

    if (strcmp(extension, ".MP3") == 0) {
        return 1;
    }

    return 0;
}

static int load_songs_from_sd(Song songs[])
{
    int song_count = 0;

    if (!fatInitDefault()) {
        snprintf(songs[0].name, SONG_NAME_LENGTH, "SD init failed");
        snprintf(songs[0].path, SONG_PATH_LENGTH, "");
        return 1;
    }

    DIR *dir = opendir("sd:/music");

    if (dir == NULL) {
        snprintf(songs[0].name, SONG_NAME_LENGTH, "No sd:/music folder");
        snprintf(songs[0].path, SONG_PATH_LENGTH, "");
        return 1;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL && song_count < MAX_SONGS) {
        if (has_mp3_extension(entry->d_name)) {
            snprintf(songs[song_count].name, SONG_NAME_LENGTH, "%s", entry->d_name);
            snprintf(songs[song_count].path, SONG_PATH_LENGTH, "sd:/music/%s", entry->d_name);
            song_count++;
        }
    }

    closedir(dir);

    if (song_count == 0) {
        snprintf(songs[0].name, SONG_NAME_LENGTH, "No mp3 files found");
        snprintf(songs[0].path, SONG_PATH_LENGTH, "");
        return 1;
    }

    return song_count;
}

static void draw_player(Song songs[], int song_count, int selected,
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

        printf("%02d. %s", i + 1, songs[i].name);

        if (i == playing) {
            printf(" [PLAYING]");
        }

        printf("\n");
    }

    printf("\n");
    printf("Controls\n");
    printf("----------------------------------------\n");
    printf("  UP/DOWN : Move\n");
    printf("  A       : Select\n");
    printf("  B       : Stop\n");
    printf("  HOME    : Exit\n");
}

int main(void)
{
    initialise_video();
    WPAD_Init();

    Song songs[MAX_SONGS];
    int song_count = load_songs_from_sd(songs);

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
            snprintf(status, sizeof(status), "SELECTED");
            snprintf(current_song, sizeof(current_song), "%s", songs[selected].name);

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