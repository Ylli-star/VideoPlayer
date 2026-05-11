#version 330 core
in vec2 tex;
uniform sampler2D yvalue;
uniform sampler2D uvalue;
uniform sampler2D vvalue;
out vec4 FragColor;

void main()
{
  float y = texture(yvalue, tex).r;
  float u = texture(uvalue, tex).r - 0.5;
  float v = texture(vvalue, tex).r - 0.5;

  float r = y + 1.140*v;
  float g = y -0.359*u -0.581*v;
  float b = y + 2.032*u;

  FragColor = vec4(r, g, b, 1.0);
}