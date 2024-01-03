#version 330 core

in vec4 screenPosition;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture1;
uniform float zoomLevel;
uniform vec2 zoomCenter;

void main()
{
    vec2 leftBottom = zoomCenter - vec2(0.5 / zoomLevel);
    vec2 rightTop = zoomCenter + vec2(0.5 / zoomLevel);

    vec2 sampleCoord;
    sampleCoord.x = mix(leftBottom.x, rightTop.x, TexCoord.x);
    sampleCoord.y = mix(leftBottom.y, rightTop.y, TexCoord.y);


    FragColor = texture(texture1, sampleCoord);
    //FragColor = vec4(zoomCenter, 0, 1);
}
