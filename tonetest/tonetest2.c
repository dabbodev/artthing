#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <alsa/asoundlib.h>
#include <alsa/mixer.h>
#define MAX_CMN_LEN 100

void SetAlsaMasterVolume(long volume)
{
    long min, max;
    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;
    const char *card = "default";
    const char *selem_name = "Master";

    snd_mixer_open(&handle, 0);
    snd_mixer_attach(handle, card);
    snd_mixer_selem_register(handle, NULL, NULL);
    snd_mixer_load(handle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);
    snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);

    snd_mixer_close(handle);
}

void CheckMOC() {
    regex_t number;
    regex_t name;
    regcomp(&number, "^[0-9]+$", 0);
    regcomp(&name, "mocp", 0);
    chdir("/proc");
    DIR* proc = opendir("/proc");
    struct dirent *dp;
    while(dp = readdir(proc)){
         if(regexec(&number, dp->d_name, 0, 0, 0)==0){
              chdir(dp->d_name);
              char buf[4096];
              int fd = open("cmdline", O_RDONLY);
              buf[read(fd, buf, (sizeof buf)-1)] = '\0';
              if(regexec(&name, buf, 0, 0, 0)==0)
                    return true
              close(fd);
              chdir("..");
         }
    }
    closedir(proc);
    return false;
}

int main(int argc, char *argv[])
{
    int sleepC = 0;
    int cVol = 100;
    int inc = false;
    SetAlsaMasterVolume(100);
    while (true) {
        if (!CheckMOC()) {
            system("mocp -p tone.wav");
        }
        if (sleepC < 10) {
            sleepC++;
        } else {
            sleepC = 0;
            if (inc) {
                cVol = cVol + 5;
            } else {
                cVol = cVol - 5;
            }
            if (cVol == 100) {
                inc = false;
            }
            if (cVol == 0) {
                inc = true;
            }
            SetAlsaMasterVolume(cVol);
        }
    }
    return 0;
}

