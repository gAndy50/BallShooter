/* Ball Shooter
Written By Andy Patterson (c) 2013-2014 */

#include "main.h"

const int MAX_WIDTH = 800;
const int MAX_HEIGHT = 600;
const float MAX_TICKS = 1000.0f;
const int MAX_FPS = 60;
const int MAX_BALLS = 4;
const int MAX_ANI_FRAMES = 2;
const int MAX_BALL_WIDTH = 64;
const int MAX_BALL_HEIGHT = 64;

void Init();
void Run();
void Shutdown();
void Update();
void Draw();
void ClearScreen();

SDL_Color textColor = {255,255,0};

SDL_Window* Win = NULL;
SDL_Surface* Surf = NULL;
TTF_Font* Font = NULL;
SDL_Event event;

int img_flag = IMG_INIT_PNG;
int snd_flag = MIX_INIT_OGG;

Uint32 old_time, current_time;
float ftime;

int freq = 22050;
int channels = 2;
int samp = 4096;

int font_size = 20;

struct PLAYER
{
	long Score,Last_Score;
	int Point_Counter;
	int Balls_Popped;

	SDL_Surface* Name_Text;
	SDL_Rect NameText_Rect;
	SDL_Surface* Point_Text;
	SDL_Rect Point_Rect;
	SDL_Surface* BallPop_Text;
	SDL_Rect BallPop_Rect;
	string name;
	ostringstream name_stream;
	ostringstream point_stream;
	ostringstream ballpop_stream;
	SDL_Surface* Ball_Txt;
	SDL_Rect BallTxt_Rect;
	string sPoints;
};

struct BALL
{
	SDL_Surface* Balls[4];
	SDL_Rect BallRect[4];
	SDL_Surface* Pop;
	SDL_Rect PopRect;

	int X_Pos[4],Y_Pos[4];
	float X_Vel[4],Y_Vel[4];
	int Ani_Frames[4];
	int CurrentFrame[4];
	int TotalFrames[4];
	int Width[4],Height[4];

	bool Popped[4];
	float Respawn_Secs[4];
	int Points[4];
	bool Visible[4];
};

SDL_Surface* Title;
SDL_Rect TitleRect;
SDL_Surface* Points;
SDL_Rect PointRect;

SDL_Point mPoint;
SDL_Finger mFinger;

bool GameRunning = true;
bool TitleScreen = true;
bool GameScreen = false;

PLAYER gPlayer;
BALL gBall;

int main(int argc, char* argv[])
{
	Init();
	Run();

	//Update();
	//Draw();

	Shutdown();

	return 0;
}

void Init()
{
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		exit(1);
	}

	Win = SDL_CreateWindow("Ball Shooter",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,MAX_WIDTH,MAX_HEIGHT,SDL_WINDOW_SHOWN);

	Surf = SDL_GetWindowSurface(Win);

	if(IMG_Init(img_flag) < 0)
	{
		exit(1);
	}

	if(Mix_Init(snd_flag) < 0)
	{
		exit(1);
	}

	if(Mix_OpenAudio(freq,MIX_DEFAULT_FORMAT,channels,samp) < 0)
	{
		exit(1);
	}

	if(TTF_Init() < 0)
	{
		exit(1);
	}

	Font = TTF_OpenFont("arial.ttf",font_size);

	mPoint.x = 0;
	mPoint.y = 0;

	Title = IMG_Load("Title.png");
	TitleRect.x = 0;
	TitleRect.y = 0;
	TitleRect.w = MAX_WIDTH;
	TitleRect.h = MAX_HEIGHT;

	Points = TTF_RenderText_Solid(Font,"Points:",textColor);
	PointRect.x = 1;
	PointRect.y = 1;
	PointRect.w = 0;
	PointRect.h = 0;

	gPlayer.Balls_Popped = 0;
	gPlayer.Score = 0;
	gPlayer.Last_Score = gPlayer.Score;
	gPlayer.Point_Counter = 0;
	gPlayer.BallPop_Text = TTF_RenderText_Solid(Font,"Popped:",textColor);
	gPlayer.BallPop_Rect.x = 1;
	gPlayer.BallPop_Rect.y = 20;
	gPlayer.BallPop_Rect.w = 0;
	gPlayer.BallPop_Rect.h = 0;

	gBall.Balls[0] = IMG_Load("BlueBall.png");
	SDL_SetColorKey(gBall.Balls[0],SDL_TRUE,SDL_MapRGB(gBall.Balls[0]->format,255,255,255));

	gBall.Balls[1] = IMG_Load("GreenBall.png");
	SDL_SetColorKey(gBall.Balls[1],SDL_TRUE,SDL_MapRGB(gBall.Balls[1]->format,255,255,255));

	gBall.Balls[2] = IMG_Load("YellowBall.png");
	SDL_SetColorKey(gBall.Balls[2],SDL_TRUE,SDL_MapRGB(gBall.Balls[2]->format,255,255,255));

	gBall.Balls[3] = IMG_Load("RedBall.png");
	SDL_SetColorKey(gBall.Balls[3],SDL_TRUE,SDL_MapRGB(gBall.Balls[3]->format,255,255,255));

	gBall.Pop = IMG_Load("Pop.png");
	SDL_SetColorKey(gBall.Pop,SDL_TRUE,SDL_MapRGB(gBall.Pop->format,255,255,255));


	gBall.X_Pos[0] = rand() % MAX_WIDTH / 2;
	gBall.Y_Pos[0] = rand() % MAX_HEIGHT / 2;
	gBall.X_Pos[1] = rand() % MAX_WIDTH / 2;
	gBall.Y_Pos[1] = rand() % MAX_HEIGHT / 2;
	gBall.X_Pos[2] = rand() % MAX_WIDTH / 2;
	gBall.Y_Pos[2] = rand() % MAX_HEIGHT / 2;
	gBall.X_Pos[3] = rand() % MAX_WIDTH / 2;
	gBall.Y_Pos[3] = rand() % MAX_HEIGHT / 2;

	gBall.X_Vel[0] = 1.0f;
	gBall.Y_Vel[0] = 1.0f;
	gBall.X_Vel[1] = 1.5f;
	gBall.Y_Vel[1] = 1.5f;
	gBall.X_Vel[2] = 2.0f;
	gBall.Y_Vel[2] = 2.0f;
	gBall.X_Vel[3] = 2.5f;
	gBall.Y_Vel[3] = 2.5f;

	gBall.Points[0] = 5;
	gBall.Points[1] = 15;
	gBall.Points[2] = 30;
	gBall.Points[3] = 45;

	gBall.Width[0] = MAX_BALL_WIDTH;
	gBall.Width[1] = MAX_BALL_WIDTH;
	gBall.Width[2] = MAX_BALL_WIDTH;
	gBall.Width[3] = MAX_BALL_WIDTH;
	gBall.Height[0] = MAX_BALL_HEIGHT;
	gBall.Height[1] = MAX_BALL_HEIGHT;
	gBall.Height[2] = MAX_BALL_HEIGHT;
	gBall.Height[3] = MAX_BALL_HEIGHT;

	gBall.Popped[0] = false;
	gBall.Popped[1] = false;
	gBall.Popped[2] = false;
	gBall.Popped[3] = false;

	gBall.Visible[0] = true;
	gBall.Visible[1] = true;
	gBall.Visible[2] = true;
	gBall.Visible[3] = true;

	gBall.CurrentFrame[0] = 1;
	gBall.CurrentFrame[1] = 1;
	gBall.CurrentFrame[2] = 1;
	gBall.CurrentFrame[3] = 1;
	gBall.TotalFrames[0] = 2;
	gBall.TotalFrames[1] = 2;
	gBall.TotalFrames[2] = 2;
	gBall.TotalFrames[3] = 2;
	gBall.Ani_Frames[0] = gBall.CurrentFrame[0];
	gBall.Ani_Frames[1] = gBall.CurrentFrame[1];
	gBall.Ani_Frames[2] = gBall.CurrentFrame[2];
	gBall.Ani_Frames[3] = gBall.CurrentFrame[3];

	gBall.Respawn_Secs[0] = 300;
	gBall.Respawn_Secs[1] = 300;
	gBall.Respawn_Secs[2] = 300;
	gBall.Respawn_Secs[3] = 300;

	gBall.BallRect[0].x = gBall.X_Pos[0];
	gBall.BallRect[0].y = gBall.Y_Pos[0];
	gBall.BallRect[0].w = gBall.Width[0];
	gBall.BallRect[0].h = gBall.Height[0];
	gBall.BallRect[1].x = gBall.X_Pos[1];
	gBall.BallRect[1].y = gBall.Y_Pos[1];
	gBall.BallRect[1].w = gBall.Width[1];
	gBall.BallRect[1].h = gBall.Height[1];
	gBall.BallRect[2].x = gBall.X_Pos[2];
	gBall.BallRect[2].y = gBall.Y_Pos[2];
	gBall.BallRect[2].w = gBall.Width[2];
	gBall.BallRect[2].h = gBall.Height[2];
	gBall.BallRect[3].x = gBall.X_Pos[3];
	gBall.BallRect[3].y = gBall.Y_Pos[3];
	gBall.BallRect[3].w = gBall.Width[3];
	gBall.BallRect[3].h = gBall.Height[3];

	current_time = SDL_GetTicks();
}

void Run()
{
	while(GameRunning)
	{
		old_time = current_time;
		current_time = SDL_GetTicks();
		ftime = (current_time - old_time) / 1000.0f;

		gPlayer.point_stream << gPlayer.Score;
		gPlayer.Point_Text = TTF_RenderText_Solid(Font,gPlayer.point_stream.str().c_str(),textColor);
		gPlayer.Point_Rect.x = 60;
		gPlayer.Point_Rect.y = 1;
		gPlayer.Point_Rect.w = 0;
		gPlayer.Point_Rect.h = 0;

		gPlayer.ballpop_stream << gPlayer.Balls_Popped;
		gPlayer.Ball_Txt = TTF_RenderText_Solid(Font,gPlayer.ballpop_stream.str().c_str(),textColor);
		gPlayer.BallTxt_Rect.x = 75;
		gPlayer.BallTxt_Rect.y = 20;
		gPlayer.BallTxt_Rect.w = 0;
		gPlayer.BallTxt_Rect.h = 0;

		gPlayer.point_stream.str("");
		gPlayer.point_stream.str() = "";
		gPlayer.point_stream.str(string());

		gPlayer.ballpop_stream.str("");
		gPlayer.ballpop_stream.str() = "";
		gPlayer.ballpop_stream.str(string());

		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT)
			{
				GameRunning = false;
			}

			if(event.type == SDL_KEYDOWN)
			{
				if(event.key.keysym.sym == SDLK_ESCAPE)
				{
					GameRunning = false;
				}

				if(event.key.keysym.sym == SDLK_RETURN && TitleScreen == true)
				{
					TitleScreen = false;
					GameScreen = true;
				}
			}

			if(event.type == SDL_MOUSEMOTION || event.type == SDL_FINGERMOTION)
			{
				SDL_GetMouseState(&mPoint.x,&mPoint.y);

				mPoint.x = event.motion.x;
				mPoint.y = event.motion.y;
			}

			if(event.type == SDL_MOUSEBUTTONDOWN)
			{
				SDL_GetMouseState(&mPoint.x,&mPoint.y);

				mPoint.x = event.button.x;
				mPoint.y = event.button.y;

				if( (mPoint.x > gBall.BallRect[0].x) && (mPoint.x < gBall.BallRect[0].x + gBall.BallRect[0].w) && (mPoint.y > gBall.BallRect[0].y) && (mPoint.y < gBall.BallRect[0].y + gBall.BallRect[0].h) )
				{
					gPlayer.Score += gBall.Points[0];
					gPlayer.Balls_Popped++;
					gBall.Popped[0] = true;
					gBall.CurrentFrame[0]++;
					gBall.Visible[0] = false;
				}
				else if( (mPoint.x > gBall.BallRect[1].x) && (mPoint.x < gBall.BallRect[1].x + gBall.BallRect[1].w) && (mPoint.y > gBall.BallRect[1].y) && (mPoint.y < gBall.BallRect[1].y + gBall.BallRect[1].h) )
				{
					gPlayer.Score += gBall.Points[1];
					gPlayer.Balls_Popped++;
					gBall.Popped[1] = true;
					gBall.CurrentFrame[1]++;
					gBall.Visible[1] = false;
				}
				else if( (mPoint.x > gBall.BallRect[2].x) && (mPoint.x < gBall.BallRect[2].x + gBall.BallRect[2].w) && (mPoint.y > gBall.BallRect[2].y) && (mPoint.y < gBall.BallRect[2].y + gBall.BallRect[2].h) )
				{
					gPlayer.Score += gBall.Points[2];
					gPlayer.Balls_Popped++;
					gBall.Popped[2] = true;
					gBall.CurrentFrame[2]++;
					gBall.Visible[2] = false;
				}
				else if( (mPoint.x > gBall.BallRect[3].x) && (mPoint.x < gBall.BallRect[3].x + gBall.BallRect[3].w) && (mPoint.y > gBall.BallRect[3].y) && (mPoint.y < gBall.BallRect[3].y + gBall.BallRect[3].h) )
				{
					gPlayer.Score += gBall.Points[3];
					gPlayer.Balls_Popped++;
					gBall.Popped[3] = true;
					gBall.CurrentFrame[3]++;
					gBall.Visible[3] = false;
				}
			}
		}

		Update();
		Draw();

		SDL_UpdateWindowSurface(Win);
	}
}

void Update()
{
	gBall.BallRect[0].x += (int)gBall.X_Vel[0];
	gBall.BallRect[0].y += (int)gBall.Y_Vel[0];
	gBall.BallRect[1].x += (int)gBall.X_Vel[1];
	gBall.BallRect[1].y += (int)gBall.Y_Vel[1];
	gBall.BallRect[2].x += (int)gBall.X_Vel[2];
	gBall.BallRect[2].y += (int)gBall.Y_Vel[2];
	gBall.BallRect[3].x += (int)gBall.X_Vel[3];
	gBall.BallRect[3].y += (int)gBall.Y_Vel[3];

	if(GameScreen == true)
	{
		if(gBall.BallRect[0].x < 0 || gBall.BallRect[0].x + gBall.BallRect[0].w > MAX_WIDTH)
		{
			gBall.BallRect[0].x -= (int)gBall.X_Vel[0];
			gBall.X_Vel[0] *= -1;
		}

		if(gBall.BallRect[0].y < 0 || gBall.BallRect[0].y + gBall.BallRect[0].h > MAX_HEIGHT)
		{
			gBall.BallRect[0].y -= (int)gBall.Y_Vel[0];
			gBall.Y_Vel[0] *= -1;
		}

		if(gBall.BallRect[1].x < 0 || gBall.BallRect[1].x + gBall.BallRect[1].w > MAX_WIDTH)
		{
			gBall.BallRect[1].x -= (int)gBall.X_Vel[1];
			gBall.X_Vel[1] *= -1;
		}

		if(gBall.BallRect[1].y < 0 || gBall.BallRect[1].y + gBall.BallRect[1].h > MAX_HEIGHT)
		{
			gBall.BallRect[1].y -= (int)gBall.Y_Vel[1];
			gBall.Y_Vel[1] *= -1;
		}

		if(gBall.BallRect[2].x < 0 || gBall.BallRect[2].x + gBall.BallRect[2].w > MAX_WIDTH)
		{
			gBall.BallRect[2].x -= (int)gBall.X_Vel[2];
			gBall.X_Vel[2] *= -1;
		}

		if(gBall.BallRect[2].y < 0 || gBall.BallRect[2].y + gBall.BallRect[2].h > MAX_HEIGHT)
		{
			gBall.BallRect[2].y -= (int)gBall.Y_Vel[2];
			gBall.Y_Vel[2] *= -1;
		}

		if(gBall.BallRect[3].x < 0 || gBall.BallRect[3].x + gBall.BallRect[3].w > MAX_WIDTH)
		{
			gBall.BallRect[3].x -= (int)gBall.X_Vel[3];
			gBall.X_Vel[3] *= -1;
		}

		if(gBall.BallRect[3].y < 0 || gBall.BallRect[3].y + gBall.BallRect[3].h > MAX_HEIGHT)
		{
			gBall.BallRect[3].y -= (int)gBall.Y_Vel[3];
			gBall.Y_Vel[3] *= -1;
		}

		if(gBall.Popped[0] == true)
		{
			gBall.PopRect.x = gBall.BallRect[0].x;
			gBall.PopRect.y = gBall.BallRect[0].y;
			gBall.PopRect.w = gBall.BallRect[0].w;
			gBall.PopRect.h = gBall.BallRect[0].h;

			gBall.X_Vel[0] = 0;
			gBall.Y_Vel[0] = 0;

			gBall.Respawn_Secs[0]--;

			if(gBall.Respawn_Secs[0] == 0)
			{
				gBall.CurrentFrame[0] = 1;
				gBall.Popped[0] = false;
				gBall.Visible[0] = true;
				gBall.X_Vel[0] += 1.0f;
				gBall.Y_Vel[0] += 1.0f;
				gBall.Respawn_Secs[0] = 300;
			}
		}
		else if(gBall.Popped[1] == true)
		{
			gBall.PopRect.x = gBall.BallRect[1].x;
			gBall.PopRect.y = gBall.BallRect[1].y;
			gBall.PopRect.w = gBall.BallRect[1].w;
			gBall.PopRect.h = gBall.BallRect[1].h;

			gBall.X_Vel[1] = 0;
			gBall.Y_Vel[1] = 0;

			gBall.Respawn_Secs[1]--;

			if(gBall.Respawn_Secs[1] == 0)
			{
				gBall.CurrentFrame[1] = 1;
				gBall.Popped[1] = false;
				gBall.Visible[1] = true;
				gBall.X_Vel[1] += 1.5f;
				gBall.Y_Vel[1] += 1.5f;
				gBall.Respawn_Secs[1] = 300;
			}
		}
		else if(gBall.Popped[2] == true)
		{
			gBall.PopRect.x = gBall.BallRect[2].x;
			gBall.PopRect.y = gBall.BallRect[2].y;
			gBall.PopRect.w = gBall.BallRect[2].w;
			gBall.PopRect.h = gBall.BallRect[2].h;

			gBall.X_Vel[2] = 0;
			gBall.Y_Vel[2] = 0;

			gBall.Respawn_Secs[2]--;

			if(gBall.Respawn_Secs[2] == 0)
			{
				gBall.CurrentFrame[2] = 1;
				gBall.Popped[2] = false;
				gBall.Visible[2] = true;
				gBall.X_Vel[2] += 2.0f;
				gBall.Y_Vel[2] += 2.0f;
				gBall.Respawn_Secs[2] = 300;
			}
		}
		else if(gBall.Popped[3] == true)
		{
			gBall.PopRect.x = gBall.BallRect[3].x;
			gBall.PopRect.y = gBall.BallRect[3].y;
			gBall.PopRect.w = gBall.BallRect[3].w;
			gBall.PopRect.h = gBall.BallRect[3].h;

			gBall.X_Vel[3] = 0;
			gBall.Y_Vel[3] = 0;

			gBall.Respawn_Secs[3]--;

			if(gBall.Respawn_Secs[3] == 0)
			{
				gBall.CurrentFrame[3] = 1;
				gBall.Popped[3] = false;
				gBall.Visible[3] = true;
				gBall.X_Vel[3] += 2.5f;
				gBall.Y_Vel[3] += 2.5f;
				gBall.Respawn_Secs[3] = 300;
			}
		}
	}
}

void Draw()
{
	if(TitleScreen == true)
	{
		SDL_BlitSurface(Title,NULL,Surf,&TitleRect);
	}
	else if(GameScreen == true)
	{
		ClearScreen();

		SDL_BlitSurface(gBall.Balls[0],NULL,Surf,&gBall.BallRect[0]);
		SDL_BlitSurface(gBall.Balls[1],NULL,Surf,&gBall.BallRect[1]);
		SDL_BlitSurface(gBall.Balls[2],NULL,Surf,&gBall.BallRect[2]);
		SDL_BlitSurface(gBall.Balls[3],NULL,Surf,&gBall.BallRect[3]);

		SDL_BlitSurface(Points,NULL,Surf,&PointRect);
		SDL_BlitSurface(gPlayer.Point_Text,NULL,Surf,&gPlayer.Point_Rect);
		SDL_BlitSurface(gPlayer.BallPop_Text,NULL,Surf,&gPlayer.BallPop_Rect);
		SDL_BlitSurface(gPlayer.Ball_Txt,NULL,Surf,&gPlayer.BallTxt_Rect);

		if(gBall.Popped[0] == true && gBall.CurrentFrame[0] == 2)
		{
			SDL_BlitSurface(gBall.Pop,NULL,Surf,&gBall.PopRect);
		}
		else if(gBall.Popped[1] == true && gBall.CurrentFrame[1] == 2)
		{
			SDL_BlitSurface(gBall.Pop,NULL,Surf,&gBall.PopRect);
		}
		else if(gBall.Popped[2] == true && gBall.CurrentFrame[2] == 2)
		{
			SDL_BlitSurface(gBall.Pop,NULL,Surf,&gBall.PopRect);
		}
		else if(gBall.Popped[3] == true && gBall.CurrentFrame[3] == 2)
		{
			SDL_BlitSurface(gBall.Pop,NULL,Surf,&gBall.PopRect);
		}
	}
}

void Shutdown()
{
	SDL_DestroyWindow(Win);
	SDL_FreeSurface(Surf);

	SDL_FreeSurface(Title);
	SDL_FreeSurface(Points);

	for(int i = 0; i == MAX_BALLS; i++)
	{
		SDL_FreeSurface(gBall.Balls[i]);
	}

	SDL_FreeSurface(gPlayer.Point_Text);
	SDL_FreeSurface(gPlayer.BallPop_Text);

	SDL_FreeSurface(gBall.Pop);

	Mix_CloseAudio();
	Mix_Quit();

	TTF_CloseFont(Font);
	TTF_Quit();

	SDL_Quit();
}

void ClearScreen()
{
	SDL_FillRect(Surf,NULL,SDL_MapRGB(Surf->format,0,0,0));
}