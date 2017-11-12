#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>



class Sprite2D
{

 private:

    GLuint _program_id;
    GLuint _vbo_buf;  //vertex, color, uv



    GLuint _texture_id;    //set value at setTexuter() function
    bool _texture_enable;  //set true at setTexuter() function

    float _pivot[2];  //user center at Rotate

    float _trans_mat[16];
    float _rotate_mat[16];
    float _scale_mat[16];

    float _y_gain;
    float _cb_gain;
    float _cr_gain;

    void initMatIdent(float* mat);

    void mat4_mul(float* a, float* b, float* ans);
    int forcePow2(int a);

    //Set Matrix at Direct 
    void setTransMat(float* mat);
    void setRotateMat(float* mat);
    void setScaleMat(float* mat);


 public:

    Sprite2D(GLuint program_id, float* vertex, float* color = NULL, float* uv = NULL);
    ~Sprite2D();
    
    void set_gain(float y_gain, float cb_gain, float cr_gain);
    void draw(bool blend_enable = true, bool gain_enable = false);



    void setVertex(float* vertex);
    void setTexture(unsigned char* img, int width_powOf2, int height_powOf2, bool is_nearestInterpol = true);
    void setTextureFromFile(const char* fname, bool is_nearestInterpol = true);
    void setTexUV(float* uv);
    void updateTexture(unsigned char* img, int size_x, int size_y); // size_x, size_y is ok at not power of 2

    void setColor(float* color);

    //Set Value instead of Matrix
    void setTransVal(int x, int y);  // set pixel
    void setRotateVal(double degree_val, float pivot_x, float pivot_y);  //0 <= degree_val <= 360
    void setScaleVal(int scale_x, int scale_y);

};



