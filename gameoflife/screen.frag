#version 330 core

in vec4 screenPosition;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float zoomLevel;
uniform vec2 cameraCenter;
uniform bool bwColor;

void main()
{
    vec2 leftBottom = cameraCenter - vec2(0.5 / zoomLevel);
    vec2 rightTop = cameraCenter + vec2(0.5 / zoomLevel);

    vec2 sampleCoord;
    sampleCoord.x = mix(leftBottom.x, rightTop.x, TexCoord.x);
    sampleCoord.y = mix(rightTop.y, leftBottom.y, TexCoord.y);

    
    FragColor = bwColor ? vec4(vec3(texture(texture1, sampleCoord).r), 1): texture(texture1, sampleCoord);
}
