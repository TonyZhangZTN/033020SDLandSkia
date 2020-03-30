#version 300 es
in vec4 vPosition;
in vec2 texPosition;
in vec3 normal;
in vec4 aColor;
uniform mat4 mvp;
out vec2 outPos;
out vec4 vColor;
out vec3 fsNormal;
invariant gl_Position;
uniform mediump int isGUI;
void main()
{
    if(isGUI == 0){
        fsNormal = (mvp * vec4(normal, 0.0f)).xyz;
        vColor = aColor;
        outPos = texPosition;
        gl_Position = mvp * vPosition;
    }else{
        vColor = aColor;
        outPos = texPosition;
        gl_Position = vPosition;
    }
}