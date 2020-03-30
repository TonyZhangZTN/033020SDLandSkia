#include "head.h"
typedef struct{
    //设置Red
    Uint32 rmask = 0x00ff0000;
    //设置Green
    Uint32 gmask = 0x0000ff00;
    //设置Blue
    Uint32 bmask = 0x000000ff;
    //设置Alpha
    Uint32 amask = 0xff000000;
}RGBA;
typedef enum {
    WIDGET_BUTTON=1,
    WIDGET_TEXT,
    WIDGET_ASSEMBLE
}WIDGET_TYPE;
extern RGBA rgba;
class WIDGET{
public:
    WIDGET(std::string type);
    ~WIDGET();
};
class GUI{
private:
    SkBitmap skBitmap;
    int screenWidth,screenHeight;
    GLuint VAO,VBO,EBO;
    GLuint textureId;
    vector<WIDGET> widgetContainer;

    void createTexture();

    ~GUI();
    void activeTexture();
    void inactiveTexture();
    void init();
    void updateTexture();
public:
    GUI(GLint screenWidth, GLint screenHeight);
    void draw();
    void* getPixels();

    void vDraw();


};
extern GUI* g_gui;