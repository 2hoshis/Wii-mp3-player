#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <dirent.h>

#include <gccore.h>
#include <wiiuse/wpad.h>
#include <fat.h>
#include <asndlib.h>
#include <mp3player.h>

#define MAX_SONGS 64
#define SONG_NAME_LENGTH 128
#define SONG_PATH_LENGTH 256
#define MAX_MP3_SIZE (12 * 1024 * 1024)

#define SCREEN_WIDTH 79
#define SCREEN_HEIGHT 30

#define LIST_TOP_ROW 14
#define MAX_VISIBLE_SONGS 10
#define STATUS_ROW 28

typedef struct {
    char name[SONG_NAME_LENGTH];
    char path[SONG_PATH_LENGTH];
} Song;

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

static Song songs[MAX_SONGS];
static int song_count = 0;
static int selected_song = 0;
static int playing_song = -1;
static int list_offset = 0;
static int is_paused = 0;

static u8 *mp3_buffer = NULL;
static long mp3_size = 0;

static char status_message[256] = "Ready";

/* --------------------------------------------------
   文字列の末尾が .mp3 か判定
-------------------------------------------------- */
int ends_with_mp3(const char *filename) {
    int len = strlen(filename);

    if (len < 4) {
        return 0;
    }

    const char *ext = filename + len - 4;

    return (
        ext[0] == '.' &&
        (ext[1] == 'm' || ext[1] == 'M') &&
        (ext[2] == 'p' || ext[2] == 'P') &&
        ext[3] == '3'
    );
}

/* --------------------------------------------------
   SDカードの sd:/music からMP3一覧を読む
-------------------------------------------------- */
void load_songs_from_sd(void) {
    DIR *dir;
    struct dirent *entry;

    song_count = 0;
    selected_song = 0;
    playing_song = -1;
    list_offset = 0;
    is_paused = 0;

    dir = opendir("sd:/music");

    if (dir == NULL) {
        strcpy(status_message, "No sd:/music folder");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (song_count >= MAX_SONGS) {
            break;
        }

        if (ends_with_mp3(entry->d_name)) {
            snprintf(
                songs[song_count].name,
                SONG_NAME_LENGTH,
                "%s",
                entry->d_name
            );

            snprintf(
                songs[song_count].path,
                SONG_PATH_LENGTH,
                "sd:/music/%s",
                entry->d_name
            );

            song_count++;
        }
    }

    closedir(dir);

    if (song_count == 0) {
        strcpy(status_message, "No mp3 files in sd:/music");
    } else {
        snprintf(
            status_message,
            sizeof(status_message),
            "%d song(s) loaded",
            song_count
        );
    }
}

/* --------------------------------------------------
   MP3をファイルから全部メモリに読み込んで再生
-------------------------------------------------- */
void play_mp3_file(const char *path) {
    FILE *fp;
    size_t read_size;

    fp = fopen(path, "rb");

    if (fp == NULL) {
        snprintf(status_message, sizeof(status_message), "Failed to open: %s", path);
        return;
    }

    fseek(fp, 0, SEEK_END);
    mp3_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (mp3_size <= 0) {
        fclose(fp);
        strcpy(status_message, "Invalid mp3 file");
        return;
    }

    if (mp3_size > MAX_MP3_SIZE) {
        fclose(fp);
        strcpy(status_message, "MP3 too large. Try smaller file.");
        return;
    }

    if (mp3_buffer != NULL) {
        free(mp3_buffer);
        mp3_buffer = NULL;
    }

    mp3_buffer = memalign(32, mp3_size);

    if (mp3_buffer == NULL) {
        fclose(fp);
        strcpy(status_message, "Not enough memory");
        return;
    }

    read_size = fread(mp3_buffer, 1, mp3_size, fp);
    fclose(fp);

    if (read_size != (size_t)mp3_size) {
        free(mp3_buffer);
        mp3_buffer = NULL;
        strcpy(status_message, "Failed to read mp3");
        return;
    }

    DCFlushRange(mp3_buffer, mp3_size);

    ASND_Pause(0);
    MP3Player_Stop();

    if (MP3Player_PlayBuffer(mp3_buffer, mp3_size, NULL) == 0) {
        playing_song = selected_song;
        is_paused = 0;

        snprintf(
            status_message,
            sizeof(status_message),
            "Playing %d/%d: %s",
            selected_song + 1,
            song_count,
            songs[selected_song].name
        );
    } else {
        playing_song = -1;
        is_paused = 0;
        strcpy(status_message, "MP3Player_PlayBuffer failed");
    }
}

/* --------------------------------------------------
   選択中の曲を再生
-------------------------------------------------- */
void play_selected_song(void) {
    if (song_count <= 0) {
        strcpy(status_message, "No song selected");
        return;
    }

    play_mp3_file(songs[selected_song].path);
}

/* --------------------------------------------------
   次の曲を選んで再生
-------------------------------------------------- */
void play_next_song(void) {
    if (song_count <= 0) {
        strcpy(status_message, "No song selected");
        return;
    }

    selected_song++;

    if (selected_song >= song_count) {
        selected_song = 0;
    }

    play_selected_song();
}

/* --------------------------------------------------
   前の曲を選んで再生
-------------------------------------------------- */
void play_prev_song(void) {
    if (song_count <= 0) {
        strcpy(status_message, "No song selected");
        return;
    }

    selected_song--;

    if (selected_song < 0) {
        selected_song = song_count - 1;
    }

    play_selected_song();
}

/* --------------------------------------------------
   再生停止
-------------------------------------------------- */
void stop_mp3(void) {
    ASND_Pause(0);
    MP3Player_Stop();

    playing_song = -1;
    is_paused = 0;
    strcpy(status_message, "Stopped");
}

/* --------------------------------------------------
   一時停止 / 再開
-------------------------------------------------- */
void toggle_pause(void) {
    if (playing_song < 0) {
        strcpy(status_message, "No song is playing");
        return;
    }

    if (is_paused) {
        ASND_Pause(0);
        is_paused = 0;

        snprintf(
            status_message,
            sizeof(status_message),
            "Resumed %d/%d: %s",
            playing_song + 1,
            song_count,
            songs[playing_song].name
        );
    } else {
        ASND_Pause(1);
        is_paused = 1;

        snprintf(
            status_message,
            sizeof(status_message),
            "Paused %d/%d: %s",
            playing_song + 1,
            song_count,
            songs[playing_song].name
        );
    }
}

/* --------------------------------------------------
   指定行を白背景で消す
-------------------------------------------------- */
void clear_line(int row) {
    int i;

    printf("\x1b[47;35m");
    printf("\x1b[%d;1H", row);

    for (i = 0; i < SCREEN_WIDTH; i++) {
        putchar(' ');
    }

    printf("\x1b[%d;1H", row);
}

/* --------------------------------------------------
   画面全体を白背景で塗る
   起動時や再読み込み時だけ使う
-------------------------------------------------- */
void clear_screen_white(void) {
    int y;

    printf("\x1b[47;35m");

    for (y = 1; y <= SCREEN_HEIGHT; y++) {
        clear_line(y);
    }

    printf("\x1b[1;1H");
}

/* --------------------------------------------------
   選択中の曲が見えるようにスクロール位置を調整
-------------------------------------------------- */
void update_list_offset(void) {
    if (selected_song < list_offset) {
        list_offset = selected_song;
    }

    if (selected_song >= list_offset + MAX_VISIBLE_SONGS) {
        list_offset = selected_song - MAX_VISIBLE_SONGS + 1;
    }

    if (list_offset < 0) {
        list_offset = 0;
    }
}

/* --------------------------------------------------
   曲リスト部分だけ描画
-------------------------------------------------- */
void draw_song_list(void) {
    int i;
    int index;
    int row;
    int last_visible;

    update_list_offset();

    for (i = 0; i < MAX_VISIBLE_SONGS; i++) {
        row = LIST_TOP_ROW + i;
        index = list_offset + i;

        clear_line(row);

        if (index < song_count) {
            if (index == selected_song && index == playing_song) {
                if (is_paused) {
                    printf(">P %s", songs[index].name);
                } else {
                    printf(">* %s", songs[index].name);
                }
            } else if (index == selected_song) {
                printf(" > %s", songs[index].name);
            } else if (index == playing_song) {
                if (is_paused) {
                    printf(" P %s", songs[index].name);
                } else {
                    printf(" * %s", songs[index].name);
                }
            } else {
                printf("   %s", songs[index].name);
            }
        }
    }

    clear_line(LIST_TOP_ROW + MAX_VISIBLE_SONGS);

    if (song_count > MAX_VISIBLE_SONGS) {
        last_visible = list_offset + MAX_VISIBLE_SONGS;

        if (last_visible > song_count) {
            last_visible = song_count;
        }

        printf(
            "   Showing %d-%d / %d",
            list_offset + 1,
            last_visible,
            song_count
        );
    }
}

/* --------------------------------------------------
   ステータス行だけ描画
-------------------------------------------------- */
void draw_status(void) {
    clear_line(STATUS_ROW);
    printf("Status: %s", status_message);
}

/* --------------------------------------------------
   固定UIを描画
-------------------------------------------------- */
void draw_base_screen(void) {
    clear_screen_white();

    printf("\x1b[47;35m");
    printf("\x1b[1;1H");

    printf("========================================\n");
    printf("        Wii Homebrew MP3 Player          \n");
    printf("========================================\n\n");

    printf("Controls:\n");
    printf("  UP/DOWN : Select only\n");
    printf("  A       : Play selected\n");
    printf("  B       : Stop\n");
    printf("  + / -   : Next / Previous and play\n");
    printf("  1       : Reload sd:/music\n");
    printf("  2       : Pause / Resume\n");
    printf("  HOME    : Exit\n\n");

    printf("----------------------------------------\n");
    printf("Songs in sd:/music\n");
    printf("----------------------------------------\n");

    draw_song_list();

    clear_line(STATUS_ROW - 1);
    printf("----------------------------------------");

    draw_status();

    clear_line(STATUS_ROW + 1);
    printf("----------------------------------------");

    printf("\x1b[47;35m");
}

/* --------------------------------------------------
   Wiiの画面初期化
-------------------------------------------------- */
void init_video(void) {
    VIDEO_Init();

    rmode = VIDEO_GetPreferredMode(NULL);

    xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

    console_init(
        xfb,
        0,
        0,
        rmode->fbWidth,
        rmode->xfbHeight,
        rmode->fbWidth * VI_DISPLAY_PIX_SZ
    );

    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();

    if (rmode->viTVMode & VI_NON_INTERLACE) {
        VIDEO_WaitVSync();
    }

    VIDEO_WaitVSync();
}

/* --------------------------------------------------
   main
-------------------------------------------------- */
int main(int argc, char **argv) {
    int running = 1;

    init_video();

    WPAD_Init();

    if (!fatInitDefault()) {
        strcpy(status_message, "fatInitDefault failed");
    } else {
        strcpy(status_message, "SD initialized");
        load_songs_from_sd();
    }

    ASND_Init();
    MP3Player_Init();

    draw_base_screen();

    while (running) {
        WPAD_ScanPads();

        u32 pressed = WPAD_ButtonsDown(0);

        if (pressed & WPAD_BUTTON_HOME) {
            running = 0;
        }

        if (pressed & WPAD_BUTTON_UP) {
            if (song_count > 0) {
                selected_song--;

                if (selected_song < 0) {
                    selected_song = song_count - 1;
                }

                draw_song_list();
            }
        }

        if (pressed & WPAD_BUTTON_DOWN) {
            if (song_count > 0) {
                selected_song++;

                if (selected_song >= song_count) {
                    selected_song = 0;
                }

                draw_song_list();
            }
        }

        if (pressed & WPAD_BUTTON_A) {
            play_selected_song();
            draw_song_list();
            draw_status();
        }

        if (pressed & WPAD_BUTTON_B) {
            stop_mp3();
            draw_song_list();
            draw_status();
        }

        if (pressed & WPAD_BUTTON_PLUS) {
            play_next_song();
            draw_song_list();
            draw_status();
        }

        if (pressed & WPAD_BUTTON_MINUS) {
            play_prev_song();
            draw_song_list();
            draw_status();
        }

        if (pressed & WPAD_BUTTON_1) {
            ASND_Pause(0);
            MP3Player_Stop();

            is_paused = 0;
            playing_song = -1;

            if (mp3_buffer != NULL) {
                free(mp3_buffer);
                mp3_buffer = NULL;
            }

            load_songs_from_sd();
            draw_base_screen();
        }

        if (pressed & WPAD_BUTTON_2) {
            toggle_pause();
            draw_song_list();
            draw_status();
        }

        VIDEO_WaitVSync();
    }

    ASND_Pause(0);
    MP3Player_Stop();

    if (mp3_buffer != NULL) {
        free(mp3_buffer);
        mp3_buffer = NULL;
    }

    return 0;
}