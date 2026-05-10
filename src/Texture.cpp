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
	/*Wspó³rzêdne tekstury zazwyczaj mieszcz¹ siê w zakresie od (0,0) do (1,1), ale
	co siê stanie, jeœli podamy wspó³rzêdne poza ten zakres? Domyœlne
	zachowanie OpenGL polega na powtarzaniu obrazów tekstury (w zasadzie ignorujemy
	czêœæ ca³kowit¹ zmiennoprzecinkowych wspó³rzêdnych tekstury), ale
	istnieje wiêcej opcji oferowanych przez OpenGL:*/
	/*GL_REPEAT: Domyœlne zachowanie dla tekstur. Powtarza obraz tekstury.
	GL_MIRRORED_REPEAT: To samo co GL_REPEAT, ale odbija obraz przy ka¿dym
	powtórzeniu.
	GL_CLAMP_TO_EDGE: Ogranicza wspó³rzêdne miêdzy 0 a 1. Skutkiem tego jest to, ¿e
	wy¿sze wspó³rzêdne s¹ ograniczane do krawêdzi.
	GL_CLAMP_TO_BORDER: Wspó³rzêdne poza zakresem otrzymuj¹ teraz okreœlony przez
	u¿ytkownika kolor obramowania.*/
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	/*Filtrowanie tekstur mo¿e byæ ustawione dla operacji powiêkszania i
	pomniejszania (przy zmianie skali w górê lub w dó³), dziêki czemu na przyk³ad
	mo¿na u¿yæ filtrowania s¹siadów najbli¿szych, gdy tekstury s¹ zmniejszane, oraz
	filtrowania liniowego dla tekstur powiêkszonych. Dlatego te¿ musimy
	okreœliæ metodê filtrowania dla obu opcji za pomoc¹ glTexParameter*/
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Dodatkowe linie dla GL_CLAMP_TO_BORDER
	// float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);
	glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType,
		bytes);
	// Mipmapy
	/*GL_NEAREST_MIPMAP_NEAREST: wybiera najbli¿szy poziom mipmapy pasuj¹cy do
	rozmiaru piksela i u¿ywa interpolacji najbli¿szych s¹siadów do próbkowania tekstury.
	GL_LINEAR_MIPMAP_NEAREST: wybiera najbli¿szy poziom mipmapy i próbkuje ten
	poziom za pomoc¹ interpolacji liniowej.
	GL_NEAREST_MIPMAP_LINEAR: interpoluje liniowo miêdzy dwoma mipmapami, które
	najbardziej odpowiadaj¹ rozmiarowi piksela, i próbkuje
	poziom interpolowany za pomoc¹ interpolacji najbli¿szych s¹siadów.
	GL_LINEAR_MIPMAP_LINEAR: interpoluje liniowo miêdzy dwoma najbli¿szymi mipmapami
	i próbkuje poziom interpolowany za pomoc¹ interpolacji liniowej.*/
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