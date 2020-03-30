#include "MusicHelper.h"
#define __DEBUG_MODE
MusicHelper::MusicHelper(string musicSourceRootDirPath) {
    this->musicSourceRootDirPath = musicSourceRootDirPath;
}

MusicHelper::~MusicHelper() {

}

void MusicHelper::PlayMusic(string musicPathName) {
    if (this->musicSourceRootDirPath.empty()) {
        LOGE("[MusicHelper]The music source root dir path is empty!");
        return;
    }
    if (musicPathName.empty()) {
        LOGE("[MusicHelper]The music path name is empty!");
        return;
    }
    string musicDir = this->musicSourceRootDirPath+"/"+musicPathName;
#ifdef __DEBUG_MODE
    LOGI("Playing music :Path %s", musicDir.c_str());
#endif
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048)) {
        LOGE("[MusicHelper]error while opening audio:%s", Mix_GetError());
        return;
    }
    Mix_Music *sound = Mix_LoadMUS(musicDir.c_str());
    if (Mix_PlayMusic(sound, 1)) {
        LOGE("[MusicHelper]error while playing music:%s", Mix_GetError());
    }
}