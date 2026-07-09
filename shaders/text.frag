#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D glyph;
uniform vec4 textColor;

void main(){
    float alpha = texture(glyph, TexCoords).r;
    FragColor = vec4(textColor.rgb, textColor.a*alpha);
}