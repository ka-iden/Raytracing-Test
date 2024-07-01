#pragma once
#include "SDL/include/SDL.h"
#include <iostream>

class Window
{
public:
	Window(std::string title, uint32_t w, uint32_t h, uint32_t iw, uint32_t ih)
	{
		SDL_Init(SDL_INIT_EVERYTHING);
		m_Window = SDL_CreateWindow("App", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
		m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		m_Framebuffer = SDL_CreateTexture(m_Renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, iw, ih);

		isRunning = true;
	}
	~Window()
	{
		SDL_DestroyTexture(m_Framebuffer);
		SDL_DestroyRenderer(m_Renderer);
		SDL_DestroyWindow(m_Window);
		SDL_Quit();
	}

	bool Running() { return isRunning; }
	SDL_Window* window() { return m_Window; };

	bool HandleEvents()
	{
		SDL_Event event;
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT) // Yoda is genuinely smart
		{
			isRunning = false;
		}

		if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
			return true;
		}

		return false;
	}

	void Update(std::vector<unsigned char> pixels, int pitch)
	{
		SDL_UpdateTexture(m_Framebuffer, nullptr, pixels.data(), pitch);
	}
	void Render()
	{
		SDL_RenderClear(m_Renderer);
		SDL_RenderCopy(m_Renderer, m_Framebuffer, nullptr, nullptr);
		SDL_RenderPresent(m_Renderer);
	}

private:
	bool isRunning = false;

	SDL_Window* m_Window{ nullptr };
	SDL_Renderer* m_Renderer{ nullptr };
	SDL_Texture* m_Framebuffer{ nullptr };
};