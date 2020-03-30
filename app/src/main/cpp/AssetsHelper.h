#include "head.h"

class AssetHelper {
private:
    AAssetManager *pAssetManager;

public:
    AssetHelper();

    ~AssetHelper();

    AAssetManager *getAssetManager(void);
};