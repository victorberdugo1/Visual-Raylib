// Script parser

typedef struct
{
  char *name;
  int value;
} Label;

Label ListLabels[LABELS_NUMBERS];


void init_dial() //Handle parsing and logic
{
  // if (init_done == false) //en fait ça faisait absolument rien du tout mdr
  {
    //Managing les passages spéciaux
    switch (SCRPT[menu_index].t)
    {
    case C:
    {

      ListMenuPage[choice_menu_index].visible = true;
      inMenuChoice = true;
      // choice_sel = 0;
      int choice_index = 0; //Pour l'affichage

      strncpy(buffText, SCRPT[menu_index].c, min(strlen(SCRPT[menu_index].c),60));
      first_word = strtok(buffText, " ");
      second_word = strtok(NULL, " ");
      third_word = strtok(NULL, " ");
      fourth_word = strtok(NULL, " ");
      //This method require to have a space at the end of strlen(SCRPT[index].c
      //TODO: Find a way to fix this so it's more intuitive to write

      char* word[4];
      word[0]=first_word;
      word[1]=second_word;
      word[2]=third_word;
      word[3]=fourth_word;

      

      nb_choice = 1;

      int choice_displayed = 0;
      
      if (second_word!=NULL){
        nb_choice = 2;
        if (third_word!=NULL){
          nb_choice = 3;
          if (fourth_word!=NULL){ nb_choice = 4;}
        }
      }

      //Close your eyes kids, this is not pretty
      for (int i_choice_btn = 0; i_choice_btn<nb_choice; i_choice_btn++)
      {
        for (int i_listchoix = 0; i_listchoix < sizeof(ListeChoix)/sizeof(ListeChoix[0]); i_listchoix++) //First word
        {
          if (word[i_choice_btn]==NULL){break;}
          
          if (strcmp(ListeChoix[i_listchoix].key, word[i_choice_btn]) == 0) //If we have a matching choice (between script and ListeChoix)
          { 
            for (int i_label = 0; i_label < LABELS_NUMBERS; i_label++)
            {
              if (strcmp(ListeChoix[i_listchoix].jmp, ListLabels[i_label].name) == 0){
                //I would like to add some debug info here
                ListMenuPage[choice_menu_index].items[i_choice_btn].label = ListeChoix[i_listchoix].txt;
                ListMenuPage[choice_menu_index].items[i_choice_btn].visible = true;
                ListMenuPage[choice_menu_index].items[i_choice_btn].param = ListLabels[i_label].value;
                choice_displayed++;
                break;
              }
            }
          }
        }
      }

      if (choice_displayed==0){ //If we didn't found any corresponding choices
        // index++;
        // init_dial();
        // inMenuChoice = false;
        playSomeSound();
      }
      
      break;
    }
    case J:
    {
      for (int i = 0; i < LABELS_NUMBERS; i++)
      {
        if (ListLabels[i].name == SCRPT[menu_index].c)
        {
          menu_index = ListLabels[i].value;
          init_dial();
          break;
        }
      }
      break;
    }
    // case SWPEL:
    // {
    //   // sprEl = c_atoi(SCRPT[index].c);
    //   index++;
    //   init_dial();
    //   break;
    // }
    // case SWPER:
    // {
    //   // sprEr = c_atoi(SCRPT[index].c);
    //   index++;
    //   init_dial();
    //   break;
    // }
    case SWPM:
    {

      strncpy(buffText, SCRPT[menu_index].c, min(strlen(SCRPT[menu_index].c),60));
      first_word = strtok(buffText, " ");

      char id_expression[10];

      for (int i = 0; i < CHARACTER_NUMBER; i++)
      {
        if (CharaList[i].key != NULL)
        {
          if (strcmp(first_word, CharaList[i].key) == 0)
          {
            strcpy(id_expression, SCRPT[menu_index].c + strlen(first_word) + 1);

            CharaList[i].expression_index = c_atoi(id_expression);
          }
        }
      }

      menu_index++;
      init_dial();
      break;
    }
    case H:
    {

      strncpy(buffText, SCRPT[menu_index].c, min(strlen(SCRPT[menu_index].c),60));
      first_word = strtok(buffText, " ");

      for (int i = 0; i < CHARACTER_NUMBER; i++)
      {
        if (CharaList[i].key != NULL)
        {
          if (strcmp(first_word, CharaList[i].key) == 0)
          {
            CharaList[i].visible = !CharaList[i].visible;
          }
        }
      }

      menu_index++;
      init_dial();
      break;
    }
    case N:
    {
      //Parse le texte pour chercher les noms
      //On veux éviter de faire ça à chaque fois si possible
      inMenuChoice = false;
      char* line;
      //*TranslationData[i].trans_script[0]

      // if (strcmp(current_language, default_language)==0)
      // {
      line = SCRPT[menu_index].c;
      // }
      // else
      // {
      //   line = TranslationData[language_index].trans_script[0];
      // }
      
      strncpy(buffText, line, min(strlen(line),60)); //Cause a Segmentation fault in debug build :(

      
      first_word = strtok(buffText, " ");

      for (int i = 0; i < CHARACTER_NUMBER; i++)
      {
        if (CharaList[i].key != NULL)
        {
          if (strcmp(first_word, CharaList[i].key) == 0)
          {
            chara_name = CharaList[i].name; 
            // text_to_display = line + strlen(first_word) + 1;
            strcpy(text_to_display, line + strlen(first_word) + 1);
            
            
            // init_done = true; //what?

            UI_TEXTBOX_NAME_COLOR = CharaList[i].color_name;


            break;
          }
          else
          {
            chara_name = "";

            //text_to_display = SCRPT[index].c;
            // text_to_display = line;
            strcpy(text_to_display, line);


            UI_TEXTBOX_NAME_COLOR = DARKGRAY;
          }
        }
      }
      break;
    }
    case F: //FIN
    {
      game_st = END;
      menu_index = 0;

      break;
    }
    // case LABEL:
    // {
    //   index++;
    //   init_dial();
    //   break;
    // }
    case MOV:
    {

      char mov_to[10];

      strncpy(buffText, SCRPT[menu_index].c, min(strlen(SCRPT[menu_index].c),60));
      first_word = strtok(buffText, " ");

      for (int i = 0; i < CHARACTER_NUMBER; i++)
      {
        if (CharaList[i].key != NULL)
        {
          if (strcmp(first_word, CharaList[i].key) == 0)
          {
            strcpy(mov_to, SCRPT[menu_index].c + strlen(first_word) + 1);

            if (strcmp(mov_to, "center") == 0) //TODO : handle more special cases like this (third, side, offscreen...)
            {
              CharaList[i].gotox = (screenWidth - CharaList[i].base_image.width)/2;
            }
            else
            {
              CharaList[i].gotox = c_atoi(mov_to);
            }

            break;
          }
        }
      }

      menu_index++;
      init_dial();
      break;
    }
    case CJUMP:
    {
      //CONDITIONAL JUMP
      //C = FLAGKEY SIGN VALUE KEYLABEL

      //PARSE C
      strncpy(buffText, SCRPT[menu_index].c, min(strlen(SCRPT[menu_index].c),60));
      first_word = strtok(buffText, " "); //FLAGKEY
      second_word = strtok(NULL, " ");    //SIGN
      third_word = strtok(NULL, " ");     //VALUE
      fourth_word = strtok(NULL, " ");    //KEYLABEL

      //GET THE FLAGKEY
      for (int i = 0; i < FLAGS_NUMBER; i++)
      {
        if (FlagList[i].key != NULL)
        {
          if (strcmp(first_word, FlagList[i].key) == 0)
          {
            static bool flag_condition;

            //EVALUATE CONDITION (DETERMINED BY THE SIGN)
            if (strcmp(second_word, ">") == 0) //+
            {
              flag_condition = (FlagList[i].value > c_atoi(third_word));
            }
            else if (strcmp(second_word, "<") == 0) //+
            {
              flag_condition = (FlagList[i].value < c_atoi(third_word));
            }
            else if (strcmp(second_word, ">=") == 0) //+
            {
              flag_condition = (FlagList[i].value >= c_atoi(third_word));
            }
            else if (strcmp(second_word, "<=") == 0) //+
            {
              flag_condition = (FlagList[i].value <= c_atoi(third_word));
            }
            else if (strcmp(second_word, "==") == 0) //+
            {
              flag_condition = (FlagList[i].value == c_atoi(third_word));
            }
            else if (strcmp(second_word, "!=") == 0) //+
            {
              flag_condition = (FlagList[i].value != c_atoi(third_word));
            }

            //JUMP TO THE LABEL IF TRUE
            if (flag_condition)
            {
              // index = c_atoi(fourth_word);
              //FIND THE KEYLABEL FIRST
              for (i = 0; i < LABELS_NUMBERS; i++)
              {
                if (strcmp(ListLabels[i].name, fourth_word) == 0)
                {
                  menu_index = ListLabels[i].value;
                  init_dial();
                  break;
                }
              }
            }
            else
            {
              menu_index++;
              init_dial();
            }

            break;
          }
        }
      }

      break;
    }
    case CFLAGS:
    {
      //CHANGE FLAG VALUE
      //C = FLAGKEY (optional +/-) VALUE

      strncpy(buffText, SCRPT[menu_index].c, min(strlen(SCRPT[menu_index].c),60));
      first_word = strtok(buffText, " ");
      second_word = strtok(NULL, " ");
      third_word = strtok(NULL, " ");

      //GET THE FLAGKEY
      for (int i = 0; i < FLAGS_NUMBER; i++)
      {
        if (FlagList[i].key != NULL)
        {
          if (strcmp(first_word, FlagList[i].key) == 0)
          {
            //CHECK IF SECOND ARGUMENT IS + OR -
            if (strcmp(second_word, "+") == 0) //+
            {
              FlagList[i].value += c_atoi(third_word);
            }
            else if (strcmp(second_word, "-") == 0) //-
            {
              FlagList[i].value -= c_atoi(third_word);
            }
            else if (strcmp(second_word, "*") == 0) //*
            {
              FlagList[i].value *= c_atoi(third_word);
            }
            else if (strcmp(second_word, "/") == 0) // /
            {
              FlagList[i].value /= c_atoi(third_word);
            }
            else
            {
              FlagList[i].value = atoi(second_word); //It seem that c_atoi had problem with the input. Investigation may be useful
            }

            menu_index++;
            init_dial();
            break;
          }
        }
      }

      break;
    }
    case BG:
    {
      for (int i = 0; i < MAX_BACKGROUND; i++)
      {
        if (Background.images[i] == SCRPT[menu_index].c)
        {
          Background.in_transition = true;
          Background.bg_next = i;
          break;
        }
      }
      menu_index++;
      init_dial();

      break;
      
    }
    case MUSIC:
    {
      if (SCRPT[menu_index].c=="stop")
      {
        VN_StopMusicStream(MusicList.music_list[MusicList.music_playing]);
        MusicList.isplaying = false;
      }
      else if (SCRPT[menu_index].c=="play")
      {
        VN_PlayMusicStream(MusicList.music_list[MusicList.music_playing]);
        MusicList.isplaying = true;
      }
      else if (SCRPT[menu_index].c=="pause")
      {
        VN_PauseMusicStream(MusicList.music_list[MusicList.music_playing]);
        MusicList.isplaying = false;

      }
      else if (SCRPT[menu_index].c=="resume")
      {
        VN_ResumeMusicStream(MusicList.music_list[MusicList.music_playing]);
        MusicList.isplaying = true;
      }
      else
      {
        for (int i = 0; i < MAX_MUSIC; i++)
        {
          if (MusicList.music_name[i] == SCRPT[menu_index].c){
            VN_StopMusicStream(MusicList.music_list[MusicList.music_playing]);
            MusicList.music_playing=i ;
            // PlayMusicStream(MusicList.music_list[MusicList.music_playing]);
          }
        }
      }

      menu_index++;
      init_dial();
      break;
      
    }
    case SOUND:
    {
      for (int i = 0; i < MAX_SOUND; i++)
      {
        if (SoundList.sound_name[i] == SCRPT[menu_index].c){
          SoundList.sound_playing = i; // Do we really need that variable ?
          VN_PlaySound(SoundList.sound_list[i]);
          // StopMusicStream(MusicList.music_list[MusicList.music_playing]);
          // MusicList.music_playing=i ;
          // PlayMusicStream(MusicList.music_list[MusicList.music_playing]);
        }
      }
      // PlaySound()
      menu_index++;
      init_dial();
      break;
    }
    default:
    {
      menu_index++;
      init_dial();
      break;
    }
    }
  }
}
