#ifndef __HEAD__
#define __HEAD__
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <SDL.h>
#include <jni.h>
#include <SDL_mixer.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <include/core/SkCanvas.h>
#include <include/core/SkBitmap.h>
#include <include/core/SkTypeface.h>
#include <include/core/SkPaint.h>
#include <include/core/SkFont.h>
#include <GLES3/gl3.h>
#include <GLES3/gl32.h>
#include <ctime>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
using namespace glm;
using namespace std;
using namespace Assimp;
#define TAG "Test"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
//#define ZTN_DEBUG

#endif