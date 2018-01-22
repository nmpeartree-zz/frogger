/*********************************************************************
                     Nelson Pereira Inc. Presents

  FFFFFFF  RRRRR     OOOOO    GGGGGG    GGGGGG   EEEEEEE   RRRRR 
  F        R    R   O     O  G      G  G      G  E         R    R
  F        R    R  O       O G         G         E         R    R
  FFFFF    RRRRR   O       O G   GGGG  G   GGGG  EEEEE     RRRRR
  F        R    R  O       O G      G  G      G  E         R    R
  F        R     R  O     O   G     G   G     G  E         R     R 
  F        R     R   OOOOO     GGGGG     GGGGG   EEEEEEE   R     R

  
  Version 1.5 - 6/2004 - added joystick function, sound effects, music
**********************************************************************/

/* INCLUDES ******************************************************/
#define WIN32_LEAN_AND_MEAN  
#define INITGUID //COM object ID

#include <windows.h>   // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>
#include <objbase.h>
#include <iostream.h>  // include important C/C++ stuff
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h> 
#include <math.h>
#include <io.h>
#include <fcntl.h>


#include <ddraw.h>  // DirectX modules
#include <dinput.h>
#include <dsound.h>
#include <dmksctrl.h>
#include <dmusici.h>
#include <dmusicc.h>
#include <dmusicf.h>

#include "T3DLIB1.H" //include library files
#include "T3DLIB2.H"
#include "T3DLIB3.H"
#include "resource.h" //include resources
////////////////////////////////////////////////////////////////////


/* DEFINES ********************************************************/
// defines for windows 
#define WINDOW_CLASS_NAME "WINCLASS1"

// default screen size
#define SCREEN_WIDTH    640  // size of screen
#define SCREEN_HEIGHT   480
#define SCREEN_BPP      8    // bits per pixel

#define BITMAP_ID            0x4D42 // universal id for a bitmap
#define MAX_COLORS_PALETTE   256

#define HOME_COLOR  230
////////////////////////////////////////////////////////////////////


/* MACROS *********************************************************/
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)
//////////////////////////////////////////////////////////////////////


/* PROTOTYPES *****************************************************/
int Game_Init(void *parms = NULL, int num_parms = 0);
int Game_Shutdown(void *parms = NULL, int num_parms = 0);
int Game_Main(void *parms = NULL,  int num_parms = 0);
void Welcome_Screen();
void Collision_Detection();
void New_Level();
void New_Game();
int Time();
////////////////////////////////////////////////////////////////////


/* GLOBALS ********************************************************/
HWND      main_window_handle = NULL; // globally track main window
HINSTANCE hinstance_app = NULL; // globally track hinstance
HINSTANCE main_instance      = NULL; // globally track hinstance


LPDIRECTDRAW7 lpDD;

WNDCLASSEX winclass;
HWND hwnd=0;
float tempo = 1.0; //for DirectMusic

//Welcome screen vector rasterization
int splashBegin = 0;

POLYGON2D FrogPoly[3];

// FrogPoly bounding box
float frog_min_x[2],
      frog_min_y[2],
      frog_max_x[2],
      frog_max_y[2];

//for collision detection
int flag = 0;
int polytwo = 2;


//BACKGROUND BITMAPS
BITMAP_IMAGE background[7];   // the background pages 4x1
int curr_bg = 0;//for displaying the appropriate background screen
				// 0 means Welcome screen
				// 1 means Game screen
				// 2 means Game Over
				// 3 means Level Complete
				// 4 means Pause screen
				// 5 means Help screen            
				// 6 means Credits screen

//MAIN CHARACTER
BOB Frogger; 
int startx = 280; //starting position for Frogger
int starty = 440;
int dx, dy;

//FROGGER BITMAPS
#define Frogger_Up 0  //the numbers correspond to the Frogger#.bmp
#define Frogger_Left 3
#define Frogger_Right 2
#define Frogger_Down 1

//Frogger Animation
int Frogger_anims[4][2] = {    {0,1},

                             {0+2,1+2},

                             {0+4,1+4},

                             {0+6,1+6},

                           };

BOB MrGator[2]; //evil frog-eating Mr.Gator

//Mr.Gator Bitmaps
#define MrGators 0

//MrGator Animation
int MrGator_anims[1][2] = { {0,1}, 
							};


//enemies, objects and powerups
BOB car [3];
BOB truck[2];
BOB bus;
BOB biglog [5];
BOB powerTime;
BOB powerFly;
BOB powerLife;
int gotPower[3] = {0,0,0};//0 is for time, 1, is for fly, 2 is for life


#define START_GLOW_COLOR  245 // starting color index to glow HOME & lights on cars
#define END_GLOW_COLOR    250 // ending color index to glow HOME & lights on cars


//InfoBar Attributes
double time = 120.00; //120 seconds
double decrementer =  0.12; //countdown seconds with this variable
int score = 0;
int level = 1;
int lives = 6;

int lastScore = 0;//for welcome screen when game over

//speed for enemies in the game
double speed = 1.0; 
double MrGatorSpeed = 2.0;


int sidewalkFlag = 0; //used for collision detection of sidewalk

int swim = 0;
char buffer[80];  // general printing buffer
///////////////////////////////////////////////////////////////////

/********************************************************************
					WINPROC FUNCTION
********************************************************************/
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, 
                            LPARAM lparam) 
{
	// this is the main message handler of the system
	PAINTSTRUCT		ps;		// used in WM_PAINT
	HDC				hdc;	// handle to a device context
	char buffer[80];        // used to print strings

	// what is the message 
switch(msg)
	{
	case WM_CREATE: 
		{
			// do initialization stuff here
			// return success
			return(0);
		} break;

	case WM_PAINT: //temporary GDI function
		{
		// simply validate the window 
   	    hdc = BeginPaint(hwnd,&ps);	 
        
        // end painting
        EndPaint(hwnd,&ps);

        // return success
		return(0);
               
   		} break;

	case WM_CLOSE:  //when the user clicks the "close" button
		{
			PostQuitMessage(0); //kill the application
			return(0); //return success
		}break;


	case WM_DESTROY: 
		{ 
			PostQuitMessage(0); //kill the application
			return(0); //return success
		} break;

	default:break;

    } // end switch

	
	return (DefWindowProc(hwnd, msg, wparam, lparam)); // process any messages that we didn't take care of 

} // end WinProc///////////////////////////////////



/************************************************************
					GAME_INIT FUNCTION
************************************************************/
int Game_Init(void *parms, int num_parms)
{
	
	int index;         // looping var
	char filename[80]; // used to build up files names

	// initilize DirectSound
   DSound_Init();
   // initialize DirectDraw
   DDraw_Init(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP);
   // initialize DirectMusic
   DMusic_Init();
   // initialize DirectInput
   //DInput_Init();
   // acquire the keyboard & joystick
   //DInput_Init_Joystick(-24,24,-24,24,10);
   //DInput_Init_Keyboard();


	// set clipping rectangle to screen extents so mouse cursor doesn't mess up at edges
	RECT screen_rect = {0,0,screen_width,screen_height}; 
	lpddclipper = DDraw_Attach_Clipper(lpddsback,1,&screen_rect);

	//Welcome Screen FrogPoly (2 vectors)
	// first create sin/cos lookup table
	for (int ang = 0; ang < 360; ang++)
	{
		float theta = (float)ang*PI/(float)180;
		cos_look[ang] = cos(theta);
		sin_look[ang] = sin(theta);
	} 

	//vertices for FrogPoly
	VERTEX2DF FrogPoly_vertices[11] =
	{	-16,-32, 28,-32, 28,-24, -4,-24,
		-4,-16, 12,-16, 12,-8, -4,-8,
		-4,16, -16,16, -16,-32
	};

	// initialize FrogPoly
	for(int z = 0; z < 3; z++)
	{
		FrogPoly[z].state = 1; // turn it on
		FrogPoly[z].num_verts = 11;
		FrogPoly[z].x0 = SCREEN_WIDTH / 2; // position it
		FrogPoly[z].y0 = SCREEN_HEIGHT / 2;
		FrogPoly[z].xv = 0;
		FrogPoly[z].yv = 0;
		FrogPoly[z].color = 2; // green
		FrogPoly[z].vlist = new VERTEX2DF [FrogPoly[z].num_verts];

		for(int index2 = 0; index2 < FrogPoly[z].num_verts; index2++)
		{
			FrogPoly[z].vlist[index2] = FrogPoly_vertices[index2];
		}
	}


	// load in each page of the scrolling background
	for (index = 0; index < 7; index++)
		{
		// build up file name
		sprintf(filename,"BACKGROUND%d.BMP",index);

		// load the background
		Load_Bitmap_File(&bitmap8bit, filename);

		// create and load the reactor bitmap image
		Create_Bitmap(&background[index], 0,0, 640, 480);
		Load_Image_Bitmap(&background[index],&bitmap8bit,0,0,BITMAP_EXTRACT_MODE_ABS);
		Unload_Bitmap_File(&bitmap8bit);
		}


	// set the palette to background image palette
	Set_Palette(bitmap8bit.palette);



	// create Frogger
	if (!Create_BOB(&Frogger,0,0,39,35,32, BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM,
		DDSCAPS_SYSTEMMEMORY))
			return(0);


	// load the frames in 4 directions, 2 frames each
	for (int direction = 0; direction < 4; direction++)
    { 
		sprintf(filename,"frogger%d.BMP", direction);

		// load in new bitmap file
		Load_Bitmap_File(&bitmap8bit,filename);
		
		//load frames
		Load_Frame_BOB(&Frogger,&bitmap8bit,0+direction*2,0,0,BITMAP_EXTRACT_MODE_CELL);  
		Load_Frame_BOB(&Frogger,&bitmap8bit,1+direction*2,1,0,BITMAP_EXTRACT_MODE_CELL);  
 
		// unload the bitmap file
		Unload_Bitmap_File(&bitmap8bit);
		
		// set the animation sequences for Frogger
		Load_Animation_BOB(&Frogger,direction,2,Frogger_anims[direction]);
    }
 

	// set up starting state for frogger
	Set_Pos_BOB(&Frogger, startx, starty); //on the south sidewalk
	
	//animation parameters for Frogger
	Set_Animation_BOB(&Frogger, 0);
	Set_Anim_Speed_BOB(&Frogger, 2);
	Set_Vel_BOB(&Frogger, 0,0);


	//create Mr.Gator
	for(int i = 0; i < 2; i++)
	{
		if (!Create_BOB(&MrGator[i],0,0,116,43,32, BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM,
		DDSCAPS_SYSTEMMEMORY))
			return(0);
	
		// load and unload in new bitmap file
		Load_Bitmap_File(&bitmap8bit,"croc.BMP");
		//load frames
		Load_Frame_BOB(&MrGator[i],&bitmap8bit,0,0,0,BITMAP_EXTRACT_MODE_CELL);  
		Load_Frame_BOB(&MrGator[i],&bitmap8bit,1,1,0,BITMAP_EXTRACT_MODE_CELL);  
		Unload_Bitmap_File(&bitmap8bit);
		
		// set the animation sequence for Mr.Gator
		Load_Animation_BOB(&MrGator[i],0,2,MrGator_anims[0]);
    
		// set up starting state for MrGator
		if(i == 1)
			Set_Pos_BOB(&MrGator[i], 20, 40); //top row of logs
		else Set_Pos_BOB(&MrGator[i], 300, 155);// bottom row of logs

		//animation parameters for MrGator
		Set_Animation_BOB(&MrGator[i], 0);
		Set_Anim_Speed_BOB(&MrGator[i], 3);
		Set_Vel_BOB(&MrGator[i], 0,0);
	}

	
	//create enemies, objects and powerups
	for(i = 0; i < 3; i++)
	{
		if (!Create_BOB(&car[i],0,0,63,45,32, BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM,
		DDSCAPS_SYSTEMMEMORY))
			return(0);
		// load and unload in new bitmap file
		Load_Bitmap_File(&bitmap8bit,"car1.BMP");
		Load_Frame_BOB(&car[i],&bitmap8bit,0,0,0,BITMAP_EXTRACT_MODE_CELL);
		Unload_Bitmap_File(&bitmap8bit);
		Set_Pos_BOB(&car[i], 10*(i*25), 375);
	}

	for(i = 0; i < 2; i++)
	{
		if (!Create_BOB(&truck[i],0,0,146,60,32, BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM,
			DDSCAPS_SYSTEMMEMORY))
			return(0);
		// load and unload in new bitmap file
		Load_Bitmap_File(&bitmap8bit,"car2.BMP");
		Load_Frame_BOB(&truck[i],&bitmap8bit,0,0,0,BITMAP_EXTRACT_MODE_CELL);
		Unload_Bitmap_File(&bitmap8bit);
		Set_Pos_BOB(&truck[i], 10*(i*30), 243);
	}


	if (!Create_BOB(&bus,0,0,165,85,32, BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM,
		DDSCAPS_SYSTEMMEMORY))
			return(0);
	// load and unload in new bitmap file
	Load_Bitmap_File(&bitmap8bit,"car3.BMP");
	Load_Frame_BOB(&bus,&bitmap8bit,0,0,0,BITMAP_EXTRACT_MODE_CELL);
	Unload_Bitmap_File(&bitmap8bit);
	Set_Pos_BOB(&bus, 330, 290);
	
	for(i = 0; i < 5; i++)
	{
		if (!Create_BOB(&biglog[i],0,0,129,65,32, BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM,
			DDSCAPS_SYSTEMMEMORY))
			return(0);
		// load and unload in new bitmap file
		Load_Bitmap_File(&bitmap8bit,"log.BMP");
		Load_Frame_BOB(&biglog[i],&bitmap8bit,0,0,0,BITMAP_EXTRACT_MODE_CELL);
		Unload_Bitmap_File(&bitmap8bit);
		if(i < 2)
			Set_Pos_BOB(&biglog[i], 10*(i*40), 148);//bottom row logs
		else if((i >= 2) && (i < 4))
			Set_Pos_BOB(&biglog[i], 10*((i-1)*25), 90);//middle row logs
		else if(i == 4) 
			Set_Pos_BOB(&biglog[i], 10*((i-2)*10), 32);//top log
	}
	
	if (!Create_BOB(&powerTime,0,0,30,30,32, BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM,
		DDSCAPS_SYSTEMMEMORY))
			return(0);
	// load and unload in new bitmap file
	Load_Bitmap_File(&bitmap8bit,"powerTime.BMP");
	Load_Frame_BOB(&powerTime,&bitmap8bit,0,0,0,BITMAP_EXTRACT_MODE_CELL);
	Unload_Bitmap_File(&bitmap8bit);
	Set_Pos_BOB(&powerTime, rand()%SCREEN_WIDTH, rand()%SCREEN_HEIGHT);

	if (!Create_BOB(&powerFly,0,0,30,30,32, BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM,
		DDSCAPS_SYSTEMMEMORY))
			return(0);
	// load and unload in new bitmap file
	Load_Bitmap_File(&bitmap8bit,"powerFly.BMP");
	Load_Frame_BOB(&powerFly,&bitmap8bit,0,0,0,BITMAP_EXTRACT_MODE_CELL);
	Unload_Bitmap_File(&bitmap8bit);
	Set_Pos_BOB(&powerFly, rand()%SCREEN_WIDTH, rand()%SCREEN_HEIGHT);

	if (!Create_BOB(&powerLife,0,0,30,30,32, BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM,
		DDSCAPS_SYSTEMMEMORY))
			return(0);
	// load and unload in new bitmap file
	Load_Bitmap_File(&bitmap8bit,"powerLife.BMP");
	Load_Frame_BOB(&powerLife,&bitmap8bit,0,0,0,BITMAP_EXTRACT_MODE_CELL);
	Unload_Bitmap_File(&bitmap8bit);


	//load in all sounds & music
	//powerup
	DSound_Load_WAV("Sounds\\powerup.WAV");
	DSound_Set_Freq(0,44100);
	
	//cars & trucks
	DSound_Load_WAV("Sounds\\honk.WAV");
	DSound_Set_Freq(1,44100);

	//collision with water
	DSound_Load_WAV("Sounds\\ocean.WAV");
	DSound_Set_Freq(2,44100);

	//sound for level complete
	DSound_Load_WAV("Sounds\\complete.WAV");
	DSound_Set_Freq(3,44100);

	//sound for accessing cheat
	DSound_Load_WAV("Sounds\\cheat.WAV");
	DSound_Set_Freq(4,44100);

	//sound for gator eat
	DSound_Load_WAV("Sounds\\roar.WAV");
	DSound_Set_Freq(5,44100);

	//sound for frogger moving
	DSound_Load_WAV("Sounds\\move.WAV");
	DSound_Set_Freq(6,44100);

	DMusic_Delete_MIDI(0);
	DMusic_Load_MIDI("Sounds\\opening.mid");
	dm_perf->SetGlobalParam(GUID_PerfMasterTempo, &tempo, sizeof(float));
	DMusic_Play(0);

	return(1); //return success

} // end Game_Init ////////////////////////////////


  
/***************************************************************
					GAME_MAIN FUNCTION
***************************************************************/
int Game_Main(void *parms, int num_parms)
{
	
	static int player_motion; //detect player motion flag
	static PALETTEENTRY glow = {0,0,0,PC_NOCOLLAPSE};  // used to animate HOME & headlights
	

	// start the timing clock
	Start_Clock();

	// clear the drawing surface
	DDraw_Fill_Surface(lpddsback, 0);

	// lock the back buffer
	DDraw_Lock_Back_Surface();

	// draw the background image
	Draw_Bitmap(&background[curr_bg], back_buffer, back_lpitch, 0);

	//JOYSTICK FUNCTION
	DInput_Read_Joystick();

	
	//IF AT WELCOME BACKGROUND SCREEN
	if(curr_bg == 0)
	{
		Welcome_Screen();
	}

	
	//IF AT THE GAME BACKGROUND SCREEN
	else if(curr_bg == 1)
	{
		

		if(KEYDOWN(VK_ESCAPE) /*|| joy_state.rgbButtons[4]*/) //PAUSE
			curr_bg = 4;

		//splash screen removal
		splashBegin = 1;

		// hide the mouse
		ShowCursor(FALSE);
		
		//draw info bar
		sprintf(buffer,"SCORE: %d       LIVES: %d                                                        LEVEL: %d    TIME REMAINING: %d", score, lives, level, Time());
		Draw_Text_GDI(buffer,8,8,RGB(255,255,255),lpddsback);

		
		// animation for flashing HOME & headlights
		static int glow_count = 0;

		if(++glow_count > 5)
		{
			// rotate the colors
			Rotate_Colors(START_GLOW_COLOR, END_GLOW_COLOR);

			// reset the counter
			glow_count = 0;
		} 

		// animate the HOME land & headlights
		glow.peBlue = rand()%256;
		Set_Palette_Entry(HOME_COLOR, &glow);

		
		// reset motion flag
		int player_moving = 0;

 
		//Motion for Frogger (joystick added)
		if (KEY_DOWN(VK_RIGHT) /*|| joy_state.lX >0*/) 
		{
			Frogger.x+=2;
			dx=2;
			dy=0;
			player_moving = 1; // set motion flag
			if (Frogger.curr_animation != Frogger_Right)//check for change in direction
				Set_Animation_BOB(&Frogger,Frogger_Right);
			if(DSound_Status_Sound(6) != DSBSTATUS_PLAYING)
				DSound_Play(6,0);
		}
		else if (KEY_DOWN(VK_LEFT) /*|| joy_state.lX <0*/)  
		{
			Frogger.x-=2;
			dx=-2;
			dy=0; 
			player_moving = 1; // set motion flag
			if (Frogger.curr_animation != Frogger_Left)//check for change in direction
				Set_Animation_BOB(&Frogger,Frogger_Left);
			if(DSound_Status_Sound(6) != DSBSTATUS_PLAYING)
				DSound_Play(6,0);
		}
		else if (KEY_DOWN(VK_UP) /*|| joy_state.lY <0*/)  
		{
			Frogger.y-=2;
			dx=0;
			dy=-2; 
			player_moving = 1; // set motion flag
			if (Frogger.curr_animation != Frogger_Up)//check for change in direction
				Set_Animation_BOB(&Frogger,Frogger_Up);
			if(DSound_Status_Sound(6) != DSBSTATUS_PLAYING)
				DSound_Play(6,0);
		}
		else if (KEY_DOWN(VK_DOWN) /*|| joy_state.lY >0*/)  
		{
			Frogger.y+=2;
			dx=0;
			dy=2; 
			player_moving = 1; // set motion flag
			if (Frogger.curr_animation != Frogger_Down)//check for change in direction
				Set_Animation_BOB(&Frogger,Frogger_Down);
			if(DSound_Status_Sound(6) != DSBSTATUS_PLAYING)
				DSound_Play(6,0);
		}
 
		
		


		// Check to animate if Frogger is moving
		// and bump back if offscreen
		if (player_moving)
		  Animate_BOB(&Frogger);
		
		//if Frogger is off screen, bump back
		if(Frogger.x > SCREEN_WIDTH - (Frogger.width >> 1))
			Frogger.x-=dx;
		else if (Frogger.x < -(Frogger.width >> 1))
		    Frogger.x=-(Frogger.width >> 1);
		else if(Frogger.y > SCREEN_HEIGHT - (Frogger.height >> 1))
		    Frogger.y-=dy;

		
		//Animate & Move enemies & objects
		//Also, if they are offscreen, wrap around to other side
		for(int i = 0; i < 2; i++)
		{
			MrGator[i].x-= MrGatorSpeed;
			Animate_BOB(&MrGator[i]);
			//reset to right side
			if (MrGator[i].x < -(MrGator[i].width >> 1 ))
				MrGator[i].x = SCREEN_WIDTH - (MrGator[i].width >> 1);
		}
	   
		for(i = 0; i < 5; i++)
		{	
			if(i < 2)
			{
				biglog[i].x-= speed;
				//reset to right side
				if (biglog[i].x < -(biglog[i].width >> 1))
					biglog[i].x = SCREEN_WIDTH - (biglog[i].width >> 1);
			}
			else if((i >=2) && (i < 4))
			{
				biglog[i].x+= speed;
				//reset to left side
				if (biglog[i].x > SCREEN_WIDTH - (biglog[i].width >> 1))
					biglog[i].x = -(biglog[i].width >> 1);
			}
			else if(i == 4)
			{
				biglog[i].x-= speed;
				//reset to right side
				if (biglog[i].x < -(biglog[i].width >> 1))
					biglog[i].x = SCREEN_WIDTH - (biglog[i].width >> 1);
			}

		}
	   
		for(i = 0; i < 3; i++)
		{
			car[i].x+= speed;
			//reset to left side
			if (car[i].x > SCREEN_WIDTH - (car[i].width >> 1))
					car[i].x = -(car[i].width >> 1);
		}
		
		for(i = 0; i < 2; i++)
		{
			truck[i].x+= speed;
			//reset to left side
			if (truck[i].x > SCREEN_WIDTH - (truck[i].width >> 1))
					truck[i].x = -(truck[i].width >> 1);
		}

		bus.x-= speed;
		//reset to right side
		if (bus.x < -(bus.width >> 1))
			bus.x = SCREEN_WIDTH - (bus.width >> 1);


		// lock surface, so we can scan for collisions
		DDraw_Lock_Back_Surface();

		
		//All Collision Detection
		Collision_Detection();
		

	    // now unlock after the scan
		DDraw_Unlock_Back_Surface();
		
		

	   //DRAW ALL VISIBLE OBJECTS HERE 
	   //order is in a Layered hierarchy, bottom to top
	   for(i = 0; i < 5; i++)
	   {
			Draw_BOB(&biglog[i], lpddsback);
	   }
	  
	   
	   //if player did not get powerups, draw them
		if(gotPower[0] == 0)
			Draw_BOB(&powerTime, lpddsback);
	   else Hide_BOB(&powerTime);

	   if(gotPower[1] == 0)
			Draw_BOB(&powerFly, lpddsback);
	   else Hide_BOB(&powerFly);
	   
	   //draw extra life every other level and if player did not get it
	   if((level % 2 == 0) && (gotPower[2] == 0))
			Draw_BOB(&powerLife, lpddsback);
	   else Hide_BOB(&powerLife);

	   
	   for(i = 0; i < 2; i++)
	   {
		   Draw_BOB(&MrGator[i], lpddsback);
	   }

	   Draw_BOB(&Frogger, lpddsback);
	   
	   for(i = 0; i < 3; i++)
	   {
			Draw_BOB(&car[i], lpddsback);
	   }
	   for(i = 0; i < 2; i++)
	   {
			Draw_BOB(&truck[i], lpddsback);
	   }

	   Draw_BOB(&bus, lpddsback);

		//if music is stopped, loop
	   if(DMusic_Status_MIDI(0)==MIDI_STOPPED)
			DMusic_Play(0);
	   
	   if(lives == 0) //Game is Over if no lives are left
	   {
		   curr_bg = 2;
		   tempo = 1.0;
		   DMusic_Delete_MIDI(0);
		   DMusic_Load_MIDI("Sounds\\over.MID");
		   dm_perf->SetGlobalParam(GUID_PerfMasterTempo, &tempo, sizeof(float));
		   DMusic_Play(0);
	   }

	}
	


	//IF IS GAME OVER SCREEN
	else if(curr_bg == 2)
	{

		//show final score
		sprintf(buffer,"FINAL SCORE: %d pts.", score);
		Draw_Text_GDI(buffer,230,320,RGB(255,255,255),lpddsback);

		if(KEYDOWN(VK_SPACE))
		{
			DMusic_Delete_MIDI(0);
			DMusic_Load_MIDI("Sounds\\opening.mid");
			dm_perf->SetGlobalParam(GUID_PerfMasterTempo, &tempo, sizeof(float));
			DMusic_Play(0);
			New_Game();
		}
	}

	
	//IF IS LEVEL COMPLETE
	else if(curr_bg == 3)
	{
		if(DSound_Status_Sound(3) != DSBSTATUS_PLAYING)
             DSound_Play(3,0);
		if(KEYDOWN(VK_RETURN) /*|| joy_state.rgbButtons[0]*/) 
			New_Level();
	}
	
	
	
	//PAUSE SCREEN
	else if(curr_bg == 4)
	{

		if(KEYDOWN(VK_RETURN) /*|| joy_state.rgbButtons[0]*/)
			curr_bg = 1; //back to game screen

		if(KEYDOWN(0x48))//H key
			curr_bg = 5; //help screen

		if(KEYDOWN(0x4E))//N key
			New_Game();

		if(KEYDOWN(0x49))//I key
			curr_bg = 6; //credits screen

	}


	//HELP SCREEN
	else if(curr_bg == 5)
	{
		if(KEYDOWN(VK_RETURN) /*|| joy_state.rgbButtons[0]*/)
			curr_bg = 4; //back to pause screen

	}

	
	//CREDIT SCREEN
	else if(curr_bg == 6)
	{
		if(KEYDOWN(VK_RETURN) /*|| joy_state.rgbButtons[0]*/)
			curr_bg = 4; //back to pause screen
		
		
		
		//SPECIAL CHEATS!!!!!!!
		if(KEYDOWN(VK_F2))
		{
			lives = 1000; //1000 lives
			sprintf(buffer,"LIFE CHEAT ACTIVATED");
			Draw_Text_GDI(buffer,230,450,RGB(255,0,0),lpddsback);
			if(DSound_Status_Sound(4) != DSBSTATUS_PLAYING)
				DSound_Play(4,0);
		}

		if(KEYDOWN(VK_F4))
		{
			decrementer = 0.0; //Infinite Time
			sprintf(buffer,"INFINITE TIME CHEAT ACTIVATED");
			Draw_Text_GDI(buffer,230,450,RGB(255,0,0),lpddsback);
			if(DSound_Status_Sound(4) != DSBSTATUS_PLAYING)
				DSound_Play(4,0);
		}

		if(KEYDOWN(VK_F10))
		{
			swim = 1; //Swimming Ability
			sprintf(buffer,"SWIMMING CHEAT ACTIVATED");
			Draw_Text_GDI(buffer,230,450,RGB(255,0,0),lpddsback);
			if(DSound_Status_Sound(4) != DSBSTATUS_PLAYING)
				DSound_Play(4,0);
		}

		if(KEYDOWN(VK_F12))
		{
			speed = .5;
			MrGatorSpeed = .25; //Super Slow Moving Objects
			sprintf(buffer,"SUPER SLOW CHEAT ACTIVATED");
			Draw_Text_GDI(buffer,230,450,RGB(255,0,0),lpddsback);
			if(DSound_Status_Sound(4) != DSBSTATUS_PLAYING)
				DSound_Play(4,0);
		}

	}


	// unlock the back buffer
	DDraw_Unlock_Back_Surface();

	// flip the surfaces
	DDraw_Flip();

	
	//EXIT THE GAME
	if(KEYDOWN(0x58))//X key
	{
		Game_Shutdown();
		PostQuitMessage(0); //kill the app & exit
	}
	
	// sync to 30 fps
	Wait_Clock(30);

	return(1);

} // end Game_Main /////////////////////////



/***************************************************
				TIME FUNCTION
*****************************************************/
int Time()
{	
	time -= decrementer; //time countdown

	if(time < 0.00) //if time runs out
	{
		time = 120.00; //reset time
		Set_Pos_BOB(&Frogger, startx, starty);// restart Frogger
		lives -= 1; //lose a life
		score -= 25; //lose points
	}

	return time;
	
	
}//end Time()//////////////////////////////////////////



/****************************************************
				NEW_LEVEL FUNCTION
****************************************************/
void New_Level()
{
	//Begin a New Level

	score += 100; //level complete bonus
	score += (time * 10); //timer bonus
	time = 120.00; //reset timer
	level += 1; //add one to level
	tempo = tempo + 0.025;
    dm_perf->SetGlobalParam(GUID_PerfMasterTempo, &tempo, sizeof(float));//increase tempo for music
	
	Set_Pos_BOB(&Frogger, startx, starty); //place back Frogger

	//show all powerups and reset powerup array
	Show_BOB(&powerTime);
	Set_Pos_BOB(&powerTime, rand()%SCREEN_WIDTH, rand()%SCREEN_HEIGHT);
	gotPower[0] = 0;

	Show_BOB(&powerFly);
	Set_Pos_BOB(&powerFly, rand()%SCREEN_WIDTH, rand()%SCREEN_HEIGHT);
	gotPower[1] = 0;
	
	Show_BOB(&powerLife);
	Set_Pos_BOB(&powerLife, rand()%SCREEN_WIDTH, rand()%SCREEN_HEIGHT);
	gotPower[2] = 0;

	sidewalkFlag = 0; //reset sidewalkFlag

	//increase game speed
	speed += .25;
	MrGatorSpeed += .25;

	curr_bg = 1; //show game background

}//end New_Level()//////////////////////////////////



/***************************************************
				NEW_GAME FUNCTION
****************************************************/
void New_Game()
{
	//for either a game over or a new game
	
	if(lastScore < score)
		lastScore = score; //save last highest score
	time = 120.00; //reset timer
	level = 1; //reset level
	score = 0; //reset score
	lives = 6; //reset lives
	tempo = 1.0; //reset music tempo
	
	Set_Pos_BOB(&Frogger, startx, starty); //place back Frogger


	//show all powerups and reset powerup array
	Show_BOB(&powerTime);
	Set_Pos_BOB(&powerTime, rand()%SCREEN_WIDTH, rand()%SCREEN_HEIGHT);
	gotPower[0] = 0;

	Show_BOB(&powerFly);
	Set_Pos_BOB(&powerFly, rand()%SCREEN_WIDTH, rand()%SCREEN_HEIGHT);
	gotPower[1] = 0;
	
	Show_BOB(&powerLife);
	Set_Pos_BOB(&powerLife, rand()%SCREEN_WIDTH, rand()%SCREEN_HEIGHT);
	gotPower[2] = 0;
	
	sidewalkFlag = 0; //reset sidewalkFlag

	
	//reset speed
	speed = 1.0;
	MrGatorSpeed = 2.0;

	curr_bg = 0; //show welcome screen
	
}///end New_Game()//////////////////////////////////



 
/****************************************************
				WELCOME_SCREEN FUNCTION
****************************************************/
void Welcome_Screen()
{
	//hide cursor
	ShowCursor(FALSE);

	/***play music for intro***/

	if(splashBegin == 0)//first time at splash screen
	{
		//****add logo splash screen***/

		//draw FrogPoly
		Draw_Filled_Polygon2D(&FrogPoly[0], (UCHAR *)ddsd.lpSurface, ddsd.lPitch);
		Draw_Filled_Polygon2D(&FrogPoly[1], (UCHAR *)ddsd.lpSurface, ddsd.lPitch);
		Draw_Polygon2D(&FrogPoly[2], (UCHAR *)ddsd.lpSurface, ddsd.lPitch);
		
		//draw border lines
		Draw_Line(0,290,640,290,2,(UCHAR *)ddsd.lpSurface,ddsd.lPitch);
		Draw_Line(0,190,640,190,2,(UCHAR *)ddsd.lpSurface,ddsd.lPitch);

		//translate & rotate
		Translate_Polygon2D_Mat(&FrogPoly[0], 3, 0);
		Translate_Polygon2D_Mat(&FrogPoly[1], -3, 0);
		Rotate_Polygon2D_Mat(&FrogPoly[2], polytwo);

		//Collsion Test
		Find_Bounding_Box_Poly2D(&FrogPoly[0], frog_min_x[0], frog_max_x[0], frog_min_y[0], frog_max_y[0]);
		Find_Bounding_Box_Poly2D(&FrogPoly[1], frog_min_x[1], frog_max_x[1], frog_min_y[1], frog_max_y[1]);

		if (Collision_Test(FrogPoly[0].x0+frog_min_x[0], // x1
                  FrogPoly[0].y0+frog_min_y[0],       // y1
                  frog_max_x[0] - frog_min_x[0],  // w1
                  frog_max_y[0] - frog_min_y[0],  // h1
                  FrogPoly[1].x0+frog_min_x[1],       // x2
                  FrogPoly[1].y0+frog_min_y[1],       // y2
                  frog_max_x[1] - frog_min_x[1],  // w2
                  frog_max_y[1] - frog_min_y[1])) // h2 
		{
			if(flag == 0)
			{
				polytwo = 2;
				flag == 1;
			}
			else if(flag == 1)
			{
				polytwo = -2;
				flag == 0;
			}
		}
	}
	else
	{
		//show last score
		sprintf(buffer,"HIGHEST SCORE:   %d pts.", lastScore);
		Draw_Text_GDI(buffer, SCREEN_WIDTH / 2 - 100 ,SCREEN_HEIGHT / 2,RGB(255,255,255),lpddsback);
	}

	if(KEYDOWN(VK_RETURN) /*|| joy_state.rgbButtons[0]*/) //move from welcome screen to game screen
	{
		curr_bg = 1;
		//play game music
		DMusic_Delete_MIDI(0);
		DMusic_Load_MIDI("Sounds\\game.mid");
		dm_perf->SetGlobalParam(GUID_PerfMasterTempo, &tempo, sizeof(float));
		DMusic_Play(0);
	}

}/////end Welcome_Screen////////////////////////////////

 
/****************************************************
				COLLISION_DETECTION FUNCTION
****************************************************/
void Collision_Detection()
{
	
	//car collision
	for(int cd = 0; cd < 3; cd++)
	{
		if(Collision_BOBS(&Frogger, &car[cd]))
		{
			lives -= 1; //lose a life
			time = 120.00; //reset time
			score -=25; //lose 25 points
			Set_Pos_BOB(&Frogger, startx, starty); //start back on the south sidewalk
			if(DSound_Status_Sound(1) != DSBSTATUS_PLAYING)
				DSound_Play(1,0);
		}
	}
	
	//truck collision
	for(cd = 0; cd < 2; cd++)
	{
		if(Collision_BOBS(&Frogger, &truck[cd]))
		{
			lives -= 1; //lose a life
			time = 120.00; //reset time
			score -=25; //lose 25 points
			Set_Pos_BOB(&Frogger, startx, starty); //start back on the south sidewalk
			if(DSound_Status_Sound(1) != DSBSTATUS_PLAYING)
				DSound_Play(1,0);
		}
	}

	//MrGator collision
	for(cd = 0; cd < 2; cd++)
	{
		if(Collision_BOBS(&Frogger, &MrGator[cd]))
		{
			lives -= 1; //lose a life
			time = 120.00; //reset time
			score -=25; //lose 25 points
			Set_Pos_BOB(&Frogger, startx, starty); //start back on the south sidewalk
			if(DSound_Status_Sound(5) != DSBSTATUS_PLAYING)
				DSound_Play(5,0);
		}
	}
	
	//Stony Brook Bus Collision
	if(Collision_BOBS(&Frogger, &bus))
	{	
		lives -= 1; //lose a life
		time = 120.00; //reset time
		score -=25; //lose 25 points
		Set_Pos_BOB(&Frogger, startx, starty); //start back on the south sidewalk
		if(DSound_Status_Sound(1) != DSBSTATUS_PLAYING)
				DSound_Play(1,0);
	}

	
	int waterFlag = 0; //for water collision, 0 means not on log
	//Log Collision (move with them)
	for(cd = 0; cd < 5; cd++)
	{
		if(cd < 2)
		{
			if(Collision_BOBS(&Frogger, &biglog[cd]))
			{
				//keep moving frogger in the direction
				Frogger.x -=speed;
				dx= -speed;
				dy= 0;
				waterFlag = 1; //Frogger on log
			}
		}
		else if((cd >=2) && (cd < 4))
		{
			if(Collision_BOBS(&Frogger, &biglog[cd]))
			{
				//keep moving frogger in the direction
				Frogger.x +=speed;
				dx= +speed;
				dy= 0;
				waterFlag = 1; //Frogger on log
			}
		}
		else if(cd == 4)
		{
			if(Collision_BOBS(&Frogger, &biglog[cd]))
			{
				//keep moving frogger in the direction
				Frogger.x -=speed;
				dx= -speed;
				dy= 0;
				waterFlag = 1; //Frogger on log
			}
		}
	}


	//PowerUp Collision
	if((Collision_BOBS(&Frogger, &powerTime)) && (gotPower[0] == 0))
	{	
		time +=30; //plus 30 seconds
		gotPower[0] = 1; //got powerup
		if(DSound_Status_Sound(0) != DSBSTATUS_PLAYING)
				DSound_Play(0,0);
	}

	if((Collision_BOBS(&Frogger, &powerFly)) && (gotPower[1] == 0))
	{	
		score +=25; //add 25 points
		gotPower[1] = 1; //got powerup
		if(DSound_Status_Sound(0) != DSBSTATUS_PLAYING)
				DSound_Play(0,0);
	}
	
	if((Collision_BOBS(&Frogger, &powerLife)) && (gotPower[2] == 0))
	{	
		lives +=1; //add 1 life
		gotPower[2] = 1; //got powerup
		if(DSound_Status_Sound(0) != DSBSTATUS_PLAYING)
				DSound_Play(0,0);
	}

	
	//if Frogger made it to north sidewalk add 50 bonus points
	if(Frogger.y <= 240 && sidewalkFlag == 0)
	{
		sidewalkFlag = 1;
		score += 50;
	}


	//Water Collision
	//if frogger is off a log, Frogger has hit the water
	//swim is a cheat feature
	if(swim == 0)
	{
		if(Frogger.y <= 180 && waterFlag == 0)
		{
			lives -= 1; //lose a life
			time = 120.00; //reset time
			score -=25; //lose 25 points
			Set_Pos_BOB(&Frogger, startx, starty); //start back on the south sidewalk
			if(DSound_Status_Sound(2) != DSBSTATUS_PLAYING)
				DSound_Play(2,0);
		}
	}
	

	//HOME Collision to start a new level
	if(Frogger.y <= 15 && Frogger.x <= 300 && Frogger.x >= 235) //coordinates of HOME
		curr_bg = 3; //Level is Complete


}////end Collision_Detection////////////////////////////////////


/*************************************************************
					GAME_SHUTDOWN FUNCTION
*************************************************************/
int Game_Shutdown(void *parms, int num_parms)
{
	
	// kill the background
	for (int index=0; index < 7; index++){
		Destroy_Bitmap(&background[index]);
	}

	// kill BOBs
	Destroy_BOB(&Frogger);
	
	for(index = 0; index < 3; index++){
		Destroy_BOB(&car[index]);
	}

	for(index = 0; index < 2; index++){
		Destroy_BOB(&truck[index]);
	}

	Destroy_BOB(&bus);
	
	for(index = 0; index < 5; index++){
		Destroy_BOB(&biglog[index]);
	}

	Destroy_BOB(&powerTime);
	Destroy_BOB(&powerFly);
	Destroy_BOB(&powerLife);

	//Shutdown DirectDraw
	DDraw_Shutdown();
	// shut down directinput
   DInput_Shutdown();
   // shutdown directdraw
   DDraw_Shutdown();
   // now directsound
   DSound_Stop_All_Sounds();
   // shutdown directsound
   DSound_Shutdown();
   // delete all the music
   DMusic_Delete_All_MIDI();
   // shutdown directmusic
   DMusic_Shutdown();
	return(0);

} // end Game_Shutdown /////////////////////////////



/***************************************************************
					WINMAIN FUNCTION
****************************************************************/
int WINAPI WinMain(	HINSTANCE hinstance, HINSTANCE hprevinstance,
					LPSTR lpcmdline, int ncmdshow) 
{

WNDCLASSEX winclass; // this will hold the class we create
HWND	   hwnd;	 // generic window handle
MSG		   msg;		 // generic message
HDC        hdc;      // graphics device context

// first fill in the window class stucture
winclass.cbSize         = sizeof(WNDCLASSEX);
winclass.style			= CS_DBLCLKS | CS_OWNDC | 
                          CS_HREDRAW | CS_VREDRAW;
winclass.lpfnWndProc	= WindowProc;
winclass.cbClsExtra		= 0;
winclass.cbWndExtra		= 0;
winclass.hInstance		= hinstance;
winclass.hIcon			= LoadIcon(NULL, MAKEINTRESOURCE(FROG_ICON));
winclass.hCursor		= LoadCursor(NULL, MAKEINTRESOURCE(FROG_CURSOR)); 
winclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
winclass.lpszMenuName	= "FROGGER_MENU";
winclass.lpszClassName	= WINDOW_CLASS_NAME;
winclass.hIconSm        = LoadIcon(NULL, MAKEINTRESOURCE(FROG_ICON));

// save hinstance in global
hinstance_app = hinstance;

// register the window class
if (!RegisterClassEx(&winclass))
	return(0);

// create the window
if (!(hwnd = CreateWindowEx(NULL,                  // extended style
                            WINDOW_CLASS_NAME,     // class
						    "Frogger by Nelson Pereira", // title
						    WS_POPUP | WS_VISIBLE,
					 	    0,0,	  // initial x,y
							SCREEN_WIDTH,SCREEN_HEIGHT,  // initial width, height
						    NULL,	  // handle to parent 
						    NULL,//LoadMenu(hinstance, MAKEINTRESOURCE(FROGGER_MENU)), // handle to menu
						    hinstance,// instance of this application
						    NULL)))	// extra creation parms
return(0);

	// save main window handle
	main_window_handle = hwnd;

	// initialize game here
	Game_Init();

	// enter main event loop
	while(TRUE)
		{
			// test if there is a message in queue, if so get it
			if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
			{ 
				// test if this is a quit
				if (msg.message == WM_QUIT)
					break;
	
				// translate any accelerator keys
				TranslateMessage(&msg);

				// send the message to the window proc
				DispatchMessage(&msg);
			} // end if
			
			// Perform game operations
			Game_Main();

		} // end while

	// closedown game here
	Game_Shutdown();

	// return to Windows like this
	return(msg.wParam);

} // end WinMain//////



////////////////////END OF FILE////////////////////////////////////
/////////////COPYRIGHT NELSON PEREIRA INC./////////////////////////