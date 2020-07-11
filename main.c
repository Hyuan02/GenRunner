#include <genesis.h>
#include <string.h>
#include <tools.h>
#include "res/resources.h"


#define ANIM_RUN	0
#define ANIM_JUMP	1

const char msg_start[22] = "Press START to Begin!";
const char msg_reset[22] = "Press START to Reset!";
const char msg_score[7] = "SCORE:";
int score = 0;
char text_score[3];
bool added_score = FALSE; 
int scrollspeed = 2;
bool game_on = FALSE;

// game_variables
const int floor_height = 128;
fix16 gravity = FIX16(0.2);
bool jumping = FALSE;

typedef struct {
	Sprite* character;
	int x;
	fix16 y;
	fix16 velY;
	int height;
} Player;

typedef struct{
	Sprite* graph;
	int x;
	int y;
	int velX;
} Obstacle;

Player p1;
Obstacle o1;

void showText(char s[]){
	VDP_drawText(s, 20 - strlen(s)/2 ,10);
}

void clearText(){
	VDP_clearText(0,10,32);
}

void updateScore(){
	sprintf(text_score, "%d", score); 
	VDP_drawText(text_score,17,1);
}

void clearScore(){
	VDP_clearText(17,1,10);
}

void startGame(){
	if(game_on == FALSE){
		game_on = TRUE;
		p1.x = 32;
		o1.x = 320;
		score = 0;
		scrollspeed = 2;
		o1.velX = -scrollspeed;
		SPR_setPosition(p1.character, p1.x, fix16ToInt(p1.y));
		SPR_setPosition(o1.graph, o1.x, o1.y);
		updateScore();
		clearText();
	}
}

void endGame(){
	if(game_on == TRUE){
		showText(msg_reset);
		game_on = FALSE;
	}
}


void myJoyHandler( u16 joy, u16 changed, u16 state)
{
	if (joy == JOY_1)
	{
		/*Start game if START is pressed*/
		if (state & BUTTON_START)
		{
			if(game_on == FALSE){
				startGame();
			}
		}
		if (state & BUTTON_C)
		{
			if(jumping == FALSE){
				jumping = TRUE;
        		p1.velY = FIX16(-4);
        		SPR_setAnim(p1.character, ANIM_JUMP);
			}
		}
	}
}



int main(){
	p1.x = 32;
	p1.y = FIX16(112);
	p1.height = 16;
	p1.velY = FIX16(0);
	o1.x = 320;
	o1.y = 120;
	o1.velX = 0;


    JOY_init();
	JOY_setEventHandler( &myJoyHandler );
	SPR_init(0,0,0);
    VDP_setPlanSize(32,32);
	VDP_drawText(msg_score,10,1);
	VDP_setPaletteColor(0,RGB24_TO_VDPCOLOR(0x6dc2ca));
    VDP_loadTileSet(floor.tileset,1,DMA);
    VDP_loadTileSet(wall.tileset,2,DMA);
    VDP_loadTileSet(light.tileset,3,DMA);
    // VDP_setBackgroundColor(RGB24_TO_VDPCOLOR(0x6dc2ca));
    VDP_setPalette(PAL1, light.palette->data);
	VDP_setPalette(PAL2, runner.palette->data);
    VDP_fillTileMapRect(PLAN_B, TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,1),0,16,32,1);
    VDP_fillTileMapRect(PLAN_B, TILE_ATTR_FULL(PAL1,0,FALSE,TRUE,2),0,17,32,14);
    VDP_fillTileMapRectInc(PLAN_B,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,3),15,13,2,3);
	p1.character = SPR_addSprite(&runner,p1.x,fix16ToInt(p1.y),TILE_ATTR(PAL2,0,FALSE,FALSE));
	o1.graph = SPR_addSprite(&rock,o1.x,o1.y,TILE_ATTR(PAL2,0,FALSE,FALSE));
	SPR_setAnim(p1.character, ANIM_RUN);
    showText(msg_start);
	o1.velX = -scrollspeed;
	int offset = 0;
	VDP_setScrollingMode(HSCROLL_PLANE,VSCROLL_PLANE);
	
	// updateScore();
	while(1)
	{
		if(game_on == TRUE){
			if(jumping == TRUE) p1.velY = fix16Add(p1.velY, gravity);
			VDP_setHorizontalScroll(PLAN_B, offset -= scrollspeed);
			if(offset <= -256) offset = 0;
			p1.y = fix16Add(p1.y, p1.velY);
			SPR_setPosition(p1.character, p1.x, fix16ToInt(p1.y));
			o1.x += o1.velX;
			if(o1.x < -8){
				o1.x = (int)random() % (320 + 1 - 200) + 200;
			} 
			SPR_setPosition(o1.graph,o1.x,o1.y);

			if(jumping && fix16ToInt(p1.y) + p1.height >= floor_height){
				jumping = FALSE;
				p1.velY = FIX16(0);
				p1.y = intToFix16(floor_height - p1.height);
				SPR_setAnim(p1.character, ANIM_RUN);
			}

			if(p1.x < o1.x + 8 && p1.x + 8 > o1.x){
    			if(jumping == FALSE){
        			endGame();
    			}
				else{
					if(!added_score){
						score++;
						clearScore();
						updateScore();
						if(score%10==0){
							scrollspeed++;
							o1.velX = -scrollspeed;
						}
						added_score = TRUE;
					}
				}
			}
			else{
				if(added_score){
					added_score = FALSE;
				}
			}

			SPR_update();
		}
		VDP_waitVSync();
	}
    return 0;
}