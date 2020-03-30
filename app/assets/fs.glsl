#version 300 es
precision mediump float;
out vec4 fragColor;
in vec2 outPos;
in vec4 vColor;
in vec3 fsNormal;
uniform sampler2D texId;
uniform sampler2D TDTex;
uniform int isGUI;
layout(std140) uniform Light{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

void main()
{
//    vec2 pos = vec2(1.0f - outPos.x,1.0f - outPos.y);
//    fragColor = texture(texId,outPos);
//    fragColor = (vColor + texture(texId,outPos))*vec4(vec3(gl_FragCoord.z),1);
    if(isGUI == 0){
        float ambientVal = 0.2f;
        vec4 ambientColor = vec4(ambient*vec4(ambientVal, ambientVal, ambientVal, 1.0f));
        vec3 lightDir = vec3(0.0f, -10.0f, -1.0f);
        float lightVal = 1.0f;
        vec3 lightColor = vec3(lightVal, lightVal, lightVal);
        float diffuseFactor = dot(normalize(fsNormal), normalize(-lightDir));
        vec4 diffuseColor;
        if (diffuseFactor>0.0f)
        {
            diffuseColor = vec4(lightColor*(diffuse.xyz+ambient.xyz)*diffuseFactor, 1.0f);
        }
        else
        {
            diffuseColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
        }

        fragColor = texture(texId, outPos)*(ambientColor + diffuseColor);
    }else{
        //if(texture(texId, outPos).xyz != vec3(1.0f,1.0f,1.0f))
        fragColor = texture(TDTex, vec2(1.0f - outPos.x,1.0f - outPos.y))*0.5+texture(texId,outPos)*0.6;
        //else
        //fragColor=fragColor;
        //fragColor = vec4(1.0f,1.0f,1.0f,1.0f);
    }
    //fragColor = texture(texId,outPos)*ambientColor;
}