#ifndef Renderer_h
#define Renderer_h

#include <unordered_map>
#include <string>
#include <filesystem>
#include <cmath>

#include "Actor.hpp"
#include "Helper.h"
#include "glm/glm.hpp"
#include "SDL2_image/SDL_image.h"
#include "SDL2_ttf/SDL_ttf.h"

class TextRenderRequest {
public:
	std::string text;
	std::string font;
	SDL_Color color;
	int size;
	int x;
	int y;

	TextRenderRequest(
		std::string text, std::string font, SDL_Color color,
		int size, int x, int y);
};

class UIRenderRequest {
public:
	std::string image;
	SDL_Color color = { 255, 255, 255, 255 };
	float x;
	float y;
	int sorting_order;

	UIRenderRequest(std::string image, SDL_Color color, float x, float y, int sorting_order);
};

class ImageRenderRequest {
public:
	std::string image;
	SDL_Color color;
	float x;
	float y;
	int sorting_order;
	float rotation;
	float scale_x;
	float scale_y;
	float pivot_x;
	float pivot_y;

	ImageRenderRequest(std::string image, SDL_Color color,
		float x, float y, int sorting_order, float rotation,
		float scale_x, float scale_y, float pivot_x, float pivot_y);
};

class DrawPixelRequest {
public:
	SDL_Color color;
	int x;
	int y;

	DrawPixelRequest(SDL_Color color, float x, float y);
};

class Renderer
{
private:

	static inline std::unordered_map<std::string, SDL_Texture*> textures;
	
public:

	// Window Resolution
	static inline int x_resolution = 640; // width
	static inline int y_resolution = 360; // height

	// Camera Properties
	static inline glm::vec2 cam_position = glm::vec2(0, 0);
	static inline float zoom_factor = 1.0f;
	float cam_ease_factor = 1.0f;

	// SDL clear color
	static inline int clear_color_r = 255;
	static inline int clear_color_g = 255;
	static inline int clear_color_b = 255;

	// SDL Window and Renderer
	static inline SDL_Window* window;
	static inline SDL_Renderer* renderer;

	// Intro images and texts
	std::vector<std::string> intro_image_names;
	std::vector<std::string> intro_texts;

	// Font specification
	static inline std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> fonts;

	static float GetCameraZoomFactor();

	static glm::ivec2 GetCameraDimension();

	static glm::vec2 GetCameraPosition();

	static SDL_Texture* GetTexture(const std::string& name);

	//static SDL_RendererFlip GetRendererFlip(bool horizontalFlip, bool verticalFlip);

	static void SetCameraPosition(float x, float y);

	static float GetCameraPositionX();

	static float GetCameraPositionY();

	static void SetCameraZoom(float zoom_factor_set);

	static float GetCameraZoom();

	void Initialize(const std::string& title);

	void CleanUp();

	void LoadImages();

	void StoreImage(const std::string& imageName);

	static TTF_Font* StoreFont(const std::string& fontName, int font_size);

	static void DrawText(const std::string& str_content, int x, int y, const std::string& font_name, int font_size, int r, int g, int b, int a);

	static void RenderTextRequest(TextRenderRequest& request);

	static void DrawUI(const std::string& image_name, float x, float y);

	static void DrawUIEx(const std::string& image_name, float x, float y, float r, float g, float b, float a, int sorting_order);

	static void Draw(const std::string& image_name, float x, float y);

	static void DrawEx(const std::string& image_name, float x, float y, float rotation_degrees,
		float scale_x, float scale_y, float pivot_x, float pivot_y, int r, int g, int b, int a, int sorting_order);

	static void DrawPixel(float x, float y, float r, float g, float b, float a);

	static void RenderUIRequest(UIRenderRequest& request);

	static void RenderImageRequest(ImageRenderRequest& request);

	static void RenderPixelRequest(DrawPixelRequest& request);

	// void RenderText(TTF_Font* font, const std::string& text, int x, int y);
};

#endif /* Renderer.h */
