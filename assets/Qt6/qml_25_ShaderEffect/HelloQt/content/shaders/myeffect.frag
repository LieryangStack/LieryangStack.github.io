#version 440
layout(location = 0) in vec2 coord;
layout(location = 0) out vec4 fragColor;

layout(location = 1) in float vPosY;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
};
layout(binding = 1) uniform sampler2D src;

void main() {
    vec4 tex = texture(src, coord);

    float alpha = (coord.y + 1.0) / 2.0;
    // fragColor = vec4(0.0, (coord.y + 1.0)/2.0, 0.0, 1.0);
    // fragColor = vec4(vec3(dot(tex.rgb, vec3(0.344, 0.5, 0.156))), tex.a) * qt_Opacity;

    // 定义浅绿色和白色
    vec4 green = vec4(0.5, 1.0, 0.5, 1.0);
    vec4 white = vec4(1.0, 1.0, 1.0, 1.0);

    // 使用 t 进行颜色的线性插值
    fragColor = mix(green, white, alpha);
}
