#include "GUI.h"
#include "model.h"

//
// Created by ZTN on 2020/3/4/004.
//
int CalculateFrameRate() {
    static int lastTime = 0;
    int currentTime = SDL_GetTicks();
    int deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    static float avgDuration = 0.f;
    static float alpha = 1.f / 100.f; // 采样数设置为100
    static int frameCount = 0;

    ++frameCount;

    int fps = 0;
    if (1 == frameCount) {
        avgDuration = static_cast<float>(deltaTime);
    } else {
        avgDuration = avgDuration * (1 - alpha) + deltaTime * alpha;
    }

    fps = static_cast<int>(1.f / avgDuration * 1000);
    return fps;

}

RGBA rgba;
GUI *g_gui;

GUI::GUI(GLint screenWidth, GLint screenHeight) {
    this->screenHeight = screenHeight;
    this->screenWidth = screenWidth;
    this->draw();
    createTexture();
    this->init();
}

GUI::~GUI() {
    delete this;
}

void GUI::draw() {
    //TODO 需要根据控件来绘制
    //声明
    SkBitmap bitmap;
    //设置Bitmap的配置信息
    bitmap.setInfo(SkImageInfo::Make(this->screenWidth, this->screenHeight, kRGBA_8888_SkColorType,
                                     kOpaque_SkAlphaType
    ));
    //转换为像素填充
    bitmap.allocPixels();
    //创建画布
    SkCanvas canvas(bitmap);
    //创建画笔
    SkPaint paint;
    //设置画布颜色
    canvas.clear(SK_ColorWHITE);
    //设置画笔抗锯齿
    paint.setAntiAlias(true);
    //设置画笔颜色(此处为红色)
    paint.setARGB(255, 255, 0, 0);
    //绘制圆形
    canvas.drawCircle(80, 80, 40, paint);
    //绘制线段
    canvas.drawLine(0, 280, this->screenWidth, 280, paint);
    //设置字体大小
    SkFont fonts;
    fonts.setSize(60);
    //绘制字体
    ostringstream ostr;
    ostr << "fps" << setprecision(2) << CalculateFrameRate();
    canvas.drawString(ostr.str().c_str(), 300, 550, fonts, paint);
    //返回SkBitmap对象
    this->skBitmap = bitmap;
}

void *GUI::getPixels() {
    return this->skBitmap.getPixels();

}

void GUI::createTexture() {
    glGenTextures(1, &this->textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, this->textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->screenWidth, this->screenHeight, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, this->getPixels());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GUI::updateTexture() {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, this->textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->screenWidth, this->screenHeight, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, this->getPixels());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GUI::activeTexture() {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->textureId);
    glUniform1i(glGetUniformLocation(g_programObject, "texId"), 1);
    glUniform1i(glGetUniformLocation(g_programObject, "isGUI"), 1);
    glActiveTexture(GL_TEXTURE0);
}

void GUI::inactiveTexture() {
    glUniform1i(glGetUniformLocation(g_programObject, "isGUI"), 0);
}

void GUI::init() {
    //一张平铺的Bitmap
    GLfloat vertices[] = {
            -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 0.0f, 0.0f
    };
    GLuint indices[] = {
            0, 1, 2,
            0, 2, 3
    };
    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(this->VAO);
    glGenBuffers(1, &this->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glGenBuffers(1, &this->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 20, vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 6, indices, GL_STATIC_DRAW);
    glEnableVertexAttribArray((GLuint) glGetAttribLocation(g_programObject, "vPosition"));
    glEnableVertexAttribArray((GLuint) glGetAttribLocation(g_programObject, "texPosition"));
    glVertexAttribPointer((GLuint) glGetAttribLocation(g_programObject, "vPosition"), 3, GL_FLOAT,
                          GL_FALSE, 5 * sizeof(float), 0
    );
    glVertexAttribPointer((GLuint) glGetAttribLocation(g_programObject, "texPosition"), 2, GL_FLOAT,
                          GL_FALSE, 5 * sizeof(float), (void *) (sizeof(float) * 3)
    );
    glBindVertexArray(0);
}

void GUI::vDraw() {
    CalculateFrameRate();
    this->draw();
    this->updateTexture();
    this->activeTexture();
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *) 0);
    glBindVertexArray(0);
    this->inactiveTexture();
}

WIDGET::WIDGET(string type) {}

WIDGET::~WIDGET() {}