//
// Created on 2025/5/30.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
/**
 * 视频场景弹幕绘制低功耗规则
 */
#include "EGLCore.h"
#include "hilog/log.h"
#include <GLES3/gl3.h>
#include <math.h>

int LOG_PRINT_DOMAIN = 0;
bool flag_ = false;

GLfloat centerX;
GLfloat width_;
GLfloat centerY;
GLfloat height_;
GLfloat leftX;
GLfloat leftY;
GLfloat rotateX;
GLfloat rotateY;
GLfloat rightX;
GLfloat rightY;

int position;
int DRAW_COLOR;

bool ExecuteDrawStar(int position, int DRAW_COLOR, const GLfloat shapeVertices[], size_t test) { return false; };
void rotate2d(GLfloat centerX, GLfloat centerY, GLfloat *rotateX, GLfloat *rotateY, GLfloat rad){};
bool FinishDraw() { return false; }

// [Start egl_core]
void EGLCore::Draw() {
    // Determine the vertices of the quadrilateral to be drawn, expressed as a percentage of the drawing area
    const GLfloat shapeVertices[] = {centerX / width_, centerY / height_, leftX / width_,  leftY / height_,
                                     rotateX / width_, rotateY / height_, rightX / width_, rightY / height_};

    if (!ExecuteDrawStar(position, DRAW_COLOR, shapeVertices, sizeof(shapeVertices))) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "Draw execute draw star failed");
        return;
    }

    GLfloat rad = M_PI / 180 * 72;
    for (int i = 0; i < 4; ++i) {
        // Rotate the vertices of the other four quadrilaterals
        rotate2d(centerX, centerY, &rotateX, &rotateY, rad);
        rotate2d(centerX, centerY, &leftX, &leftY, rad);
        rotate2d(centerX, centerY, &rightX, &rightY, rad);

        // Determine the vertices of the quadrilateral to be drawn, expressed as a percentage of the drawing area
        const GLfloat shapeVertices[] = {centerX / width_, centerY / height_, leftX / width_,  leftY / height_,
                                         rotateX / width_, rotateY / height_, rightX / width_, rightY / height_};

        // Draw graphics
        if (!ExecuteDrawStar(position, DRAW_COLOR, shapeVertices, sizeof(shapeVertices))) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "Draw execute draw star failed");
            return;
        }
    }

    // End drawing
    if (!FinishDraw()) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "EGLCore", "Draw FinishDraw failed");
        return;
    }

    flag_ = true;
}
// [End egl_core]