#version 330


uniform sampler2D tex;

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

//Zmienne interpolowane
in float i_nl;
in float i_nl2;
in vec2 i_tc;

void main(void) {
    vec4 color=texture(tex,i_tc);
	pixelColor=vec4(color.rgb*i_nl*i_nl2,color.a);
}
