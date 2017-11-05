//#version 120

// simple.vert

attribute vec2 position;
attribute vec2 uv;
attribute vec4 color;


uniform vec2 pivot;


uniform mat4 prj_mat;
uniform mat4 wld_mat;

varying vec2 v_uv;
varying vec4 v_color;

void main(void)
{
  // 頂点位置
/*
  gl_Position = ftransform();
   //gl_Position.y = -gl_Position.y; // 上下反転
   gl_TexCoord[0] = gl_MultiTexCoord0; // 0番目のテクスチャを貼るポリゴンの座標取得
*/

  vec4 pos_out;
  pos_out = vec4(position, 1.0, 1.0) - vec4(pivot, 0.0, 0.0); 
  pos_out = wld_mat * pos_out;
  pos_out = pos_out + vec4(pivot, 0.0, 0.0); 

  gl_Position = prj_mat * pos_out;
  //gl_Position = prj_mat * wld_mat * vec4(position, 1.0, 1.0);
  //gl_Position = prj_mat * vec4(position, 1.0);
  //gl_Position = vec4(position, 1.0);
  v_uv = uv;
  v_color = color;
}
