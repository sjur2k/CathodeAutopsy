#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D Texture;
uniform vec4 tintColor;

void main(){
    vec4 texColor = texture(Texture, TexCoord);
    FragColor = vec4(tintColor.rgb, texColor.a * tintColor.a);
}