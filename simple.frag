#version 120

// simple.frag

uniform sampler2D texture0; //テクスチャユニット0

varying vec2 v_texCoord;
varying vec4 v_color;

//uniform int hoge_ary[10];
uniform int useTexture_flg;
uniform int gain_flg;

//uniform float black_offset = 0.03125;
uniform float y_gain;
uniform float cb_gain;
uniform float cr_gain;




void main (void)
{

  const mat3 RGB2YUV =  mat3(0.299, -0.169, 0.500, 
                             0.587, -0.331, -0.419,
                             0.114, 0.500, -0.081);

  const mat3 YUV2RGB = mat3(1.000, 1.000, 1.000,
                            0.0,   -0.344, 1.772,
                            1.402, -0.714, 0.0);

  vec4 texelColor;

  if (useTexture_flg == 1) {
    texelColor = texture2D(texture0, v_texCoord);
  }
  else {
    texelColor = v_color;
    //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
  }

  vec3 rgbColor;

  if (gain_flg == 1) {
    //Convert RGB->YUV
    vec3 yuvColor;
    yuvColor = RGB2YUV * vec3(texelColor.rgb);

    //y, chrom gain
    yuvColor[0] *= y_gain;
    yuvColor[1] *= cb_gain;
    yuvColor[2] *= cr_gain;
  
    //Convert YUV->RGB
    rgbColor = YUV2RGB * yuvColor;
  }
  else {
    rgbColor = texelColor.rgb;
  }

  gl_FragColor = vec4(rgbColor, texelColor[3]);
}
