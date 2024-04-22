#include "Renderer.h"
#include "Engine.hpp"

TextRenderRequest::TextRenderRequest(
	std::string text, std::string font, SDL_Color color,
	int size, int x, int y) : text(text), font(font), color(color), size(size), x(x), y(y) {}

UIRenderRequest::UIRenderRequest(std::string image, SDL_Color color, float x, float y,
	int sorting_order) : image(image), color(color), x(x), y(y), sorting_order(sorting_order) {}

ImageRenderRequest::ImageRenderRequest(std::string image, SDL_Color color,
	float x, float y, int sorting_order, float rotation,
	float scale_x, float scale_y, float pivot_x, float pivot_y) : image(image),
		color(color), x(x), y(y), sorting_order(sorting_order), rotation(rotation),
		scale_x(scale_x), scale_y(scale_y), pivot_x(pivot_x), pivot_y(pivot_y) {}

DrawPixelRequest::DrawPixelRequest(SDL_Color color, float x, float y) : color(color), x(x), y(y) {}

float Renderer::GetCameraZoomFactor() {
	return zoom_factor;
}

glm::ivec2 Renderer::GetCameraDimension() {
	return glm::ivec2(x_resolution, y_resolution);
}

glm::vec2 Renderer::GetCameraPosition() {
	return cam_position;
}

SDL_Texture* Renderer::GetTexture(const std::string& name) {
	return textures[name];
}

void Renderer::SetCameraPosition(float x, float y) {
	cam_position.x = x;
	cam_position.y = y;
}

float Renderer::GetCameraPositionX() {
	return cam_position.x;
}

float Renderer::GetCameraPositionY() {
	return cam_position.y;
}

void Renderer::SetCameraZoom(float zoom_factor_set) {
	zoom_factor = zoom_factor_set;
	SDL_RenderSetScale(renderer, zoom_factor_set, zoom_factor_set);
}

float Renderer::GetCameraZoom() {
	return zoom_factor;
}

//SDL_RendererFlip Renderer::GetRendererFlip(bool horizontalFlip, bool verticalFlip) {
//	int flip = 0;
//	if (horizontalFlip) {
//		flip |= SDL_FLIP_HORIZONTAL;
//	}
//	if (verticalFlip) {
//		flip |= SDL_FLIP_VERTICAL;
//	}
//	return static_cast<SDL_RendererFlip>(flip);
//}


void Renderer::Initialize(const std::string& title) {
	window = Helper::SDL_CreateWindow498(title.c_str(), 100, 100, x_resolution, y_resolution, SDL_WINDOW_SHOWN);
	renderer = Helper::SDL_CreateRenderer498(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	CleanUp();
}

void Renderer::LoadImages() {

	/* Load image files in resources/images */
	const std::string imageDirectoryPath = "resources/images";

	// Fills up loadedScenes if the path exists
	if (std::filesystem::exists(imageDirectoryPath)) {
		for (const auto& imageFile : std::filesystem::directory_iterator(imageDirectoryPath)) {
			if (imageFile.path() != imageDirectoryPath + "/.DS_Store")
			{
				if (textures.find(imageFile.path().filename().stem().stem().string()) == textures.end()) {
					std::string path = imageFile.path().string();
					SDL_Texture* img = IMG_LoadTexture(Renderer::renderer, path.c_str());

					textures[imageFile.path().filename().stem().stem().string()] = img;
				}
			}

		}

	}
}

void Renderer::StoreImage(const std::string& imageName) {
	std::string filepath = "resources/images/" + imageName + ".png";
	SDL_Texture* texture = IMG_LoadTexture(renderer, filepath.c_str());
	intro_image_names.emplace_back(imageName);
	textures[imageName] = texture;
}

TTF_Font* Renderer::StoreFont(const std::string& fontName, int font_size) {
	std::string filepath = "resources/fonts/" + fontName + ".ttf";
	TTF_Font* font = TTF_OpenFont(filepath.c_str(), font_size);
	fonts[fontName][font_size] = font;
	return font;
}

void Renderer::DrawText(const std::string& str_content, int x, int y, const std::string& font_name, int font_size, int r, int g, int b, int a) {
	SDL_Color font_color = { static_cast<Uint8>(std::round(r)), static_cast<Uint8>(std::round(g)), static_cast<Uint8>(std::round(b)), static_cast<Uint8>(std::round(a)) };
	Engine::text_render_requests.push_back(TextRenderRequest(str_content, font_name, font_color, font_size, x, y));
}

void Renderer::RenderTextRequest(TextRenderRequest& request) {
	TTF_Font* font;
	if (fonts.find(request.font) != fonts.end()) {
		if (fonts[request.font].find(request.size) != fonts[request.font].end()) {
			font = fonts[request.font][request.size];
		}
	}
	font = StoreFont(request.font, request.size);
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, request.text.c_str(), request.color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_Rect rect;
	rect.x = request.x;
	rect.y = request.y;
	rect.w = textSurface->w;
	rect.h = textSurface->h;
	Helper::SDL_RenderCopyEx498(0, "", renderer, texture, NULL, &rect, 0, NULL, SDL_FLIP_NONE);
}

void Renderer::DrawUI(const std::string& image_name, float x, float y) {
	SDL_Color color = { 255, 255, 255, 255 };
	Engine::ui_render_requests.push_back(UIRenderRequest(image_name, color, x, y, 0));
}

void Renderer::DrawUIEx(const std::string& image_name, float x, float y, float r, float g, float b, float a, int sorting_order) {
	SDL_Color color = { static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a) };
	Engine::ui_render_requests.push_back(UIRenderRequest(image_name, color, x, y, sorting_order));
}

void Renderer::Draw(const std::string& image_name, float x, float y) {
	SDL_Color color = { 255, 255, 255, 255 };
	Engine::image_render_requests.push_back(ImageRenderRequest(image_name, color, x, y, 0, 0, 1, 1, 0.5f, 0.5f));
}

void Renderer::DrawEx(const std::string& image_name, float x, float y, float rotation_degrees,
	float scale_x, float scale_y, float pivot_x, float pivot_y, int r, int g, int b, int a, int sorting_order) {
	SDL_Color color = { static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a) };
	Engine::image_render_requests.push_back(ImageRenderRequest(image_name, color, x, y, sorting_order,
		static_cast<int>(rotation_degrees), scale_x, scale_y, pivot_x, pivot_y));
}

void Renderer::DrawPixel(float x, float y, float r, float g, float b, float a) {
    SDL_Color color = { static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a) };
	Engine::draw_pixel_requests.push_back(DrawPixelRequest(color, x, y));
}

void Renderer::RenderUIRequest(UIRenderRequest& request) {
	int textureWidth, textureHeight;
	SDL_Texture* texture = GetTexture(request.image);
	SDL_QueryTexture(texture, NULL, NULL, &textureWidth, &textureHeight);
	SDL_Rect rect = { static_cast<int>(request.x), static_cast<int>(request.y), textureWidth, textureHeight };

	SDL_SetTextureColorMod(texture, request.color.r, request.color.g, request.color.b);
	SDL_SetTextureAlphaMod(texture, request.color.a);

	Helper::SDL_RenderCopyEx498(0, "", renderer, texture, NULL, &rect, 0, NULL, SDL_FLIP_NONE);
	
	SDL_SetTextureColorMod(texture, 255, 255, 255);
	SDL_SetTextureAlphaMod(texture, 255);
}

void Renderer::RenderImageRequest(ImageRenderRequest& request) {
	const int pixels_per_meter = 100;
	glm::vec2 final_render_position = glm::vec2(request.x, request.y) - GetCameraPosition();

	SDL_Texture* texture = GetTexture(request.image);
	SDL_Rect rect;
	SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);

	int flip = SDL_FLIP_NONE;
	if (request.scale_x < 0) {
		flip |= SDL_FLIP_HORIZONTAL;
	}
	if (request.scale_y < 0) {
		flip |= SDL_FLIP_VERTICAL;
	}
	
	float x_scale = std::abs(request.scale_x);
	float y_scale = std::abs(request.scale_y);

	rect.w *= x_scale;
	rect.h *= y_scale;

	SDL_Point pivot_point = { static_cast<int>(request.pivot_x * rect.w), static_cast<int>(request.pivot_y * rect.h) };

	glm::ivec2 cam_dimensions = GetCameraDimension();

	rect.x = static_cast<int>(final_render_position.x * pixels_per_meter + cam_dimensions.x * 0.5f * (1.0f / zoom_factor) - pivot_point.x);
	rect.y = static_cast<int>(final_render_position.y * pixels_per_meter + cam_dimensions.y * 0.5f * (1.0f / zoom_factor) - pivot_point.y);

	SDL_SetTextureColorMod(texture, request.color.r, request.color.g, request.color.b);
	SDL_SetTextureAlphaMod(texture, request.color.a);

	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	Helper::SDL_RenderCopyEx498(0, "", renderer, texture, NULL, &rect, request.rotation, &pivot_point, static_cast<SDL_RendererFlip>(flip));

	SDL_RenderSetScale(renderer, zoom_factor, zoom_factor);

	SDL_SetTextureColorMod(texture, 255, 255, 255);
	SDL_SetTextureAlphaMod(texture, 255);
}

void Renderer::RenderPixelRequest(DrawPixelRequest& request) {
	SDL_SetRenderDrawColor(renderer, request.color.r, request.color.g, request.color.b, request.color.a);
	SDL_RenderDrawPoint(renderer, request.x, request.y);
}

//void Renderer::RenderText(TTF_Font* font, const std::string& text, int x, int y) {
//	SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), fontColor);
//	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
//	SDL_Rect rect;
//	rect.x = x;
//	rect.y = y;
//	rect.w = textSurface->w;
//	rect.h = textSurface->h;
//	SDL_RenderCopy(renderer, texture, NULL, &rect);
//}

//void Renderer::RenderActor(const Actor& actor, glm::vec2 playerPosition) {
//	/*
//	// Get texture of actor via actor.view_image
//	int textureWidth, textureHeight;
//	SDL_Texture* texture;
//	texture = GetTexture(actor.CurrentViewImage());
//	SDL_QueryTexture(texture, NULL, NULL, &textureWidth, &textureHeight);
//
//	// Extra view offset
//	glm::vec2 extra_view_offset = glm::vec2(0, 0);
//	if (actor.movement_bounce_enabled && actor.velocity != glm::vec2(0, 0)) {
//		extra_view_offset = glm::vec2(0, -glm::abs(glm::sin(Helper::GetFrameNumber() * 0.15f)) * 10.0f);
//	}
//
//	// Set pivot point for actor
//	SDL_Point pivotSDLPoint;
//	pivotSDLPoint.x = std::round(actor.pivot_offset.x * std::abs(actor.scale.x));
//	pivotSDLPoint.y = std::round(actor.pivot_offset.y * std::abs(actor.scale.y));
//
//	// Set destination rect for actor
//	SDL_Rect dstRect;
//	dstRect.x = static_cast<int>(std::round(((actor.position.x - cam_offset.x - cam_position.x) * 100 + x_resolution * 0.5 / zoom_factor - pivotSDLPoint.x)));
//	dstRect.y = static_cast<int>(std::round(((actor.position.y - cam_offset.y - cam_position.y) * 100 + y_resolution * 0.5 / zoom_factor - pivotSDLPoint.y) + extra_view_offset.y));
//	dstRect.w = textureWidth * std::abs(actor.scale.x);
//	dstRect.h = textureHeight * std::abs(actor.scale.y);
//
//	SDL_RenderCopyEx(
//		renderer,
//		texture,
//		NULL,
//		&dstRect,
//		actor.transform_rotation_degrees,
//		&pivotSDLPoint,
//		GetRendererFlip(actor.scale.x < 0, actor.scale.y < 0)
//	);
//	*/
//}

//void Renderer::RenderHUD(const std::string& hp_image, TTF_Font* font, int health, int score) {
//	int w;
//	SDL_Texture* texture = GetTexture(hp_image);
//	SDL_QueryTexture(texture, NULL, NULL, &w, NULL);
//	for (int i = 0; i < health; i++) {
//		RenderImage(hp_image, 5 + i * (w + 5), 25);
//	}
//	// RenderText(font, "score : " + std::to_string(score), 5, 5);
//}

void Renderer::CleanUp() {
	SDL_SetRenderDrawColor(renderer, clear_color_r, clear_color_g, clear_color_b, 255);
	SDL_RenderClear(renderer);
}
