#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Sprite2D.h"

Sprite2D::Sprite2D(GLuint program_id, float* vertex, float* color)
    :_program_id(program_id),
     _texture_enable(false)
{

    glGenBuffers(3, _vbo_buf);  //vertex, color, uv


    //Init Matrix
    initMatIdent(_trans_mat);
    initMatIdent(_rotate_mat);
    initMatIdent(_scale_mat);


    //init _pivot[2]
    _pivot[0] = 0.0f;
    _pivot[1] = 0.0f;

    //set Vertex to VBO
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_buf[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*8, vertex, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);  //unbind VBO

    //set Color to VBO
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_buf[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*16, color, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);  //unbind VBO
    
}

Sprite2D::~Sprite2D()
{

    glDeleteBuffers(3, _vbo_buf);

    if (_texture_enable) {
        glDeleteTextures(1, &_texture_id);
    }

}

void Sprite2D::initMatIdent(float* mat)
{

    mat[0] = 1.0f;
    mat[1] = 0.0f;
    mat[2] = 0.0f;
    mat[3] = 0.0f;
    mat[4] = 0.0f;
    mat[5] = 1.0f;
    mat[6] = 0.0f;
    mat[7] = 0.0f;
    mat[8] = 0.0f;
    mat[9] = 0.0f;
    mat[10] = 1.0f;
    mat[11] = 0.0f;
    mat[12] = 0.0f;
    mat[13] = 0.0f;
    mat[14] = 0.0f;
    mat[15] = 1.0f;
}



void Sprite2D::setVertex(float* vertex)
{
 

    //set Vertex to VBO
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_buf[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*8, vertex, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);  //unbind VBO

}

void Sprite2D::setColor(float* color)
{

    //set Color to VBO
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_buf[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*16, color, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);  //unbind VBO


}

void Sprite2D::setTexUV(float* uv)
{

    for (int i = 0; i < 8; i++) {
        _uv[i] = uv[i];
    }    
}


// ex) a= 34, return 64
int Sprite2D::forcePow2(int a) 
{
    if (a <= 0) {
        fprintf(stderr, "input error\n");
        exit(1);
    }

    int cnt = 0;
    while (a % 2 == 0) {
        a = a / 2;
        cnt++;
    }


    int b = 1;
    while (a > b) {
        b = b*2;
    }

    int ret_val = (int)(pow(2, cnt)) * b;

    //printf("ans = %d\n", ret_val);

    return ret_val;

}

//read bitmap
void Sprite2D::setTextureFromFile(const char* fname)
{

    FILE* fp = fopen(fname, "rb");

    if (fp == NULL) {
        fprintf(stderr, "error, setTextureFromFile(), cannot file open\n");
        exit(1);
    }


    typedef struct _bitMapInfoHeader {
        uint32_t biSize;
        int32_t biWidth;
        int32_t biHeight;
        uint16_t biPlanes;
        uint16_t biBitCount;
        uint32_t biCompression;
        uint32_t biSizeImage;
        int32_t biXPixPerMeter;
        int32_t biYPixPerMeter;
        uint32_t biClrUserd;
        uint32_t biCirImportant;
    } bitMapInfoHeader_t;

    bitMapInfoHeader_t bitmapInfo;

    fseek(fp, 14, SEEK_SET);
    fread(&bitmapInfo, sizeof(bitmapInfo), 1, fp);


    printf("widht = %d\n", bitmapInfo.biWidth);
    printf("height = %d\n", bitmapInfo.biHeight);
    printf("bitCount = %d\n", bitmapInfo.biBitCount);

    int width_pow2 = forcePow2(bitmapInfo.biWidth);
    int height_pow2 = forcePow2(bitmapInfo.biHeight);

    
    //alloc temp buffer
    uint8_t *pBufByte = new uint8_t[bitmapInfo.biWidth * bitmapInfo.biHeight * 4];

    //alloc texture buffer
    uint32_t* pData = new uint32_t[width_pow2 * height_pow2];

    //skip header(14Byte + 40Byte = 54Byte)
    fseek(fp, 54, SEEK_SET);
    fread(pBufByte, 1, bitmapInfo.biWidth * bitmapInfo.biHeight * 4, fp);

    uint8_t R,G,B,A;
    if (bitmapInfo.biBitCount == 32) {
                
        for (int y = 0; y < bitmapInfo.biHeight; y++) {
            for (int x = 0; x < bitmapInfo.biWidth; x++) {

                B = pBufByte[(bitmapInfo.biHeight - y -1) * bitmapInfo.biWidth*4 + 4*x + 0];
                G = pBufByte[(bitmapInfo.biHeight - y -1) * bitmapInfo.biWidth*4 + 4*x + 1];
                R = pBufByte[(bitmapInfo.biHeight - y -1) * bitmapInfo.biWidth*4 + 4*x + 2];
                A = pBufByte[(bitmapInfo.biHeight - y -1) * bitmapInfo.biWidth*4 + 4*x + 3];
                pData[width_pow2 * y +  x] = A << 24 | B << 16 | G << 8 | R;
            }
        }
    }
    else {

        fprintf(stderr, "support only 32bit Bitmap file\n");
        exit(1);
    }


    delete[] pBufByte;


    setTexture((uint8_t*)pData, width_pow2, height_pow2);


    delete[] pData;
    pData = NULL;



    fclose(fp);

}

//only support RGBA, size_x, size_y is power of 2.
void Sprite2D::setTexture(unsigned char* img, int width_powOf2, int height_powOf2)
{

    glGenTextures(1, &_texture_id);

    glBindTexture(GL_TEXTURE_2D , _texture_id);

    /* テクスチャ画像はバイト単位に詰め込まれている */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_powOf2, height_powOf2, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // 拡大時近傍
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // 縮小時近傍

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA , GL_ONE);
    //glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    _texture_enable = true;

}

 // size_x, size_y is ok at not power of 2
void Sprite2D::updateTexture(unsigned char* img, int size_x, int size_y)
{

    if (_texture_enable == false) {
        fprintf(stderr, "fatal error. updateTexture()\n");
        exit(1);
    }

    glBindTexture(GL_TEXTURE_2D , _texture_id);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size_x, size_y, GL_RGBA, GL_UNSIGNED_BYTE, img);


}

void Sprite2D::draw()
{

    //get AttribLocation
    GLuint positionLoc = glGetAttribLocation(_program_id, "position");
    GLuint uvLoc = glGetAttribLocation(_program_id, "uv");
    GLuint colorLoc = glGetAttribLocation(_program_id, "color");

    //get UniformLocation
    GLuint wldMatLoc = glGetUniformLocation(_program_id, "wld_mat");
    GLuint pivotLoc = glGetUniformLocation(_program_id, "pivot");   


    float wld_mat[16];
    float temp_mat[16];

    mat4_mul(_trans_mat, _rotate_mat, temp_mat);
    mat4_mul(temp_mat, _scale_mat, wld_mat);

    glUniform2fv(pivotLoc, 1, _pivot);
    glUniformMatrix4fv(wldMatLoc, 1, GL_FALSE, wld_mat);

    /* １つ目のバッファオブジェクトに頂点データ配列を転送する */    
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_buf[0]);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(_vertex), _vertex, GL_STATIC_DRAW);
    glEnableVertexAttribArray(positionLoc);
    glVertexAttribPointer(positionLoc, 2, GL_FLOAT, false, 0, NULL);
    
    //glEnableVertexAttribArray(positionLoc);

    /* 2つ目のバッファオブジェクトに頂点colorを転送する */
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_buf[1]);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(_color), _color, GL_STATIC_DRAW);

    glEnableVertexAttribArray(colorLoc);
    glVertexAttribPointer(colorLoc, 4, GL_FLOAT, false, 0, NULL);


    glUniform1i(glGetUniformLocation(_program_id, "use_tex_flg"), _texture_enable);

    if (_texture_enable) {


        /* 3つ目のバッファオブジェクトにtexture_uv データ配列を転送する */
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_buf[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(_uv), _uv, GL_STATIC_DRAW);

        glEnableVertexAttribArray(uvLoc);
        glVertexAttribPointer(uvLoc, 2, GL_FLOAT, false, 0, NULL);


        //change Textue

        glBindTexture(GL_TEXTURE_2D , _texture_id);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // 拡大時近傍
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // 縮小時近傍
    }


    glDrawArrays(GL_QUADS, 0, 4);


    glBindBuffer(GL_ARRAY_BUFFER, 0);  //unbind VBO


    glDisableVertexAttribArray(positionLoc);
    glDisableVertexAttribArray(colorLoc);
    glDisableVertexAttribArray(uvLoc);

}

void Sprite2D::mat4_mul(float* a, float* b, float* ans)
{

    //ans_mat[0] = a[0]*b[0] + a[4]*b[1] + a[8]*b[2] * a[12]*b[3];
    //ans_mat[1] = a[1]*b[0] + a[5]*b[1] + a[9]*b[2] * a[13]*b[3];
    //ans_mat[2] = a[2]*b[0] + a[6]*b[1] + a[10]*b[2] *a[14]*b[3];
    //ans_mat[3] = a[3]*b[0] + a[7]*b[1] + a[11]*b[2] *a[15]*b[3];
    
    //ans_mat[4] = a[0]*b[4] + a[4]*b[5] + a[8]*b[6] *a[12]*b[7];
    //ans_mat[5] = a[1]*b[4] + a[5]*b[5] + a[9]*b[6] *a[13]*b[7];

    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {

            ans[j*4 + i] = a[i+0]*b[4*j+0] + a[i+4]*b[4*j+1] + a[i+8]*b[4*j+2] + a[i+12]*b[4*j+3];

            //printf("a[%d] = %f\n", j*4+i, ans[j*4+i]);
        }
    }
}

void Sprite2D::setTransMat(float* mat)
{
    for (int i = 0; i < 16; i++) {
        _trans_mat[i] = mat[i];
    }
}

void Sprite2D::setRotateMat(float* mat)
{
    for (int i = 0; i < 16; i++) {
        _rotate_mat[i] = mat[i];
    }
}

void Sprite2D::setScaleMat(float* mat)
{
    for (int i = 0; i < 16; i++) {
        _scale_mat[i] = mat[i];
    }
}

void Sprite2D::setTransVal(int x, int y)
{

    _trans_mat[0] = 1.0f;
    _trans_mat[1] = 0.0f;
    _trans_mat[2] = 0.0f;
    _trans_mat[3] = 0.0f;
    _trans_mat[4] = 0.0f;
    _trans_mat[5] = 1.0f;
    _trans_mat[6] = 0.0f;
    _trans_mat[7] = 0.0f;
    _trans_mat[8] = 0.0f;
    _trans_mat[9] = 0.0f;
    _trans_mat[10] = 1.0f;
    _trans_mat[11] = 0.0f;
    _trans_mat[12] = (float)x;
    _trans_mat[13] = (float)y;
    _trans_mat[14] = 0.0f;
    _trans_mat[15] = 1.0f;

}

void Sprite2D::setScaleVal(int scale_x, int scale_y)
{
    _scale_mat[0] = scale_x;
    _scale_mat[1] = 0.0f;
    _scale_mat[2] = 0.0f;
    _scale_mat[3] = 0.0f;
    _scale_mat[4] = 0.0f;
    _scale_mat[5] = scale_y;
    _scale_mat[6] = 0.0f;
    _scale_mat[7] = 0.0f;
    _scale_mat[8] = 0.0f;
    _scale_mat[9] = 0.0f;
    _scale_mat[10] = 0.0f;
    _scale_mat[11] = 0.0f;
    _scale_mat[12] = 0.0f;
    _scale_mat[13] = 0.0f;
    _scale_mat[14] = 0.0f;
    _scale_mat[15] = 1.0f;

}

void Sprite2D::setRotateVal(double degree_val, float pivot_x, float pivot_y)
{

    double rad = degree_val * M_PI / 180.0;

    _pivot[0] = pivot_x;
    _pivot[1] = pivot_y;


    _rotate_mat[0] = (float)(cos(rad));
    _rotate_mat[1] = (float)(sin(rad));
    _rotate_mat[2] = 0.0f;
    _rotate_mat[3] = 0.0f;
    _rotate_mat[4] = (float)(-sin(rad));
    _rotate_mat[5] = (float)(cos(rad));
    _rotate_mat[6] = 0.0f;
    _rotate_mat[7] = 0.0f;
    _rotate_mat[8] = 0.0f;
    _rotate_mat[9] = 0.0f;
    _rotate_mat[10] = 1.0f;
    _rotate_mat[11] = 0.0f;
    _rotate_mat[12] = 0.0f;
    _rotate_mat[13] = 0.0f;
    _rotate_mat[14] = 0.0f;
    _rotate_mat[15] = 1.0f;
}
