#version 450

layout (push_constant) uniform RoundedBox {
    vec4 rect;
    vec4 radii;
    vec4 borderWidths;
    vec4 color;
    vec4 borderColor;
};

layout (origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;
layout (location = 0) out vec4 outColor;

float roundedBoxDistance(vec2 p, vec2 pos, vec2 size, float r) {
    vec2 halfSize = size / 2.0;
    vec2 center = pos + halfSize;
    r = min(min(r, halfSize.x), min(r, halfSize.y));
    vec2 d = p - center;
    vec2 q = abs(d) - (halfSize - r);
    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - r;
}

void main() {
    vec2 p = gl_FragCoord.xy;
    vec2 pos = rect.xy;
    vec2 size = rect.zw;
    float d = roundedBoxDistance(p, pos, size, radii.x);
    if (d > 2.0) discard;
    float alpha = 1.0 - smoothstep(0.0, 2.0, d);
    outColor = color * alpha;

    float borderWidth;
    if (p.x < pos.x) borderWidth = borderWidths.x; // left
    if (p.y < pos.y) borderWidth = borderWidths.y; // top
    if (p.x > (pos.x + size.x)) borderWidth = borderWidths.z; // right
    if (p.x > (pos.y + size.y)) borderWidth = borderWidths.w; // bottom

    if (borderWidth > 0.0 && d > -borderWidth) {
        float borderAlpha = 1.0 - smoothstep(-borderWidth, -borderWidth + 1.0, d);
        outColor = mix(borderColor, color, borderAlpha) * alpha;
    }
}