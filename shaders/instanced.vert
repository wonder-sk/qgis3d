#version 150 core

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec3 pos;

out vec3 worldPosition;
out vec3 worldNormal;
out vec3 kd;

uniform mat4 modelView;
uniform mat3 modelViewNormal;
uniform mat4 mvp;

uniform vec3 kdx;

void main()
{
    vec3 offsetPos = vertexPosition + pos;

    worldNormal = normalize(modelViewNormal * vertexNormal);
    worldPosition = vec3(modelView * vec4(offsetPos, 1.0));
    kd = kdx; //vec3(1.0, 1.0, 1.0);

    gl_Position = mvp * vec4(offsetPos, 1.0);
}
