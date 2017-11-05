//#version 120

// simple.frag

uniform sampler2D texture0;//テクスチャユニット0

varying vec2 v_uv;
varying vec4 v_color;

uniform int hoge_ary[10];
uniform int use_tex_flg;

void main (void)
{

  //gl_FragColor = texture2D(texture0, v_uv);

  if (use_tex_flg == 1) {
    gl_FragColor = texture2D(texture0, v_uv);
  }
  else {
    gl_FragColor = v_color;
    //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
  }

  //gl_FragColor = v_color;
}
