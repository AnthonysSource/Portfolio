#version 460 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D sprite;

void main(){
	vec4 texel = texture(sprite, TexCoord);
	if(texel.a < 0.5){
		discard;
	}
	FragColor = texel;
}