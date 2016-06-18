#version 400
precision highp float;
uniform sampler2D image;
in vec2 vUv;
out vec4 fragColor;
void main() {
  fragColor = texture(image, vUv);
}
