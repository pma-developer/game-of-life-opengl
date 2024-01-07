#version 330 core

in vec2 TexCoord;

uniform int GRID_WIDTH;
uniform int GRID_HEIGHT;

out vec4 FragColor;
uniform sampler2D computationTexture;
uniform sampler2D videoFrameTexture;

const vec4 live = vec4(1, 0, 0, 1);
const vec4 dead = vec4(0, 0, 0, 1);

#ifndef OVERRIDE_NEIGHBOR_COUNT_RULE
int countNeighbors(ivec2 selfCoord)
{
    int liveCount = 0;

    for(int y = -1; y <= 1; y++)
    {
        for(int x = -1; x <= 1; x++)
        {
            if(x == 0 && y == 0) continue;

            ivec2 neighborCoord = selfCoord + ivec2(x, y);

            if(neighborCoord.x < 0 || neighborCoord.x >= GRID_WIDTH || neighborCoord.y < 0 || neighborCoord.y >= GRID_HEIGHT)
                continue;

            vec4 neighborColor = texelFetch(computationTexture, neighborCoord, 0);
            if(neighborColor.r == live.r)
            {
                liveCount++;
            }
        }
    }
    return liveCount;
}
#endif

#ifndef OVERRIDE_LIFE_RULE
vec4 getNextState(int liveCount, vec4 selfState)
{
    if(selfState.r == live.r)
    {
        return liveCount == 2 || liveCount == 3 ? live : dead;
    }
    else
    {
        return liveCount == 3 ? live : dead;
    }
}
#endif

float sqrDistance(vec4 vec)
{
    return pow(vec.x, 2) + pow(vec.y, 2) + pow(vec.z, 2) + pow(vec.w, 2);
}

void outputNextState()
{
    ivec2 selfCoord = ivec2(gl_FragCoord.xy);
    vec4 selfState = texelFetch(computationTexture, selfCoord, 0);
    int liveCount = countNeighbors(selfCoord);

    FragColor = getNextState(liveCount, selfState);

    vec4 additionalTextureColor = texture(videoFrameTexture, TexCoord, 0);
    if(sqrDistance((additionalTextureColor - vec4(1, 1, 1, 1))) < 0.01f)
    {
        FragColor = live;
    }
}

void main()
{
    outputNextState();
}
