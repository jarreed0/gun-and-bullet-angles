#if __has_include(<SDL2/SDL.h>)
 #include <SDL2/SDL.h>
 #include <SDL2/SDL_image.h>
 #include <SDL2/SDL_ttf.h>
#else
 #include <SDL.h>
 #include <SDL_image.h>
 #include <SDL_ttf.h>
#endif

#include <iostream>

#include <vector>
#include <cmath>

#define PI 3.14

int setFPS = 60;
int frameCount, timerFPS, lastFrame, fps;
static int lastTime = 0;

int WIDTH = 1200;
int HEIGHT = 900;
SDL_Renderer* renderer;
SDL_Window* window;
TTF_Font *font;
SDL_Color font_color;
int font_size;

struct bullet {
 int x, y;
 double angle;
} bulletTmp;

SDL_Point mouse;
SDL_Rect gun;
SDL_Rect gunSrc;
SDL_Point gunCenter;
std::vector<bullet> bullets;
SDL_Texture* gunTex;
double gunAngle;
int bulletVel = 44;

SDL_Surface *text_surface;
SDL_Texture *text_texture;
SDL_Rect wrect;
void write(std::string t, int x, int y) {
 const char *text = t.c_str();
 if (font == NULL) {
  fprintf(stderr, "error: font not found\n");
  exit(EXIT_FAILURE);
 }
 text_surface = TTF_RenderText_Solid(font, text, font_color);
 text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
 wrect.w = text_surface->w;
 wrect.h = text_surface->h;
 wrect.x = x;
 wrect.y = y;
 SDL_FreeSurface(text_surface);
 SDL_RenderCopy(renderer, text_texture, NULL, &wrect);
 SDL_DestroyTexture(text_texture);
}

int main() {
 const Uint8 *keystates;
 bool running=true;
 bool fire=0;
 SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
 if(SDL_Init(SDL_INIT_EVERYTHING) < 0) std::cout << "Failed at SDL_Init()" << std::endl;
 window = SDL_CreateWindow("Point", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
 renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
 SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
 TTF_Init();
 font_size = 16;
 font = TTF_OpenFont("font.ttf", font_size);
 if(font == NULL) std::cout << "Failed to load font" << std::endl;
 gunTex = IMG_LoadTexture(renderer, "gun.png");
 gunSrc.x = gunSrc.y = 0;
 gunSrc.w = 39;
 gunSrc.h = 11;
 gun.w = gunSrc.w * 4;
 gun.h = gunSrc.h * 4;
 gun.x= WIDTH/2;// - gun.w/4;
 gun.y= HEIGHT/2 - gun.h/2;
 gunCenter.x=0;
 gunCenter.y=gun.h/2;
 while(running) {
  lastFrame=SDL_GetTicks();
  if(lastFrame>=(lastTime+1000)) {
   lastTime=lastFrame;
   fps=frameCount;
   frameCount=0;
  }

  fire=0;
  SDL_Event e;
  keystates = SDL_GetKeyboardState(NULL);
  while(SDL_PollEvent(&e)) {
   if(e.type == SDL_QUIT) running=false;
   if(e.button.button == SDL_BUTTON_LEFT) fire=1;
  }
  if(keystates[SDL_SCANCODE_ESCAPE]) running=false;
  SDL_GetMouseState(&mouse.x, &mouse.y);

  SDL_SetRenderDrawColor(renderer, 113, 242, 255, 255);
  SDL_RenderClear(renderer);
  frameCount++;
  timerFPS = SDL_GetTicks()-lastFrame;
  if(timerFPS<(1000/setFPS)) {
   SDL_Delay((1000/setFPS)-timerFPS);
  }

  //gunAngle+=2;

  //float xDistance = mouse.x - gun.x;
  //float yDistance = mouse.y - gun.y;

  //float xDistance = mouse.x - gun.x + gunCenter.x;
  //float yDistance = mouse.y - gun.y + gunCenter.y;

  float xDistance = mouse.x - gun.x - gunCenter.x;
  float yDistance = mouse.y - gun.y - gunCenter.y;

  gunAngle = (atan2(yDistance, xDistance)) * 180 / PI;

  //SDL_RenderCopyEx(renderer, gunTex, &gunSrc, &gun, gunAngle, NULL, SDL_FLIP_NONE);
  SDL_RenderCopyEx(renderer, gunTex, &gunSrc, &gun, gunAngle, &gunCenter, SDL_FLIP_NONE);

  SDL_SetRenderDrawColor(renderer, 169, 15, 0, 255);
  //SDL_RenderDrawLine(renderer, gun.x, gun.y, mouse.x, mouse.y);
  SDL_RenderDrawLine(renderer, gun.x+gunCenter.x, gun.y+gunCenter.y, mouse.x, mouse.y);

  if(fire) {
   bulletTmp.angle = atan2(yDistance, xDistance);
   //bulletTmp.x = gun.x + gun.w * cos(bulletTmp.angle);
   //bulletTmp.y = gun.y + gun.w * sin(bulletTmp.angle);
   bulletTmp.x = gun.x + gunCenter.x + gun.w * cos(atan2(yDistance, xDistance));
   bulletTmp.y = gun.y + gunCenter.y + gun.w * sin(atan2(yDistance, xDistance));
   bullets.push_back(bulletTmp);

   bulletTmp.angle = atan2(yDistance, xDistance) - .2;
   bullets.push_back(bulletTmp);
   bulletTmp.angle = atan2(yDistance, xDistance) + .2;
   bullets.push_back(bulletTmp);
  }
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  for(int i=0; i<bullets.size(); i++) {
   SDL_Rect bd;
   bullets[i].x += bulletVel * cos(bullets[i].angle);
   bullets[i].y += bulletVel * sin(bullets[i].angle);
   bd.x=bullets[i].x;
   bd.y=bullets[i].y;
   bd.w=bd.h=8;
   SDL_RenderFillRect(renderer, &bd);
  }

  write(std::to_string(mouse.x) + ", " + std::to_string(mouse.y), mouse.x + 12, mouse.y + 16);

  SDL_RenderPresent(renderer);

 }
 TTF_CloseFont(font);
 SDL_DestroyRenderer(renderer);
 SDL_DestroyWindow(window);
 SDL_Quit();
 return 0;
}
