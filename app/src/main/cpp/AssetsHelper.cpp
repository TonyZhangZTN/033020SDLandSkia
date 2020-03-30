//
// Created by ZTN on 2020/3/29/029.
//

#include "AssetsHelper.h"

AssetHelper::AssetHelper() {
    //获取JNI相关信息(assetManager)
    JNIEnv *env = (JNIEnv *) SDL_AndroidGetJNIEnv();
    jobject objActivity = (jobject) SDL_AndroidGetActivity();
    jclass clazz(env->GetObjectClass(objActivity));
    jmethodID mid = env->GetMethodID(clazz, "getAssetManagerFromJava",
                                     "()Landroid/content/res/AssetManager;"
    );
    AAssetManager *pAssetMgr = AAssetManager_fromJava(env, env->CallObjectMethod(objActivity, mid));
    if (pAssetMgr)LOGD("get the pAssetMgr");

    //清理工作
    env->DeleteLocalRef(objActivity);
    env->DeleteLocalRef(clazz);
    this->pAssetManager = pAssetMgr;
}
AssetHelper::~AssetHelper() {

}
AAssetManager* AssetHelper::getAssetManager(){
    return this->pAssetManager;
}