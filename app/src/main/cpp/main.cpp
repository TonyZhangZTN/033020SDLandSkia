#include "model.h"
#include "GUI.h"
#include "AssetsHelper.h"
#include "MusicHelper.h"

Model model1;
Model model2;
Cube cube;
GLuint FBO;
GLuint framebufferColorTexture, framebufferDepthTexture;

void genFrameBuffer() {
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glGenTextures(1, &framebufferColorTexture);
    glBindTexture(GL_TEXTURE_2D, framebufferColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g_width, g_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           framebufferColorTexture, 0
    );

    glGenTextures(1, &framebufferDepthTexture);
    glBindTexture(GL_TEXTURE_2D, framebufferDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, g_width, g_height, 0, GL_DEPTH_COMPONENT,
                 GL_UNSIGNED_INT, NULL
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           framebufferDepthTexture, 0
    );
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

void writeDir(const char *dir, const char *src);

GLuint LoadShader(GLenum, const char *);

char *LoadSrcFile(AAssetManager *, const char *);

void glesInit();

void render();


void render() {
//    pthread_mutex_lock(&g_mutex);
//    bool isInit = isinited;
//    pthread_mutex_unlock(&g_mutex);
//    if(isInit)
//    {
    //旋转
    static bool goState = true;
    static float distance = 0.0f;

//        if(goState){
//            distance +=0.1f;
//            if(distance >=10.0f)
//            {
//                goState = false;
//            }
//        }else{
//            distance -=0.1f;
//            if(distance <= 0.0f)
//            {
//                goState = true;
//            }
//        }
    static float rotation = 0.0f;
    rotation += 1.0f;
//        distance += 0.5f;
    GLint mvpLocation;
    //Z-Buffer
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glEnable(GL_DEPTH_TEST);
    glClearDepthf(1.0f);
    glDepthFunc(GL_LEQUAL);
    //反面消隐
//        glEnable(GL_CULL_FACE);
//        glCullFace(GL_BACK);
//        glFrontFace(GL_CCW);
    //清除帧、深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LEQUAL);
    //视口变换
    glViewport(0, 0, g_width, g_height);
    glUseProgram(g_programObject);
    //获得着色器统一变量索引
    mvpLocation = glGetUniformLocation(g_programObject, "mvp");
    //计算矩阵
    //    ┖透视投影
    glm::mat4 Projection = perspective(45.0f, (float) g_width / g_height, 1.0f, 100.0f);
    float scaleVal = 1.0f;
    glm::mat4 matScale = scale(glm::vec3(scaleVal, scaleVal, scaleVal));
    //    ┖坐标系变换
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 11, 50 - distance), // Camera is at (0,5,20), in World Space
            glm::vec3(0, 11, 0), // and looks at (0,5,0)
            glm::vec3(0, 12, 0/*50-distance*/) // Head is up (set to 0,6,0 )
    );
    mat4 rotater, translation;
    translation = translate(vec3(0, -15, 0));
    rotater = rotate(rotater, radians(-rotation), vec3(0.0f, 1.0f, 0.0f));
    mat4 mvp = Projection * View * rotater * translation * matScale;
    //传入统一变量
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &mvp[0][0]);
    //绘制物体
    cube.draw();
    model1.draw();
    translation = translate(vec3(-15, 0, 0));
    mvp = Projection * View * rotater * translation * matScale;
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &mvp[0][0]);

    model2.draw();
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
    glEnable(GL_DEPTH_TEST);
    glClearDepthf(1.0f);
    glDepthFunc(GL_LEQUAL);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LEQUAL);
    //视口变换

    glViewport(0, 0, g_width, g_height);
    glUseProgram(g_programObject);

    glActiveTexture(GL_TEXTURE31);
    glBindTexture(GL_TEXTURE_2D, framebufferColorTexture);
    glUniform1i(glGetUniformLocation(g_programObject, "TDTex"), 31);
    glActiveTexture(GL_TEXTURE0);

    g_gui->vDraw();
    SDL_GL_SwapWindow(g_window);
    //交换双缓冲区
//        eglSwapBuffers(eglDisplay, eglSurface);
//    }
//    else{
//        LOGD("not inited yet");
//    }

}

int main(int argc, char **argv) {
    SDL_GLContext glContext = nullptr;
    if (SDL_Init(SDL_INIT_EVERYTHING))
        LOGE("SDL_Init() error:%s", SDL_GetError());
    //设置GLES版本
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //获取JNI相关信息(assetManager)
    AssetHelper *assetHelper = new AssetHelper();
    g_pAssetManager = assetHelper->getAssetManager();
    delete assetHelper;
    //用于存放屏幕长宽等数据
    int nNumDiaplays = SDL_GetNumVideoDisplays();
    SDL_Rect rectDisplay;
    SDL_GetDisplayBounds(0, &rectDisplay);

    LOGI("number of Displays%d,width:%d,height:%d", nNumDiaplays, g_width = rectDisplay.w,
         g_height = rectDisplay.h);
    //Check depth size
    GLint depthSize;
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depthSize);
    LOGD("SDL_GL_DEPTH_SIZE:%d", depthSize);
    //创建GPU窗口
    SDL_Window *window = SDL_CreateWindow("Application", 0, 0, rectDisplay.w, rectDisplay.h,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
    );
    g_window = window;
    if (!window)
        LOGE("SDL_CreateWindow() error:%s", SDL_GetError());
    //创建OpenGL上下文
    SDL_GLContext context = SDL_GL_CreateContext(window);
    SDL_GL_GetCurrentContext();


    if (!context) {
        LOGE("error creating GLES context");
    }
    //Recheck size of depth buffer
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depthSize);
    LOGD("SDL_GL_DEPTH_SIZE:%d", depthSize);
    //Check external storage path

    //TODO 写一个状态信息在内部文件里，然后根据这个信息读取/写入外部文件
    LOGI("External storage path:%s", SDL_AndroidGetExternalStoragePath());

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    SDL_GL_SwapWindow(window);
    MusicHelper *musicHelper = new MusicHelper(string(SDL_AndroidGetInternalStoragePath()));
    musicHelper->PlayMusic("/test.flac");
    //
    //初始化gles程序
    glesInit();
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            LOGI("GetEvent:%x", event.type);
            if (event.type == SDL_APP_TERMINATING) {
                LOGI("You quited");
                running = false;
            }
        }
        //SDL_Delay(100);
        render();
    }
    //延迟

    SDL_DestroyWindow(window);
    window = nullptr;
    Mix_CloseAudio();
    SDL_Quit();
    LOGI("Application Exited");
    return 0;
}

void glesInit() {
    GLint maxNum;
    GLuint programObject, vertexShader, fragmentShader;
    GLint isLinked;
    char *vShader = LoadSrcFile(g_pAssetManager, "vs.glsl");
    char *fShader = LoadSrcFile(g_pAssetManager, "fs.glsl");
    vertexShader = LoadShader(GL_VERTEX_SHADER, vShader);
    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShader);
    programObject = glCreateProgram();
    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);
    glLinkProgram(programObject);
    glGetProgramiv(programObject, GL_LINK_STATUS, &isLinked);
    if (!isLinked) {
        int infoLen;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            char *infoLog = (char *) malloc(sizeof(char) * infoLen);
            glGetProgramInfoLog(programObject, infoLen, nullptr, infoLog);
            LOGE("link error:%s", infoLog);
            free(infoLog);
        }
        glDeleteProgram(programObject);
    }
    g_programObject = programObject;
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);


    string srcRoot = SDL_AndroidGetInternalStoragePath();
    model2.loadModel(srcRoot, "Test.pmx");
    model1.loadModel(srcRoot, "reimu.x");
    cube.init();
    g_gui = new GUI(g_width, g_height);
    genFrameBuffer();
    //LOGI("texID of GUI:%d",g_gui->getTextureId());
    float lightData[] = {0.0f, 0.0f, 0.0f, 0.0f,
                         0.0f, 0.0f, 0.0f, 0.0f,
                         0.0f, 0.0f, 0.0f, 0.0f};
    GLint blockSize;
    GLuint blockBufferId;
    GLuint bindingPoint = 1;
    glGenBuffers(1, &blockBufferId);
    glBindBuffer(GL_UNIFORM_BUFFER, blockBufferId);
    g_blockBufferId = blockBufferId;
    GLint lightBlockId = glGetUniformBlockIndex(g_programObject, "Light");
    glUniformBlockBinding(g_programObject, (GLuint) lightBlockId, bindingPoint);
    glGetActiveUniformBlockiv(g_programObject, (GLuint) lightBlockId, GL_UNIFORM_BLOCK_DATA_SIZE,
                              &blockSize
    );
    g_lightBufferSize = blockSize;
    glBufferData(GL_UNIFORM_BUFFER, blockSize, lightData, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, blockBufferId);
    LOGD("Size of block \"Light\" is %d", blockSize);

}

GLuint LoadShader(GLenum TYPE, const char *src) {
    GLint isCompiled;
    GLuint shader;
    shader = glCreateShader(TYPE);
    if (!shader) {
        return GL_FALSE;
    }
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (!isCompiled) {
        int infoLen;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            char *infoLog = (char *) malloc(sizeof(char) * infoLen);
            glGetShaderInfoLog(shader, infoLen, nullptr, infoLog);
            LOGE("compile failed:%s", infoLog);
            free(infoLog);
        }
        glDeleteShader(shader);
    }
    return shader;
}

char *LoadSrcFile(AAssetManager *p_AssetManager, const char *src) {
    if (!p_AssetManager) {
        return nullptr;
    }
    AAsset *p_Asset;
    off_t size;
    p_Asset = AAssetManager_open(p_AssetManager, src, AASSET_MODE_UNKNOWN);
    size = AAsset_getLength(p_Asset);
    char *pBuffer = (char *) malloc(sizeof(char) * (size + 1));
    pBuffer[size] = '\0';
    AAsset_read(p_Asset, pBuffer, size);
    AAsset_close(p_Asset);
    return pBuffer;
}

void writeDir(const char *dir, const char *src) {
    char buffer[128];
    memset(buffer, 0, 128);
    sprintf(buffer, "/data/data/ztn.edu.native052119/files/%s", dir);
    FILE *fp = NULL;
    fp = fopen(buffer, "w+");
    fputs(src, fp);
    fclose(fp);
}

