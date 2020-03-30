#include "head.h"

class MusicHelper{
private:
    string musicSourceRootDirPath;

public:
    MusicHelper(string);
    ~MusicHelper(void);
    void PlayMusic(string);
};