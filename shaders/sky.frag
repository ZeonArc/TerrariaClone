#version 330 core
in vec2 ScreenPos;
out vec4 FragColor;

uniform float cameraX;

// A standard GLSL hash function for pseudo-randomness
float hash(vec2 p) {
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

// A standard GPU noise generator
float noise(vec2 x) {
    vec2 i = floor(x);
    vec2 f = fract(x);
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

void main() {
    // 1. Calculate the Sky Gradient
    vec3 topColor = vec3(0.2, 0.5, 0.9);
    vec3 bottomColor = vec3(0.6, 0.8, 1.0);
    
    float gradient = (ScreenPos.y + 1.0) * 0.5;
    vec3 skyColor = mix(bottomColor, topColor, gradient);

    // 2. Calculate Parallax Clouds
    vec2 cloudPos = vec2((ScreenPos.x * 4.0) + (cameraX * 0.0005), ScreenPos.y * 4.0);
    
    float cloudNoise = noise(cloudPos);
    cloudNoise += 0.5 * noise(cloudPos * 2.0); 
    cloudNoise = smoothstep(0.4, 0.8, cloudNoise);
    
    // Smoothly fade the clouds out as they get closer to the bottom of the screen (Y = -1.0)
    float fadeOut = smoothstep(-0.2, 0.2, ScreenPos.y);
    cloudNoise *= fadeOut;
    
    // Blend the clouds into the sky
    skyColor = mix(skyColor, vec3(1.0, 1.0, 1.0), cloudNoise * 0.8);

    FragColor = vec4(skyColor, 1.0);
}