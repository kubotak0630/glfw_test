#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <sys/time.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>

#include "Sprite2D.h"


/** shader ****/
//#define GL_GLEXT_PROTOTYPES
//#include <GLFW/glfw3.h>
#include "glsl.h"





static GLuint vertShader;
static GLuint fragShader;
static GLuint programId;

Sprite2D* sprite_bg = NULL;
Sprite2D* sprite_rect = NULL;
Sprite2D* sprite_chara1 = NULL;
Sprite2D* sprite_chara2 = NULL;



//10bit texture
static unsigned short org_tex[] = {
    700, 700, 1023,     0,   0,   0,   1023, 1023, 1023,     0,   0,   0,
    1023,   0,   0,     0, 1023,   0,     0,   0, 1023 ,   1023, 1023, 1023,
    512,   0,   0,    0, 512,   0,      0,   0, 512 ,   512, 512, 512, 
    1023, 1023,   0,    1023,   0, 1023,      0, 1023, 1023 ,   1023, 1023, 1023
};

//8bit texture, R,G,B,A
static unsigned char tex2[] = {
    255, 255, 255, 255,     0,   0,   0, 255,   255, 255, 255 ,255,     0,   0,   0, 255,
    255,   0,   0, 255,     0, 255,   0, 255,     0,   0, 255 ,255,   255, 255, 255, 255,
    128,   0,   0, 255,     0, 128,   0, 255,     0,   0, 128 ,255,   128, 128, 128, 255,
    255, 255,   0, 255,   255,   0, 255, 255,     0, 255, 255 ,255,   255, 255, 255, 255
};



int g_hoge[10] = {1,2,3,4,5,6,7,8,9,10};








void myReshape(GLFWwindow* window, int width, int height)
{

    //printf("%d, %d\n", width, height);
    
    glViewport(0, 0, width, height);
    
}

void init_glsl(void)
{
    

    /* シェーダプログラムのコンパイル／リンク結果を得る変数 */
    GLint compiled, linked;


    /* シェーダオブジェクトの作成 */
    vertShader = glCreateShader(GL_VERTEX_SHADER);
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    /* シェーダのソースプログラムの読み込み */
    if (readShaderSource(vertShader, "simple.vert")) exit(1);
    if (readShaderSource(fragShader, "simple.frag")) exit(1);

    /* バーテックスシェーダのソースプログラムのコンパイル */
    glCompileShader(vertShader);
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compiled);
    printShaderInfoLog(vertShader);
    if (compiled == GL_FALSE) {
        fprintf(stderr, "Compile error in vertex shader.\n");
        exit(1);
    }

    /* フラグメントシェーダのソースプログラムのコンパイル */
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);
    printShaderInfoLog(fragShader);
    if (compiled == GL_FALSE) {
        fprintf(stderr, "Compile error in fragment shader.\n");
        exit(1);
    }

    /* プログラムオブジェクトの作成 */
    programId = glCreateProgram();

    /* シェーダオブジェクトのシェーダプログラムへの登録 */
    glAttachShader(programId, vertShader);
    glAttachShader(programId, fragShader);

    /* シェーダオブジェクトの削除 */
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    /* シェーダプログラムのリンク */
    glLinkProgram(programId);
    glGetProgramiv(programId, GL_LINK_STATUS, &linked);
    printProgramInfoLog(programId);
    if (linked == GL_FALSE) {
        fprintf(stderr, "Link error.\n");
        exit(1);
    }

    /* シェーダプログラムの適用 */
    glUseProgram(programId);

    //GLuint gTexSamplerLoc;
    //gTexSamplerLoc = glGetUniformLocation ( gProgram, "voltexture" );
    glUniform1i(glGetUniformLocation(programId, "texture0"), 0);//テクスチャユニット0とする

    //glUniform1i(glGetUniformLocation(programId, "hogehoge"), 10);  //set hogehoge = 10
    glUniform1iv(glGetUniformLocation(programId, "hoge_ary"), 10, g_hoge);  //set hogehoge = 10



}

void create_2d_prj_mat(float size_x, float size_y, float* mat)
{

    mat[0] = 2.0f/size_x;
    mat[1] = 0.0f;
    mat[2] = 0.0f;
    mat[3] = 0.0f;
    mat[4] = 0.0f;
    mat[5] = -2.0f/size_y;
    mat[6] = 0.0f;
    mat[7] = 0.0f;
    mat[8] = 0.0f;
    mat[9] = 0.0f;
    mat[10] = 1.0f;
    mat[11] = 0.0f;
    mat[12] = -1.0f;
    mat[13] = 1.0f;
    mat[14] = 0.0f;
    mat[15] = 1.0f;
}





//index 0-11
void setTexPos(int index, float* uv)
{

    const int unit_w = 32;
    const int unit_h = 32;

    int index_x = index % 3;
    int index_y = index / 3;

    int x0, y0, x1, y1;

    x0 = index_x * unit_w;
    y0 = index_y * unit_h;
    x1 = x0 + unit_w;
    y1 = y0 + unit_h;

    uv[0] = x0 / 128.f;
    uv[1] = y0 / 128.f;
    
    uv[2] = x0 / 128.f;
    uv[3] = y1 / 128.f;
    
    uv[4] = x1 / 128.f;
    uv[5] = y1 / 128.f;

    uv[6] = x1 / 128.f;
    uv[7] = y0 / 128.f;


}

int g_cnt = 0;

void change_bg_bmp(unsigned char* img)
{

    img[0] = img[0] + 1;

    img[5] = img[5] + 1;

    img[61] = img[61] + 1;
    img[62] = img[62] + 1;


}

void display(GLFWwindow *window)
{


    static struct timeval tv_now, tv_old;

    tv_old = tv_now;

    gettimeofday(&tv_now, NULL);

    double time_ms = (tv_now.tv_sec - tv_old.tv_sec) * 1000 + (tv_now.tv_usec - tv_old.tv_usec) / 1000.0;
    //printf("frame_time = %f\n", time_ms);

    
    

    glClear(GL_COLOR_BUFFER_BIT);
    
    
    GLfloat projection_mat[16];
    
    create_2d_prj_mat(640.f, 480.f, projection_mat);


    //uniform
    GLuint textureLoc = glGetUniformLocation(programId, "texture0");
    GLuint prjMatLoc = glGetUniformLocation(programId, "prj_mat");


    // uniform属性を設定する
    glUniform1i(textureLoc, 0);
    glUniformMatrix4fv(prjMatLoc, 1, GL_FALSE, projection_mat);


    

    /*** background *************************/
    GLfloat vertex_uv_bg[8] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
    };

    sprite_bg->setTexUV(vertex_uv_bg);

    change_bg_bmp(tex2);
    sprite_bg->updateTexture(tex2, 4, 4);

    sprite_bg->draw();




    /******* 1st chara ************************/

    GLfloat vertex_uv[8];
    
    static int tex_cnt = 0;
    if (g_cnt % 15 == 0) {
        tex_cnt = (tex_cnt >= 2) ? 0 : ++tex_cnt;
    }

   
    setTexPos(6 + tex_cnt, vertex_uv);
   


    static int deg_val = 0;

    deg_val = (deg_val + 2) % 360; 
    

    static float scale_val = 1.0f;

    if (scale_val > 2) {
        scale_val = 1.f;
    }
    else {
        scale_val += 0.01f;
    }
    scale_val = 1.f;



    static int xt = 0;
    xt = xt > 640 ? 0 : xt+1;


    sprite_chara1->setTexUV(vertex_uv);

    sprite_chara1->setTransVal(xt, 128);
    sprite_chara1->setRotateVal(deg_val, 16.0f, 16.0f);
    sprite_chara1->setScaleVal(1.0, 1.0);


    sprite_chara1->draw();


  
    /*** 2nd(rect) *******/
    sprite_rect->draw();

    /********* chara2(woman) ********************/

    float uv_2[] = {
        0.0f, 0.0f,
        32.0f/128.f, 0.0f,
        32.0f/128.f, 32.0f/128.0f,
        0.0f, 32.0f/128.0f
    };

    sprite_chara2->setTexUV(uv_2);

    sprite_chara2->setTransVal(300, 200);
    sprite_chara2->setScaleVal(2.0f, 2.0f);
    sprite_chara2->setRotateVal(30, 16, 16);
    sprite_chara2->draw();
    

    g_cnt++;
}


int main()
{

    if (!glfwInit()) {
        fprintf(stderr, "error glfwInit()\n");
        return 1;
    }


    GLFWwindow *window =  glfwCreateWindow(640, 480, "hello", NULL, NULL);

    if (window == NULL) {
        glfwTerminate();
        return 1;
    }

    //作成したウィンドウをOpenGLの処理対象にする
    glfwMakeContextCurrent(window);

    glfwSetWindowSizeCallback(window, myReshape);

    //Init GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "error glewInit()\n");
        return 1;
    }


    //sync
    glfwSwapInterval(1);

    glClearColor(0.0f, 0.5f, 0.5f, 0.0f);


    init_glsl();


    float vtx[] = {
        0.0f, 0.0f,
        0.0f, 480.0f,
        640.0f, 480.0f,
        640.0f, 0.0f
    };

    float vtx_chara[] = {
        0.0f, 0.0f,
        24.0f, 0.0f,
        24.0f, 32.0f,
        0.0f, 32.0f
    };

    GLfloat vtx_1[] = {
        100.0f, 50.0f,
        200.0f, 50.0f,
        200.0f, 100.0f,
        100.0f, 100.0f
    };

   sprite_bg = new Sprite2D(programId, vtx);

   GLfloat color_1[] = {
        1.0f, 0.0f, 0.0f, 1.0f,  //R,  R,G,B,A
        1.0f, 0.0f, 0.0f, 1.0f,  //R
        0.0f, 1.0f, 0.0f, 1.0f,  //G
        0.0f, 0.0f, 1.0f, 1.0f   //B
    };

   sprite_rect = new Sprite2D(programId, vtx_1, color_1);
   sprite_chara1 = new Sprite2D(programId, vtx_chara);
   sprite_chara2 = new Sprite2D(programId, vtx_chara);
   
    
    /*** Create Texture **************************/

    sprite_bg->setTexture(tex2, 4, 4);
    sprite_chara1->setTextureFromFile("515-sample01.bmp");
    sprite_chara2->setTextureFromFile("515-sample11.bmp");




    

    while (glfwWindowShouldClose(window) == GL_FALSE) {

        display(window);

        glfwSwapBuffers(window);
        
        //glfwWaitEvents();
        glfwPollEvents();

    }


    printf("**** finish ******\n");

    glfwTerminate();


    return 0;
}
