/*******************************************************************************************
*
*   VNES - Raylib port
*
********************************************************************************************/

#include "raylib/src/raylib.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>


#include "NSTD_custom_lib.h"

float currentTime;
float dt;
int cursor = 1;

#include "system/input_system.h"
#include "system/graphics_system.h"
#include "system/audio_system.h"
//#include "font_system.h" //(?? Or just manage that in the graphics system I guess)

#include "UI_DEFS.h"

bool debug_mod = true;


//----Enums

enum GAME_STATE
{
  SPLASH_SCREEN,
  MAIN_MENU,
  DIAL,
  END
};
enum GAME_STATE game_st = SPLASH_SCREEN;

//-----Variables utiles

unsigned int menu_index = 3; //index //We absolutlty need to change the name before porting
float timer_typing; //Used (this is global to be used in UpdateMenu(), It might change later)

char disp_text[256]; //Current text being displayed


int choice_sel;
int nb_choice;
char text_to_display[256]; //Text to be displayed

char *chara_name = "Character Name";
char buffText[256];
char *first_word; //TODO:Replace by all of them by a char* array Word[]
char *second_word;
char *third_word;
char *fourth_word;


//Visual Novel Content
#include "script_main.h"
#include "script_parser.h"
#include "system/saveload_system.h"
#include "translation.h"
#include "init_game_data.h"


void NEWGAME(){
  playSomeSound();
  ListMenuPage[main_menu_index].visible=false;
  menu_index = 0;
  game_st = DIAL;
  init_dial();
}


void UpdateMenu()
{
  if (IsKeyPressed("DOWN"))
  {
    if (choice_sel < nb_choice - 1)
    {
      choice_sel++;
    }
    else{choice_sel=0;}
  }
  if (IsKeyPressed("UP"))
  {
    if (choice_sel > 0)
    {
      choice_sel--;
    }
    else{choice_sel=nb_choice-1;}
  }

  //TODO: Mouve Mouse in input-system
  // Vector2 MousePosition;
  // MousePosition = GetMousePosition();


  static MENU_ITEM item_menu;
  for (int i = 0; i < MAX_ITEMS_MENU_PAGE; i++)
  {
    if (ListMenuPage[i].visible)
    {
      item_menu = ListMenuPage[i].items[choice_sel];

      switch (item_menu.type)
      {
      case CHECKBOX:
      {
        if (IsKeyPressed(KEY_A))
		{
          *item_menu.variable = !*item_menu.variable;
        }
        break;
      }
      case SLIDER:
      {
        if (BTN("LEFT"))
        {
          if (*item_menu.variable - 1 >= item_menu.values[0])
          {
            *item_menu.variable -= 1;
          }
        }
        else if (BTN("RIGHT"))
        {
          if (*item_menu.variable + 1 <= item_menu.values[1])
          {
            *item_menu.variable += 1;
          }
        }
        break;
      }
      case SCRIPT_RUNNER:
      {
        if (IsKeyPressed(KEY_A))
        {
          item_menu.function((int)item_menu.variable); //It's super hacky, but I love it
        }
        break;
      }
      case MENU_NAV:
      {
        if (IsKeyPressed(KEY_A))
		{
          // item_menu.function(item_menu.variable); //It's super hacky, but I love it
          ListMenuPage[item_menu.param].visible = true;
          ListMenuPage[i].visible = false;
          choice_sel = 0;
          nb_choice = GetVisibleChoiceNumber(item_menu.param);
          //playSomeSound();
        }
        break;
      }
      case INPUT:
      {
        if (BTN("LEFT"))
        {
          if (*item_menu.variable - 1 >= item_menu.values[0])
          {
            *item_menu.variable -= 1;
          }
        }
        else if (BTN("RIGHT"))
        {
          if (*item_menu.variable + 1 <= item_menu.values[1])
          {
            *item_menu.variable += 1;
          }
        }
        break;
      }
      case CHOICE_ITEM:
      {
        // playSomeSound();
        if (IsKeyPressed(KEY_A))
		{
          menu_index = item_menu.param;
          timer_typing=0;

          if (inMenuChoice)
          {
            inMenuChoice = false;
            ListMenuPage[choice_menu_index].visible = false;
            for (int i2 = 0; i2 < MAX_ITEMS_MENU_PAGE; i2++)
            {
              ListMenuPage[choice_menu_index].items[i2].visible = false;
            }
          }

          choice_sel = 0;
          // cursor = 0;
          memset(disp_text,0,sizeof(disp_text));

          init_dial();
        }
        break;
      }
      }
    }      
  }
}

void DrawBackground()
{
  if (Background.in_transition)
  {
    if (Background.opacity>0)
    {
      Background.opacity -= min(Background.opacity, (int)(255/Background.dissolve_duration)*dt); //C'est linéaire mais mieux que rien

    }
    else {
      // playSomeSound();
      Background.in_transition = false;
      Background.bg_index = Background.bg_next; //Swap !
      Background.opacity = 255;
    }
  }


  VN_DrawTexture(Background.texture[Background.bg_next],0,0,WHITE);
  VN_DrawTexture(Background.texture[Background.bg_index],0,0,(Color){255,255,255,Background.opacity});
}

void draw_dial()
{
  DrawBackground();
  

  //Draw characters on the screen
  for (int i = 0; i < CHARACTER_NUMBER; i++)
  {
    if (CharaList[i].visible)
    { //will change later
      VN_DrawTexture(CharaList[i].base_image, CharaList[i].x, CharaList[i].y, WHITE);
      VN_DrawTexture(CharaList[i].expression[CharaList[i].expression_index], CharaList[i].x, CharaList[i].y, WHITE);
    }
  }

  //DRAW Text & Textbox


  if (!inMenuChoice)
  {
    VN_DrawTexture(UI_IMAGE.textbox,(screenWidth-UI_IMAGE.textbox.width)/2,315,WHITE);

    //VN_DrawText(chara_name, 163+10, 315+5, 25, UI_TEXTBOX_NAME_COLOR);
    //VN_DrawText(disp_text, 163+10, 315+30, 20, UI_TEXTBOX_TEXT_COLOR);
    DrawTextEx(GetFontDefault(),disp_text,(Vector2){163+20,315+40},(float)20,(float)1,DARKGRAY);

    if (cursor >= strlen(text_to_display))
    {
      UI_IMAGE.ctc_color = UI_TEXTBOX_NAME_COLOR;
      VN_DrawTexture(UI_IMAGE.ctc, (screenWidth+UI_IMAGE.textbox.width)/2 - UI_IMAGE.ctc.width - 20,390 + sin(currentTime*3)*3,UI_IMAGE.ctc_color);
    }
  }

  DrawUI(choice_sel);
  if (debug_mod == true){
    //VN_DrawText(TextFormat("*Index* : %d\n*FLAGS* :", index),10,40,20,BLACK);
    int i;
    for (i = 0; i < FLAGS_NUMBER; i++)
    {
      //VN_DrawText(TextFormat("%s : %d\n", FlagList[i].key, FlagList[i].value),10,80+(i*20),20,LIGHTGRAY);
    }

    //VN_DrawText(TextFormat("~Cursor~ : %d", cursor),10,80+((i+1)*20),20,LIGHTGRAY);
    //VN_DrawText(TextFormat("Lang : %s", current_language),10,80+((i+2)*20),20,LIGHTGRAY);
    
  }
}

void updt_dial()
{
  timer_typing += GetFrameTime();

  //"Tweening"
  for (int i = 0; i < CHARACTER_NUMBER; i++)
  {
    if (CharaList[i].x != CharaList[i].gotox)
    {
      CharaList[i].x += (CharaList[i].gotox - CharaList[i].x) / 10;

      //Todo : have some real tweening going on
    }
  }

  if (SCRPT[menu_index].t == A || SCRPT[menu_index].t == N)
  {
    if (!inMenuPause)
    {
      if (IsKeyPressed(KEY_A))
      {
        if (cursor < strlen(text_to_display))
        {
          cursor = strlen(text_to_display);
        } //display the whole text
        else
        {
          if (index < sizeof(SCRPT) / sizeof(SCRPT[0]) - 1)
          {
            menu_index++;
            cursor = 0;
            timer_typing = 0;

            memset(disp_text,0,sizeof(disp_text)); //Vider le string

            init_dial();
          }
          else
          {
            game_st = END;
            cursor = 0;
          }
        }
      }

      if (cursor < strlen(text_to_display))
      {
        if (OPTION.cps==0)
        {
          cursor = strlen(text_to_display);
        }
        else if (timer_typing*OPTION.cps>=1)
        {
          //That would be fun to slowdown the timer if there is a ?!.,;: ...
          //Setting timer_typing to a negative value doesn't work tho, I've try

          //We will sometime jump the cursor in the middle of a multi-byte characters, that's a problem
          //TODO: Fix this, plz
          cursor += (int)(timer_typing*OPTION.cps);
          if (cursor >= strlen(text_to_display)) {cursor = strlen(text_to_display);}
          timer_typing=0;

          //Parse the text to check if we have md or BBCODE
          //This is a ugly duplication from the VN_DrawText function but I want to keep things somewhat organized

          char current_char = text_to_display[cursor];

          // if (cursor!=strlen(text_to_display)) //Do we really need that ?
          {
          if (current_char == '[')
          {
            // //TODO: Test if it crash when a bracket isn't closed
            int i_displacement;
            for (i_displacement = cursor; i_displacement < strlen(text_to_display); i_displacement++)
            {
              if (text_to_display[i_displacement]==']') break;
            }
            cursor = i_displacement+1;
          }
          else if ((current_char == '*'))
          {
            if (text_to_display[cursor+1]=='*') cursor += 2;
            else cursor+=1;
          }
          else if (current_char == '_' && text_to_display[cursor+1]=='_')
          {
            cursor += 2;
          }
          else if ((current_char == '~'))
          {
            if (text_to_display[cursor+1]=='~') cursor += 2;
            else cursor+=1;
          }
          }
        }
      }
      strncpy(disp_text, text_to_display, cursor);
    }
  }

  //MENU MANAGER
  if (IsKeyPressed("START"))
  {
    // playSomeSound();
    choice_sel = 0;
    inMenuPause = !inMenuPause;

    if (!inMenuPause) //If we quit the pause menu
    {
      ListMenuPage[pause_menu_index].visible = false;
      for (int i = pause_menu_index; i < MenuPageNumber; i++)
      {
        ListMenuPage[i].visible = false;
      }

      timer_typing=0;

      SAVECONFIG();

      init_dial();
      
    }
    else
    {
      ListMenuPage[pause_menu_index].visible = true;
      nb_choice = GetVisibleChoiceNumber(pause_menu_index);

      if (inMenuChoice)
      {
        // nb_choice = GetVisibleChoiceNumber(pause_menu_index);
        
        ListMenuPage[choice_menu_index].visible = false;
      }
    }

  }

  if (inMenuChoice || inMenuPause)
  {
    //Handle Input
    UpdateMenu();
  }

};

void draw_menu()
{
  float logoX;
  float logoY;

  logoX = screenWidth/2-UI_IMAGE.mainmenu_logo.width/2;
  logoY = screenHeight/2-UI_IMAGE.mainmenu_logo.height/2;
  
  VN_DrawTexture(UI_IMAGE.mainmenu_logo,logoX,logoY+sin(currentTime)*10, (Color){255,255,255,min(currentTime*40,255)});
  
  //char *IntroMenuText = "PRESS A TO START";
  //if (ListMenuPage[main_menu_index].visible==false)
  //{
  //  VN_DrawText(IntroMenuText, screenWidth/2 - VN_MeasureText(IntroMenuText,30)/2,400,30,(Color){0,0,0,(sin(currentTime*2)+1)*255/2});
  //}
	ListMenuPage[main_menu_index].visible= false;
  	DrawUI(choice_sel);
}

void updt_menu()
{
  if (ListMenuPage[main_menu_index].visible==false)
  {
    if (IsKeyPressed(KEY_A))
	{
        ListMenuPage[main_menu_index].visible=true;
    }
  // NEWGAME();
  }
  else{
    nb_choice=2;
    UpdateMenu();
  }
  
}

void draw_end()
{
  //draw_dial();
  //   vrambuf_put(NTADR_A(14,10),"FIN",3);
  //   vrambuf_put(NTADR_A(5,20),"Merci d'avoir joue! *",21);
}

void updt_end()
{
  if (IsKeyPressed(KEY_A))
  {
    game_st = MAIN_MENU;
    menu_index = 0;
  }
}

void draw_splash()
{

  VN_DrawTexture(UI_IMAGE.splash,
  screenWidth/2-UI_IMAGE.splash.width/2, screenHeight/2-UI_IMAGE.splash.height/2,
  (Color){255,255,255,min(currentTime*80,255)});
}

void updt_splash()
{
  if (currentTime>3 || IsKeyPressed(KEY_A) ) 
  {
    game_st = MAIN_MENU;
    menu_index = 0;
    currentTime = 0;
  }
}


int main(int argc, char *argv[])
{
  // Initialization
  //--------------------------------------------------------------------------------------
  fflush(stdout);
  InitGame();
  // SaveBackupTanslation();
  // ExportTranslation();

  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------
  init_dial();

  // printf("Hey !\n");

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // Update
    dt = GetFrameTime();
    currentTime += dt;
    

    switch (game_st)
    {
    case MAIN_MENU:
    {
      updt_menu();

      break;
    }
    case DIAL:
    {
      updt_dial();
      
      VN_SetMusicVolume(MusicList.music_list[0], (float)OPTION.volume/2/100);

      if (!OPTION.check){
        VN_UpdateMusicStream(MusicList.music_list[MusicList.music_playing]);
      }

      break;
    }
    case END:
    {
      updt_end();

      break;
    }
    case SPLASH_SCREEN:
    {
      updt_splash();
game_st = DIAL;
      break;
    }
    }

    BeginDrawing();

    ClearBackground(RAYWHITE);

    switch (game_st)
    {
    case MAIN_MENU:
    {
      draw_menu();
      break;
    }
    case DIAL:
    {
      draw_dial();

      break;
    }
    case END:
    {
      draw_end();

      break;
    }
    case SPLASH_SCREEN:
    {
      draw_splash();
      break;
    }
    }

    if (debug_mod == true)
    {
      VN_DrawFPS(10, 10);
    }

    if (a_pressed)
    {
      VN_DrawText("A", 200, 10, 20, DARKGRAY);
    }

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------

  //Textures
  for (int i = 0; i < CHARACTER_NUMBER; i++)
  {
    VN_UnloadTexture(CharaList[i].base_image);
    for (int i2 = 0; i2 < MAX_EXPRESSION; i2++)
    {
      VN_UnloadTexture(CharaList[i].expression[i2]);
    }
  }

  for (int i = 0; i < MAX_BACKGROUND; i++)
  {
    VN_UnloadTexture(Background.texture[i]);
  }

  //UI
  {
    VN_UnloadTexture(UI_IMAGE.ctc);
    VN_UnloadTexture(UI_IMAGE.textbox);
    VN_UnloadTexture(UI_IMAGE.splash);
  }

  //Sound/Musics
  for (int i = 0; i < MAX_MUSIC; i++)
  {
    VN_UnloadMusicStream(MusicList.music_list[i]);
  }

  for (int i = 0; i < MAX_SOUND; i++)
  {
    VN_UnloadSound(SoundList.sound_list[i]);
  }

  VN_UnloadFont(Text_font);
  VN_UnloadFont(Text_font_bold);
  VN_UnloadFont(Text_font_italic);
  VN_UnloadFont(Text_font_bolditalic);

  //Free translation backup
  // for (int i=0; i < sizeof(TranslationBackup.trans_script)/sizeof(TranslationBackup.trans_script[0]); i++) {
  //   free(TranslationBackup.trans_script[i]);
  // }

  CloseWindow(); // Close window and OpenGL context
  CloseAudioDevice(); 
  //--------------------------------------------------------------------------------------

  return 0;
}
