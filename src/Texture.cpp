#include"Texture.h"
Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum
	pixelType)
{
	type = texType;
	int widthImg, heightImg, numColCh;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);
	glGenTextures(1, &ID);
	glActiveTexture(slot);
	glBindTexture(texType, ID);
	/*Wsp�rz�dne tekstury zazwyczaj mieszcz� si� w zakresie od (0,0) do (1,1), ale
	co si� stanie, je�li podamy wsp�rz�dne poza ten zakres? Domy�lne
	zachowanie OpenGL polega na powtarzaniu obraz�w tekstury (w zasadzie ignorujemy
	cz�� ca�kowit� zmiennoprzecinkowych wsp�rz�dnych tekstury), ale
	istnieje wi�cej opcji oferowanych przez OpenGL:*/
	/*GL_REPEAT: Domy�lne zachowanie dla tekstur. Powtarza obraz tekstury.
	GL_MIRRORED_REPEAT: To samo co GL_REPEAT, ale odbija obraz przy ka�dym
	powt�rzeniu.
	GL_CLAMP_TO_EDGE: Ogranicza wsp�rz�dne mi�dzy 0 a 1. Skutkiem tego jest to, �e
	wy�sze wsp�rz�dne s� ograniczane do kraw�dzi.
	GL_CLAMP_TO_BORDER: Wsp�rz�dne poza zakresem otrzymuj� teraz okre�lony przez
	u�ytkownika kolor obramowania.*/
	//glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	//glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	/*Filtrowanie tekstur mo�e by� ustawione dla operacji powi�kszania i
	pomniejszania (przy zmianie skali w g�r� lub w d�), dzi�ki czemu na przyk�ad
	mo�na u�y� filtrowania s�siad�w najbli�szych, gdy tekstury s� zmniejszane, oraz
	filtrowania liniowego dla tekstur powi�kszonych. Dlatego te� musimy
	okre�li� metod� filtrowania dla obu opcji za pomoc� glTexParameter*/
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Dodatkowe linie dla GL_CLAMP_TO_BORDER
	// float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);
	glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType,
		bytes);
	// Mipmapy
	/*GL_NEAREST_MIPMAP_NEAREST: wybiera najbli�szy poziom mipmapy pasuj�cy do
	rozmiaru piksela i u�ywa interpolacji najbli�szych s�siad�w do pr�bkowania tekstury.
	GL_LINEAR_MIPMAP_NEAREST: wybiera najbli�szy poziom mipmapy i pr�bkuje ten
	poziom za pomoc� interpolacji liniowej.
	GL_NEAREST_MIPMAP_LINEAR: interpoluje liniowo mi�dzy dwoma mipmapami, kt�re
	najbardziej odpowiadaj� rozmiarowi piksela, i pr�bkuje
	poziom interpolowany za pomoc� interpolacji najbli�szych s�siad�w.
	GL_LINEAR_MIPMAP_LINEAR: interpoluje liniowo mi�dzy dwoma najbli�szymi mipmapami
	i pr�bkuje poziom interpolowany za pomoc� interpolacji liniowej.*/
	glGenerateMipmap(texType);
	stbi_image_free(bytes);
	glBindTexture(texType, 0);
}
void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);
	shader.Activate();
	glUniform1i(texUni, unit);
}
void Texture::Bind()
{
	glBindTexture(type, ID);
}
void Texture::Unbind()
{
	glBindTexture(type, 0);
}
void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}
void Texture::SetFiltering(GLenum minFilter, GLenum magFilter)
{
	Bind();
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, magFilter);
	Unbind();
}
void Texture::SetBlur(float blurStrength)
{
	Bind();
	glTexParameterf(type, GL_TEXTURE_LOD_BIAS, blurStrength);
	Unbind();
}