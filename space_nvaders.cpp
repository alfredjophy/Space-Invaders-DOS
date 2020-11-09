#include<iostream.h>
#include<conio.h>
#include<stdio.h>
#include<dos.h>
#include<stdlib.h>
#include<math.h>
#include<ctype.h>
#include<time.h>
#include<limits.h>
#include<string.h>
#include<fstream.h>

#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77
#define SPACE 32
#define ENTER 13
#define ESC 27

#define X_max 80
#define Y_max 25

#define TRUE 1
#define FALSE 0

#define W WHITE
#define Y YELLOW
#define O BROWN
#define R LIGHTRED
#define RR RED
#define B LIGHTBLUE
#define LB LIGHTCYAN
#define G GREEN
#define LG LIGHTGREEN
#define S LIGHTGRAY
#define DS DARKGRAY
#define P MAGENTA
#define PK LIGHTMAGENTA

/******************************************************
some mathematical functions
******************************************************/
//returns true if x belongs to [x1,x2] else false
inline char _rangeof(int x1,int x,int x2)
{
	if(x1<=x&&x<=x2)
		return TRUE;
	return FALSE;

}
//returns the sign of the argument passed
inline char sign(char t)
{
	if(t<=0)
		return -1;
	return 1;
}
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

void box(int x1,int y1,int x2,int y2,char color=W,char bkg=0);
void win(char clr=0);
void regenerate();

/*****************************************************
******************************************************/
//class which manages player scores and the high score file
class HIGHSCORES
{
	class PLAYER_SCORE
	{
		char Name[10];
		int Score;

		public:
			PLAYER_SCORE()
			{
				strcpy(Name,"xoxoxoxox");
				Score=0;
			}
			char* getName()
			{
				return Name;
			}
			int getScore()
			{
				return Score;
			}
			void putdata(char name[],int h_score)
			{
				Score=h_score;
				strupr(name);
				strcpy(Name,name);
			}

	};
	PLAYER_SCORE P[15];		//stores the top 15 highscore
    public:

	HIGHSCORES();			//opens the file and reads data into the array and removes the file
	void high_scores();		//displays the high score
	void check_score(int score);	//checks if a score is highscore and adds the player into the array
	~HIGHSCORES();			//writes the updated arry back to file

};

HIGHSCORES::HIGHSCORES()
{
	fstream f("SPI_HS.DAT",ios::binary|ios::in|ios::nocreate);
	char i=0;
	if(!f)
	{
		f.close();
		f.open("SPI_HS.DAT",ios::binary|ios::out);
		PLAYER_SCORE h;
		for(i=0;i<15;++i)
			f.write((char*)&h,sizeof(h));
		f.close();
		return;
	}
	f.seekg(0);
	while(f.read((char*)&P[i],sizeof(PLAYER_SCORE)))
			++i;

	f.close();
	remove("SPI_HS.DAT");
}

void HIGHSCORES::high_scores()
{
	win(1);
	box(1,1,79,25,LB);
	box(20,3,60,23,R);
	window(21,4,59,22);
	gotoxy(2,2);
	textcolor(Y);
	cprintf("  RANK         NAME          SCORE");
	char i=0;		
	char clr[]={LG,B,R,PK,W,LB};

	for(;i<15;i++)
	{
		gotoxy(2,4+i);
		textcolor(W);
		cprintf("   -         ----------       ---- ");
	}

	i=0;
	while(i<15)
	{
		if(P[i].getScore()==0)
			break;
		textcolor(clr[i%6]);
		gotoxy(2,4+i);
		cprintf("                                   ");
		gotoxy(5,4+i);
		cprintf("%d",i+1);
		gotoxy(15,4+i);
		cprintf("%s",P[i].getName());
		gotoxy(31,4+i);
		cprintf(" %d",P[i].getScore());
		i++;
	}
	win();
	getch();
}

void HIGHSCORES::check_score(int score)
{
	win(1);
	PLAYER_SCORE X;
	char i=0;

	if(P[14].getScore()>=score)
		return;

	char name[10];
	win(1);
	box(24,9,55,11,LG);
	window(25,10,54,10);
	clrscr();
	gotoxy(2,1);
	textcolor(W);
	cprintf("YOUR NAME :");
	_setcursortype(_SOLIDCURSOR);
	gotoxy(38,1);
	cin>>name;
	_setcursortype(_NOCURSOR);
	win();

	X.putdata(name,score);
	win(1);
	textcolor(11);


	for(i=13;i>=0&&P[i].getScore()<X.getScore();--i)
		P[i+1]=P[i];
	P[i+1]=X;

	high_scores();

}
HIGHSCORES::~HIGHSCORES()
{
	fstream f("SPI_HS.DAT",ios::binary|ios::out);
	f.seekp(0,ios::beg);
	for(char i=0;i<15;++i)
		f.write((char*)&P[i],sizeof(PLAYER_SCORE));

	f.close();
}

HIGHSCORES H;



/****************************************************
*****************************************************/
/*
  Class whose objects are enemy ships and the player ship
  and the functions that determine the behaviour of the ships

*/
class SHIPS
{
	//each node is a ship in the list of ships
	class NODE		
	{
		char Player;			//whether the ship is the player or the enemy

		float x,y,x_prev,y_prev;	//current and previous coordinates of the ship
		float velocity_x,velocity_y;    //velocities along x and y axes

		float health;			//health points of the ship

		char rows,columns;		//rows and columns of the ship
		char SHIP[5][10];   		//structure of the ship
		char color[5][10];              //color scheme of the ship

	   public:

		NODE* f_link,*b_link;		//links to implement a doubly linked list
		
		char gettype()
		{
			return Player;
		}
		void draw_ship();		//draws the ship at the current coordinates
		void clear_ship();		//clears the previously drawn ship
		float _Health()			//to get the health of the ship
		{
			return health;
		}
		void _UpdateHealth(float dh)    //to change the health,i.e,damage or health points
		{
			health+=dh;
			if(Player&&health>20)
				health=20;
		}
		void _ChangeVelocityx(float vx)		 //to change the velocity of the ship
		{
			velocity_x=vx;
		}
		void RevVelocityx()			//to reverse the direcion of the ship
		{
			velocity_x*=-1;
		}
		float getx()
		{
			return x;
		}
		float gety()
		{
			return y;
		}
		void modCord(char xn,char yn)		//sets coordinates of the ship	
		{
			x=xn;
			y=yn;
		}
		void CCord()				//assigns the current coordinates to prev coordinates
		{
			x_prev=floor(x);
			y_prev=floor(y);
		}
		void motionx()				//moves the ship along x_axis
		{
			x_prev=floor(x);
			x+=velocity_x;
			char r=(rows==3)?2:1;

			if(!_rangeof(4+r,x,57-r))
				RevVelocityx(),motionx();


		}
		char motion()			 	//determines the behaviour of the ships
		{
			motionx();

			if(Player)
				return FALSE;

			y_prev=floor(y);

			if(!(rand()%19))
				RevVelocityx();

			if(!(rand()%37))
				y+=velocity_y;

			if(y<=Y_max-4)
				return TRUE;

			return FALSE;

		}
		NODE(char);
		NODE();
		~NODE();
	};

	int ships_on_screen;	//the total number of ships on the screen
	int score;
	char c_d,c_z,c_t;       //variables used for random shooting and ship generation,etc
	char p_life;		//number of life the player has
	NODE *temp,*start;

   public:

	NODE*Player;
	int getScore()		
	{
		return score;
	}
	int Ships_onScreen()
	{
		return ships_on_screen;
	}
	void reset_counter()
	{
		ships_on_screen=0;
		score=0;
		p_life=3;
		start=new NODE('p');
		Player=temp=start;

	}
	void difficulty_weights(int z,int d,int t)	//assigns values to all randomizing variables
	{
		c_z=z;
		c_d=d;
		c_t=t;
	}
	SHIPS()
	{
		start=temp=Player=NULL;
	}
	~SHIPS()
	{
		destroy_all();
	}

	void draw_ships();				//draws all the ships in the list
	void clear_ships();				//clears all the ships inthe list
	char move_ships();				//moves all the ships in the list
	void new_ship(char x,char y,char direction);	//adds new ship in the list
	NODE* delete_ship(NODE * ptr);			//deletes the node passed from the list	
	char check_mutual_collision(NODE*i);		//checks collision among the ships and takes action
	void destroy_all();				//deletes all nodes in the list	
	void behaviour();				//determines when a ships shoots or changes direction


};


void SHIPS::NODE::draw_ship()		
{
	for(char i=0;i<rows;++i)
		for(char j=0;j<columns;++j)
		{
			gotoxy( floor(x)+j-(columns/2) , floor(y)+i-(rows/2) );
			textcolor( *(*(color+i)+j) );
			cprintf("%c",*(*(SHIP+i)+j) );

		}

}
void SHIPS::NODE::clear_ship()
{                              
	for(char i=0;i<rows;++i)
		for(char j=0;j<columns;++j)
		{
			if(!_rangeof(3,floor(x_prev)+j-(columns/2),57))
				continue;
			gotoxy( floor(x_prev)+j-(columns/2) , floor(y_prev)+i-(rows/2));
			cprintf(" ");

		}
}

SHIPS::NODE::NODE(char c)			//Player
{
	x=32,y=y_prev=Y_max-2;
	f_link=b_link=NULL;

	Player=TRUE;

	rows=3;
	columns=5;
	health=20.0;
	velocity_x=1;
	velocity_y=0;
	char a[3][5]={       "  ³  ",
			     " ÉÎ» ",
			     "ÅÉÎ»Å"   };

	char clr[3][5]={    0,0,W,0,0,
			    0,R,R,R,0,
			    B,G,R,G,B  };

	for(char i=0;i<rows;++i)
		for(char j=0;j<columns;++j)
			{
				*(*(SHIP+i)+j)=*(*(a+i)+j);
				*(*(color+i)+j)=*(*(clr+i)+j);
			}


}

SHIPS::NODE::NODE()                          //Enemy ships
{
	x=y=x_prev=y_prev=0;
	rows=columns=0;
	f_link=b_link=NULL;

	Player=FALSE;
	rows=2;
	columns=3;
	health=3.0;
	velocity_x=0.8;
	velocity_y=0.2;

	char a[2][3]={    "Û°Û",
			  " Ø "  };

	char clr[2][3]={ LB,Y+BLINK,LB,
			   0,R,0        };

	for(char i=0;i<rows;++i)
		for(char j=0;j<columns;++j)
		{
			*(*(SHIP+i)+j)=*(*(a+i)+j);
			*(*(color+i)+j)=*(*(clr+i)+j);
		}

}

SHIPS::NODE::~NODE()
{
	f_link=b_link=NULL;
	x=y=x_prev=y_prev=0;

	for(char i=0;i<rows;++i)
		for(char j=0;j<columns;++j)
			*(*(SHIP+i)+j)=*(*(color+i)+j)=0;
	rows=columns=0;


}


void SHIPS::draw_ships()
{
	for(NODE* i=start;i;i=i->f_link)
		i->draw_ship();
}
void SHIPS::clear_ships()
{
	for(NODE* i=start;i;i=i->f_link)
		i->clear_ship();
}
inline void SHIPS::new_ship(char x,char y,char direction)
{
	NODE *ptr=new NODE;
	if(!ptr)
		return;
	if(!start)
	{
		start=ptr;
		start->b_link=NULL;
	}
	else
	{
		temp->f_link=ptr;
		ptr->b_link=temp;
	}
	ptr->modCord(x,y);

	if(direction<0)
		ptr->RevVelocityx();

	temp=ptr;

	++ships_on_screen;

}
SHIPS::NODE* SHIPS::delete_ship(NODE * ptr)
{
	ptr->clear_ship();
	ptr->CCord();
	ptr->clear_ship();
	NODE * t=NULL;

	if(ptr==start)
	{
		start=start->f_link;
		(ptr->f_link)->b_link=NULL;
		t=start;
	}
	else
	{
		score+=5;
		(ptr->b_link)->f_link=ptr->f_link;
		if(ptr->f_link)
			(ptr->f_link)->b_link=ptr->b_link;
		else
			temp=ptr->b_link,temp->f_link=NULL;

		t=ptr->b_link;
	}

	--ships_on_screen;

	delete ptr;
	return t;
}
char SHIPS::check_mutual_collision(NODE*i)
{
	float y=i->gety();
	float x=i->getx();
	char r=1;

	for(NODE *j=start->f_link;j;j=j->f_link)
		if(j!=i)
		{
			//check for collision along x-axis
			//if true...reflect
			if(j->gety()==y)
			{
				if(x+r==j->getx()-1||x-r==j->getx()+1||x+r==j->getx()+1||x-r==j->getx()-1)					{
						j->RevVelocityx();	//reversing direction
						j->clear_ship();
						j->motionx();
						check_mutual_collision(j);
						j->clear_ship();
						return 2;
					}
				if(abs(i->getx() - j->getx())<1)
					i->RevVelocityx();


			}

		}
	return FALSE;

}
void SHIPS::destroy_all()
{
	NODE* t=NULL;
	for(NODE* i=start;i;i=i->f_link)
		delete t,t=i;
	temp=start;

}

SHIPS Ships;

/****************************************************
*****************************************************/
/*
  Class whose objects are bullets and
  and the functions that determine the behaviour of the bullets

*/
class BULLETS
{
	//each node is a ship in the list of ships
	class NODE
	{
		char ch,color,type;  		//character ,color and type(enemy/player)
		char x;                 	//x-coordinate of the bullet
		float y,y_prev,velocity;        //y-coordinate,previous y-coordinate,velocity
		float damage;                   //damage caused by the bullet

	   public:

		NODE* f_link,*b_link;
		void draw_bullet()
		{
			gotoxy(x,floor(y));
			textcolor(color);
			cprintf("%c",ch);
		}
		void clear_bullet()
		{
			gotoxy(x,floor(y_prev));
			cprintf(" ");
		}
		void CCord()
		{
			y_prev=y;
		}
		char motion()
		{
			y_prev=y;
			y+=velocity;
			if(!_rangeof(1,y,24))
			{
				velocity=0;
				return TRUE;
			}
			return FALSE;
		}
		float GetDamage()
		{
			return damage;
		}
		char getx()
		{
			return x;
		}
		float gety()
		{
			return y;
		}
		char gettype()
		{
			return type;
		}
		NODE(char,char,float);
		~NODE()
		{
			f_link=b_link=NULL;
			x=y=y_prev=0;
			color=ch=damage=0;
		}
	};
	int bullet_count[2];		//stores ammo count
	NODE*start,*temp;

     public:

	int GetBulletCount(char type)
	{
		return bullet_count[type];
	}
	void UpdateBulletCount(char dx,char type)
	{
		if(bullet_count[type]==(type+1)*10)
			return;

		bullet_count[type]+=dx;

		if(bullet_count[type]>(type+1)*10)
			bullet_count[type]=(type+1)*10;

	}
	void initialize_bullets(int b0=0,int b1=0)
	{
		bullet_count[0]=b0;
		bullet_count[1]=b1;
	}
	void draw_bullets();				//draws all the bullets 
	void clear_bullets();				//clears all the bullets
	void move_bullets();				//moves all the bullets
	void bullet_shot(char type,char x,float y);	//adds a new bullet to the list
	NODE* delete_bullet(NODE*ptr);			//deletes the passed node from the list
	void delete_all();				//deletes all the nodes from the list
	void check_mutual_collision(NODE *i);		//checks collision among bullets and takes action

	float check(SHIPS::NODE*);			//checks collision of bullets with the ships

	BULLETS()
	{
		start=temp=NULL;
		bullet_count[0]=bullet_count[1]=0;
	}
	~BULLETS()
	{
		delete_all();
	}
};

BULLETS::NODE::NODE(char t,char xn, float yn)
{
	f_link=b_link=NULL;
	x=xn,y=yn;
	type=(sign(t)<0)?TRUE:FALSE;
	switch(t)
	{
		case -2 : velocity=-1.0;
			  color=RR;
			  damage=-2.0;
			  ch=4;
			  break;

		case -1 : velocity=-1.0;
			  color=G;
			  damage=-3.0;
			  ch=24;
			  break;

		case 0  : velocity=-1.0;
			  color=W;
			  damage=-0.5;
			  ch='.';
			  break;

		case  1 : velocity=1;
			  color=PK;
			  damage=-2;
			  ch='';
			  break;

		case  2 : velocity=1;
			  color=Y;
			  damage=-4;
			  ch='³';
			  break;

		case 10 : velocity=0.5;
			  color=R;
			  damage=5;
			  ch=3;
			  break;

		case 11 : type=11;
			  velocity=0.5;
			  color=R;
			  damage=11;
			  ch=15;
			  break;

		case 12 : type=12;
			  velocity=0.5;
			  color=LG;
			  damage=12;
			  ch=15;
			  break;


	}

}


void BULLETS::draw_bullets()
{
	for(NODE*i=start;i;i=i->f_link)
		i->draw_bullet();
}
void BULLETS::clear_bullets()
{
	for(NODE*i=start;i;i=i->f_link)
		i->clear_bullet();
}
void BULLETS::move_bullets()
{
	for(NODE*i=start;i;i=i->f_link)
	{
		if(i->motion())
			i=delete_bullet(i);
		else
			check_mutual_collision(i);

	}
}
inline void BULLETS::bullet_shot(char type,char x,float y)
{
	NODE *ptr=new NODE(type,x,y);
	if(!ptr)
		return;

	if(type<0&&bullet_count[(-1*type)-1]<=0)
	{
		delete ptr;
		return;
	}
	
	if(type<0)
		--bullet_count[(-1*type)-1];

	if(!start)
	{
		start=ptr;
		start->b_link=NULL;
	}
	else
	{
		temp->f_link=ptr;
		ptr->b_link=temp;
	}

	temp=ptr;

}
BULLETS::NODE* BULLETS::delete_bullet(NODE*ptr)
{
	ptr->clear_bullet();
	ptr->CCord();
	ptr->clear_bullet();
	NODE * t=NULL;

	if(ptr==start)
	{
		start=start->f_link;
		(ptr->f_link)->b_link=NULL;
		t=start;
	}
	else
	{
		(ptr->b_link)->f_link=ptr->f_link;
		if(ptr->f_link)
			(ptr->f_link)->b_link=ptr->b_link;
		else
			temp=ptr->b_link,temp->f_link=NULL;

		t=ptr->b_link;

	}

	delete ptr;
	return t;

}
void BULLETS::delete_all()
{
	NODE* t=NULL;
	for(NODE*i=start;i;i=i->f_link)
		delete t,t=i;
	delete t;
}
void BULLETS::check_mutual_collision(NODE *i)
{
	for(NODE *j=start;j;j=j->f_link)
	{
		if(i->gettype()!=j->gettype())
			if(i->getx()==j->getx())
				if(i->gety()==j->gety())
				{
					i->clear_bullet();
					j->clear_bullet();
					j->CCord();
					j->clear_bullet();
					i=delete_bullet(i);
					j=delete_bullet(j);
					return;
				}
	}
}

BULLETS Bullets;

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
float BULLETS::check(SHIPS::NODE *ship)
{
	char r;
	char x_upper,y_upper,x_lower,y_lower;

	if(ship==Ships.Player)
	{
		x_upper=floor(ship->getx())+2;
		y_upper=floor(ship->gety())+1;
		x_lower=floor(ship->getx())-2;
		y_lower=floor(ship->gety())-1;
	}
	else
	{
		x_upper=floor(ship->getx())+1;
		y_upper=floor(ship->gety())+1;
		x_lower=floor(ship->getx())-1;
		y_lower=floor(ship->gety())-1;
	}
	char type_s=ship->gettype();

	for( NODE* i=start ;i; i=i->f_link )
	{
		if( type_s == ((i->gettype()<=0)?1:0) )
			if(_rangeof(x_lower,floor(i->getx()),x_upper))
				if( _rangeof(y_lower,floor(i->gety()),y_upper))
				{
					float d=i->GetDamage();
					i=delete_bullet(i);
					return d;
				}
	}
	return FALSE;

}

char SHIPS::move_ships()
{
	char p_lost=FALSE;
	for(NODE *i=start;i;i=i->f_link)
	{
		if(i==start)
		{
			if(start->_Health()<=0.0)
			{
				if(p_life>0)
				{
					p_life--;
					gotoxy(73+p_life*2,7);
					textcolor(DS);
					textbackground(B);
					cprintf("%c",3);
					textbackground(0);
					regenerate();
					Player->_UpdateHealth(20);
				}
				else
				{
					clrscr();
					p_lost=TRUE;

				}

			}
			start->motionx();

		}
		else
		{
			if(i->_Health()<=0.0)
			{       i->clear_ship();
				i=delete_ship(i);
				continue;
			}
			i->motion();
			if(i->gety()+1>Y_max-3||i->_Health()<=0)
			{
				i->clear_ship();
				i=delete_ship(i);
			}
		}

		float damage=Bullets.check(i);

		if(!damage)
			continue;

		else if(damage==11||damage==12)
		{
			char dx,t=0;
			if(damage==11)
				dx=10,t=1;
			else if (damage==12)
				dx=5,t=0;

			Bullets.UpdateBulletCount(dx,t);
		}

		else
		{
			i->_UpdateHealth(damage);
			if(!(i->_Health()))
			{
				if(i!=start)
				      i=delete_ship(i);

			}
			if(i==start)
			{
				gotoxy(73,5);
				textcolor(W);
				textbackground(B);
				cprintf("   ");
				gotoxy(73,5);
				cprintf("%d",(int)i->_Health());
				textbackground(0);
			}

		}



	}

	for(i=start->f_link; i; i=i->f_link)
	{
		switch(check_mutual_collision(i))
		{
			case 2 :  i->RevVelocityx();
				  i->clear_ship();
				  i->motionx();
				  i->clear_ship();
				  break;
		}

	}
	return p_lost;

}


void SHIPS::behaviour()
{
	static char flg=0;
	char type_ammo=(flg%3)?12:11;

	//Shooting randomly
	int z= rand()%c_z;
	int d=rand()%c_d+10;//rate of fire ...decrease to increase fire rate

	for(NODE *i=start->f_link ;i;i=i->f_link)
	{       char t=(rand()%c_t)?1:2;
		if(! ( (int)(z+i->getx()*i->gety() ) %d ) )
			Bullets.bullet_shot(t,i->getx(),i->gety()+1);
	}

	 if(!((z*d+326)%73))
		Bullets.bullet_shot(10,rand()%49+5,2);

	 if(!((z*d+c_z+c_d)%(c_z*7+3)) )
		Bullets.bullet_shot(type_ammo,rand()%49+5,2);

	 flg++;

}

/****************************************************
graphical functions
*****************************************************/
void win(char clr)
{
	window(1,1,X_max,Y_max);
	textbackground(0);
	textcolor(0);
	if(clr)
		clrscr();
}
/*
  creates a box(rectangle) with coordinates passed as top left corner,bottom right
  corner,color and background
*/
void box(int x1,int y1,int x2,int y2,char color,char bkg)
{

	int i;
	textcolor(color);
	textbackground(bkg);
	gotoxy(x1,y1);
	cprintf("Ú");
	gotoxy(x2,y1);
	cprintf("¿");
	gotoxy(x1,y2);
	cprintf("À");
	gotoxy(x2,y2);
	cprintf("Ù");

	for(i=x1+1;i<x2;++i)
	{
		gotoxy(i,y1);
		cprintf("Ä");
		gotoxy(i,y2);
		cprintf("Ä");

	}
	for(i=y1+1;i<y2;++i)
	{
		gotoxy(x1,i);
		cprintf("³");
		gotoxy(x2,i);
		cprintf("³");
	}


}

//#################################################################
/*
displays a loading bar from the coordintae (x,y) till (x+n) of color clr and time dealy d
*/
void loading_bar(int d=60,int n=60,int x=5,int y=5,char clr=B)
{

	textcolor(W+BLINK);
	gotoxy(x+n/4-3,y+1);
	cprintf("LOADING GAME");
	textcolor(clr);
	for(int i=x;i<=n;i++)
	{
		gotoxy(i,y);
		cprintf("%c",219);
		delay(d);
	}
	delay(300);

	gotoxy(x+n/4-3,y+1);
	cprintf("            ");
	for(i=x;i<=n;i++)
	{
		gotoxy(i,y);
		cprintf(" ");

	}



}

void loading()	//the loading screen
{
	char colr[]={B,LB,LG,PK,R};
	clrscr();
	win();
	for(int z=0;z<11;++z)
	{
		box(2*z+1,z+1,80-2*z,24-z,colr[z%5]);
		delay(300-z*30);
	}

	loading_bar(50,59,23,12,RR);
	for(z=10;z>=0;--z)
	{
		box(2*z+1,z+1,80-2*z,24-z,0);
		delay(z*20);
	}
}

void display_title()		//displays the games title screen
{
	clrscr();
	win();
	textbackground(0);
	int x=10,y=5;

	textcolor(11);
	gotoxy(x,y++);cprintf("ÚÄÄÄÄÄ ÚÄÄÄÄ¿ ÚÄÄÄÄ¿ ÚÄÄÄÄ  ÚÄÄÄÄ ");
	gotoxy(x,y++);cprintf("³      ³    ³ ³    ³ ³      ³     ");
	gotoxy(x,y++);cprintf("ÀÄÄÄÄ¿ ÃÄÄÄÄÙ ÃÄÄÄÄ´ ³      ÃÄÄÄÄ ");
	gotoxy(x,y++);cprintf("     ³ ³      ³    ³ ³      ³     ");
	gotoxy(x,y++);cprintf("ÄÄÄÄÄÙ ³      ³    ³ ÀÄÄÄÄ  ÀÄÄÄÄ ");

	x=10,y=13;
	textcolor(13+BLINK);
	gotoxy(x,y++);cprintf(" ÄÄÂÄÄ ÚÄÄÄÄ¿  ³%c%c%c%c%c%c%c³ ÚÄÄÄÄ¿ ÚÄÄÄ,  ÚÄÄÄÄ  ÚÄÄÄÄ¿  ÚÄÄÄÄÄ  ",4,4,4,4,4,4,4);
	gotoxy(x,y++);cprintf("   ³   ³    ³  ú³%c   %c³ú ³    ³ ³    \\ ³      ³    ³  ³      ",4,4);
	gotoxy(x,y++);cprintf("   ³   ³    ³   ú³%c %c³ú  ÃÄÄÄÄ´ ³    ³ ÃÄÄÄÄ  ÃÄÄÄÄÙ  ÀÄÄÄÄ¿  ",4,4);
	gotoxy(x,y++);cprintf("   ³   ³    ³    ú³%c³ú   ³    ³ ³    / ³      ³'\\          ³  ",4);
	gotoxy(x,y++);cprintf(" ÄÄÁÄÄ ³    ³     ú³ú    ³    ³ ÀÄÄÄ'  ÀÄÄÄÄ  ³   \\   ÄÄÄÄÄÙ  ");
	getch();


}

char confirmation()				//request confirmation from user to quit
{						//if confirmed return TRUE else FALSE
	const int x1=23,y1=10,x2=57,y2=14;
	char buffer[350];
	char box_fill=0,highlight_text=B;
	win();
	gettext(x1,y1,x2,y2,buffer);

	box(x1,y1,x2,y2);
	window(x1+1,y1+1,x2-1,y2-1);
	textbackground(box_fill);	//box fill color
	clrscr();


	gotoxy(3,1);
	textcolor(W);
	char *str="Are you sure you want to quit?";
	cprintf("%s",str);


	char f=1;
	char inp=0;
	char Opt[2][4]={ "YES", "NO" };
	//condition checking
	//_setcursortype(_NOCURSOR);
	while(1)
	{
		for(int i=0;i<2;i++)
		{
			textbackground(box_fill);
			textcolor(R);
			if(f==i)
				textbackground(highlight_text);
			gotoxy(i*10+10,3);
			cprintf("%s",Opt[i]);
		}
		textbackground(0);
		if(kbhit())
		{
			inp=getch();
			switch(inp)
			{

				case   0 :  inp=getch();
					    switch(inp)
					    {
						case RIGHT: if(f)
								f=0;
							    else
								f=1;
							    break;

						case LEFT : if(!f)
								f=1;
							    else
								f=0;
							    break;
					    }//End of inner switch
					    break;

				case ENTER:  win();
					     puttext(x1,y1,x2,y2,buffer);
					     textbackground(0);
					     return !f;

			}//End of outer switch
		}//End of if
	}


}

//*************************************************************************
//displays a box with text "Regenarting for 5 secs"
void regenerate()
{
	char buff[240];
	gettext(22,9,53,11,buff);
	box(22,9,53,11,W);
	window(23,10,52,10);
	clrscr();
	gotoxy(2,1);
	textcolor(R+BLINK);
	cprintf("Regenerating");
	textcolor(R);
	for(char i=0;i<14;i++)
		cprintf("."),delay(125);

	delay(250);
	win();
	puttext(22,9,53,11,buff);

}
//displays the youlost message when you lose
void you_lost()
{
	box(27,9,48,11,R);
	window(28,10,47,10);
	textbackground(B);
	clrscr();
	gotoxy(6,1);
	textcolor(Y+BLINK);
	cprintf("YOU LOST!!");
	delay(1250);
	win();

}
//pauses the game until you press P
void pause()
{
	const int x1=27,y1=10,x2=53,y2=14;
	char buffer[350];

	gettext(x1,y1,x2,y2,buffer);
	int i;
	box(x1,y1,x2,y2);
	char box_fill=0;

	window(x1+1,y1+1,x2-1,y2-1);
	textbackground(box_fill);	//box fill color
	clrscr();
	window(1,1,80,Y_max);

	//display text
	gotoxy(x1+7,y1+1);
	textcolor(W);

	cprintf("Game Paused");

	gotoxy(x1+4,y1+3);
	textbackground(0);
	textcolor(R);
	cprintf("Press P to continue");
	char ch;
	ch=tolower(ch);
	while(1)
	{
		if(kbhit())
		{
			ch=getch();
			if(ch=='p')
				break;
		}
	}
	puttext(x1,y1,x2,y2,buffer);


}
//the game page where all information like health,score etc is displayed during gameplay
void game_page()
{
	win(1);
	int i,j;

	box(1,1,60,25,LG);
	for(i=2;i<60;++i)
	{
		gotoxy(i,1);
		cprintf(" ");
		gotoxy(i,25);
		cprintf(" ");
	}

	box(62,1,80,19);
	window(63,2,79,18);
	textbackground(B);
	clrscr();
	gotoxy(2,2);
	textcolor(LB);
	cprintf(" SCORE : ");
	textcolor(PK);
	gotoxy(2,4);
	textcolor(PK);
	cprintf("HEALTH : 20");
	gotoxy(2,6);
	textcolor(R);
	cprintf(" LIFES : %c %c %c",3,3,3);
	gotoxy(2,8);
	textcolor(S);
	cprintf(" ARMED : ");
	textcolor(W);
	cprintf(" %c",249);
	win();
	box(64,11,78,17,LB);
	window(65,12,77,16);
	textbackground(0);
	clrscr();
	gotoxy(1,1);
	textcolor(W);
	cprintf("BULLET   AMMO");
	textcolor(W);
	char b[3][2]={249,W,25,LG,4,R};
	for(i=3;i<6;++i)
	{

		gotoxy(4,i);
		textcolor(b[i%3][1]);
		cprintf("%c",b[i%3][0]);
		textcolor(W);
		gotoxy(8,i),cprintf(":");
		if(i==3)
			cprintf(" Inf. ");

	}

	win();

	textcolor(RR);
	gotoxy(72,24);
	cprintf("EXIT:Esc");

	textcolor(PK);
	gotoxy(62,24);
	cprintf("PAUSE:P");

	textcolor(Y);
	gotoxy(65,21);
	cprintf("BULLET + : Z");
	gotoxy(65,22);
	cprintf("BULLET - : X");

}

//***************************************************************************
//display the credits
void credits()
{
	win(1);

	box(1,1,80,24,B);

	for(int i=0;i<34;i++)
	{
		textcolor(PK);
		gotoxy(5+i,2);
		cprintf(" CRE");
		gotoxy(75-i,2);
		cprintf("DITS ");
		delay(40);
	}
	gotoxy(38,3);
	textcolor(Y);
	cprintf("*********");
	while(!kbhit())
	{
		for(i=0;i<34;i++)
		{
			textcolor(LB);
			gotoxy(5+i,5);
			cprintf("  Alfred");


			textcolor(LG);
			gotoxy(70-i,6);
			cprintf("   Aswin ");

			if(i==32)
				continue;
			textcolor(R);
			gotoxy(39,22-i*0.5);
			cprintf(" John ");

			gotoxy(39,23-i*0.5);
			cout<<"         ";
			delay(40);

		}

		for(i=34;i>0;i--)
		{
			textcolor(LB);
			gotoxy(5+i,5);
			cprintf("  Alfred ");


			textcolor(LG);
			gotoxy(70-i,6);
			cprintf("   Aswin ");

			if(i==31)
				continue;
			textcolor(R);
			gotoxy(39,22-i*0.5);
			cout<<"         ";
			gotoxy(39,23-i*0.5);
			cprintf(" John ");
			delay(40);

		}


	}
	i=getch();


}

//********************************************************************************
//dipslay the pages of the how to play option...pages 1 to 5
void p1()
{
	box(1,1,X_max-1,Y_max,RR);

	gotoxy(36,3);
	textcolor(LG);
	cprintf("CONTROLS");
	int x=22,y=6;

	char bc=B,tc=W;

	gotoxy(x,y);
	textcolor(bc);
	cprintf("DOWN ARROW KEY");
	textcolor(tc);
	cprintf("   -  stops you ship from moving ");

	gotoxy(x,y+2);
	textcolor(bc);
	cprintf("LEFT ARROW KEY");
	textcolor(tc);
	cprintf("   -  moves your ship left");

	gotoxy(x,y+4);
	textcolor(bc);
	cprintf("RIGHT ARROW KEY");
	textcolor(tc);
	cprintf("  -  moves your ship right");

	gotoxy(x,y+6);
	textcolor(bc);
	cprintf("   SPACE BAR");
	textcolor(tc);
	cprintf("     -  shoot");

	gotoxy(x,y+8);
	textcolor(bc);
	cprintf("       Z");
	textcolor(tc);
	cprintf("         -  next weapon ");

	gotoxy(x,y+10);
	textcolor(bc);
	cprintf("       X");
	textcolor(tc);
	cprintf("         -  previous weapon ");

	gotoxy(x,y+12);
	textcolor(bc);
	cprintf("       P");
	textcolor(tc);
	cprintf("         -  pause ");

	gotoxy(x,y+14);
	textcolor(bc);
	cprintf("      Esc");
	textcolor(tc);
	cprintf("        -  quit game");

	textcolor(DS);
	gotoxy(X_max-9,Y_max-1);
	cprintf("\\ : Next");




}

void p2()
{
	int y=9,x=5;
	box(1,1,X_max-1,Y_max,RR);

	box(32,2,51,6,LB);
	box(34,3,49,5,LG);
	gotoxy(35,4);
	textcolor(PK+BLINK);
	cprintf("SPACE ");
	textcolor(LB+BLINK);
	cprintf("INVADERS");

	textcolor(W);
	gotoxy(x,y);
	cprintf("Aliens have attcked planet Earth.You have been assigned the duty to fend ");
	gotoxy(x,y+2);
	cprintf("them off.The enemy is sending wave after wave of their battleships to ");
	gotoxy(x,y+4);
	cprintf("destroy civilization as we know it.They have destroyed most our troops. ");
	gotoxy(x,y+6);
	cprintf("You are one of the few survivors.The last line of defense.Protect the");
	gotoxy(x,y+8);
	cprintf("planet as long as you can and dont forget to destroy as many ships as");
	gotoxy(x,y+10);
	cprintf("you can.");

	textcolor(DS);
	gotoxy(2,Y_max-1);
	cprintf("/ : Prev");
	gotoxy(X_max-9,Y_max-1);
	cprintf("\\ : Next");

}
void p3()
{
	int y=5,x=3;
	box(1,1,X_max-1,Y_max,RR);
	textcolor(B);
	gotoxy(31,2);
	cprintf("INFO AND INTELLIGENCE ");
	textcolor(RR);
	gotoxy(8,5);
	cprintf("THE ENEMY");
	textcolor(LB);
	gotoxy(x+2,y+3);
	cprintf("Û");
	textcolor(Y+BLINK);
	gotoxy(x+3,y+3);
	cprintf("°");
	textcolor(LB);
	gotoxy(x+4,y+3);
	cprintf("Û");
	textcolor(R);
	gotoxy(x,y+4);
	cprintf("   Ø  " );
	textcolor(W);
	gotoxy(x+8,y+3);
	cprintf("- A brilliant piece of engineering;we still havent figuerd out how ");
	gotoxy(x+8,y+4);
	cprintf("  they work but they can be destroyed; .75 calibre bullets have ");
	gotoxy(x+8,y+5);
	cprintf("  little effect, advise to use the mini-Hellfire thermonuclear ");
	gotoxy(x+8,y+6);
	cprintf("  missile to destroy them.");
	textcolor(P);
	gotoxy(x+3,y+8);
	cprintf("");
	gotoxy(x+8,y+8);
	textcolor(W);
	cprintf("- alien equivalent to our .75 claibre but they do real damage to ");
	gotoxy(x+8,y+9);
	cprintf("  our ships;try not to get hit a lot,a few hits are fine");

	y++;
	textcolor(Y);
	gotoxy(x+3,y+10);
	cprintf("³ ");
	textcolor(W);
	gotoxy(x+8,y+10);
	cprintf("- a special nuclear fusion waepon designed for close combat;a few ");
	gotoxy(x+8,y+11);
	cprintf("  hits is all it trakes to destroy our most powerful space vehicles;");
	gotoxy(x+8,y+12);
	cprintf("  the enemy use them rarely,but they would use it if you really piss");
	gotoxy(x+8,y+13);
	cprintf("  them off ");

	textcolor(DS);
	gotoxy(2,Y_max-1);
	cprintf("/ : Prev");
	gotoxy(X_max-9,Y_max-1);
	cprintf("\\ : Next");
}

void p4()
{
	int y=3,x=3;
	box(1,1,X_max-1,Y_max,RR);
	textcolor(B);
	gotoxy(31,2);

	cprintf("The Defenders(You)   ");
	textcolor(W);
	gotoxy(x+3,y+2);
	cprintf("³");

	textcolor(R);
	gotoxy(x+2,y+3);
	cprintf("É");

	textcolor(R);
	gotoxy(x+3,y+3);
	cprintf("Î");

	textcolor(R);
	gotoxy(x+4,y+3);
	cprintf("»" );

	textcolor(B);
	gotoxy(x+1,y+4);
	cprintf("Å");

	textcolor(G);
	gotoxy(x+2,y+4);
	cprintf("É");

	textcolor(R);
	gotoxy(x+3,y+4);
	cprintf("Î");

	textcolor(G);
	gotoxy(x+4,y+4);
	cprintf("»" );

	textcolor(B);
	gotoxy(x+5,y+4);
	cprintf("Å");

	textcolor(W);
	gotoxy(x+8,y+3);
	cprintf("- most powerful ship mankind has ever built.Only hope to fend of the");
	gotoxy(x+8,y+4);
	cprintf("  alien attack.Can survive wave after wave of attacks.");

	gotoxy(x+3,y+6);
	textcolor(W);
	cprintf("%c",249);
	gotoxy(x+8,y+6);
	cprintf("- .75 calibre bullets;can penetrate an armoured tank but has minimal");
	gotoxy(x+8,y+7);
	cprintf("   yet considerable effect on enemy ships;repeated hits on enemy");
	gotoxy(x+8,y+8);
	cprintf("   ships can penetrate and even destroy,if you survive long enough.");
	gotoxy(x+8,y+9);
	cprintf("   Due to space junk recycling,the ammo is infinite.");

	gotoxy(x+3,y+10);
	textcolor(G);
	cprintf("%c",24);
	textcolor(W);
	gotoxy(x+8,y+11);
	cprintf("- the mini-Hellfire thermonuclear missile;the most powerful");
	gotoxy(x+8,y+12);
	cprintf("  close combat weapon in our arsenal; a single hit is all it");
	gotoxy(x+8,y+13);
	cprintf("  takes to destroy an enemy ship;you can carry only ten of these ");
	gotoxy(x+8,y+14);
	cprintf("  in your ship,use them wisely.If you run out of ammo,wait for ");
	gotoxy(x+8,y+15);
	cprintf("  reinforcement packages ");


	gotoxy(x+3,y+17);
	textcolor(RR);
	cprintf("%c",4);
	textcolor(W);
	gotoxy(x+8,y+17);
	cprintf(" - the micro-Hellfire thermonuclear missile;a less-powerful version");
	gotoxy(x+8,y+18);
	cprintf("   of mini-Hellfire thermonuclear missile; two hits is all it takes ");
	gotoxy(x+8,y+19);
	cprintf("   to destroy an enemy ship;you can carry twenty of those,use ");
	gotoxy(x+8,y+20);
	cprintf("   use them wisely");

	textcolor(DS);
	gotoxy(2,Y_max-1);
	cprintf("/ : Prev");
	gotoxy(X_max-9,Y_max-1);
	cprintf("\\ : Next");

}
void p5()
{
	box(1,1,X_max-1,Y_max,RR);

	gotoxy(31,3);
	textcolor(B);
	cprintf("REINFORCEMENTS");

	int x=3,y=6;
	textcolor(Y);
	gotoxy(x,y);
	cprintf("  The martian colony of ours send reinforcements which restores your");
	gotoxy(x,y+1);
	cprintf("  ships health,ammo.Dont forget to collect them ");
	++y;
	gotoxy(x+3,y+3);
	textcolor(RR);
	cprintf("%c",3);
	gotoxy(x+5,y+3);
	textcolor(W);
	cprintf("- repairs damage by five points;dont shoot at them or you lose them.");
	gotoxy(x+3,y+6);
	textcolor(LG);
	cprintf("%c",15);
	gotoxy(x+5,y+6);
	textcolor(W);
	cprintf("- contains five mini-hellfire missiles;dont shoot them as well.");
	++y;
	gotoxy(x+3,y+8);
	textcolor(PK);
	cprintf("%c",15);
	gotoxy(x+5,y+8);
	textcolor(W);
	cprintf("- contains ten micro-Hellfire thermonuclear missile;dont shoot");
	gotoxy(x+5,y+9);
	cprintf("  them as well.");

	textcolor(DS);
	gotoxy(2,Y_max-1);
	cprintf("/ : Prev");

}

//th how to play option...switch between the pages to learn about the game
void how_to_play()
{
	char f=1;
	while(f)
	{
		win(1);
		switch(f)
		{
			case 1 : p1();
				 break;

			case 2 : p2();
				 break;

			case 3 : p3();
				 break;

			case 4 : p4();
				 break;

			case 5 : p5();
				 break;
		}
		while(1)
		{
			if(kbhit())
			{
				char ch=getch();
				if(ch=='/')
				{
					f--;
					(f==0)?f=1:f;
					break;
				}
				else if(ch=='\\')
				{
					f++;
					(f==6)?f=5:f;
					break;
				}
				else if(ch==ESC)
				{
					f=0;
					break;
				}
			}
		}


	}

}



/********************************************************************
gameplay functions
*********************************************************************/
char user_input()
{
	static char bullet_mode=0,xp=Ships.Player->getx();
	char f=0;
	if(kbhit())
	{
		char input=getch();
		input=toupper(input);

		switch(input)
		{

			case 0:	input=getch();
				char diff=Ships.Player->getx()-xp;
				xp=Ships.Player->getx();
				switch(input)
				{
					case LEFT:      if(diff>0)
								Ships.Player->RevVelocityx();
							if(!diff)
								Ships.Player->_ChangeVelocityx(-1);
							break;

					case RIGHT:    if(diff<0)
								Ships.Player->RevVelocityx();
						       if(!diff)
								Ships.Player->_ChangeVelocityx(1);
							break;

					case DOWN:     Ships.Player->_ChangeVelocityx(0);
								break;
				}//End of inner switch
				break;

		case SPACE:     Bullets.bullet_shot(bullet_mode,Ships.Player->getx(),20);
				break;

		case 'Z'  :     f=1;
				--bullet_mode;
			       if(bullet_mode==-3)
				    bullet_mode=0;
			       break;

		case 'X'  :     f=1;
				++bullet_mode;
			       if(bullet_mode==1)
				   bullet_mode=-2;
			       break;

		case 'P'  :    pause();
			       break;

		case ESC  :    if(confirmation())
			       {
					Bullets.delete_all();
					Ships.destroy_all();
					return TRUE;
				}
				break;
		}//End of outer switch
		if(f)
		{
			char b[3][2]={249,W,25,LG,4,R};
			gotoxy(74,9);
			textcolor(b[-bullet_mode][1]);
			textbackground(B);
			cprintf("%c",b[-bullet_mode][0]);
			textbackground(0);

		}
		return FALSE;
	}
	return FALSE;



}
inline void screen_render()
{
	delay(40);

	Ships.clear_ships();
	Bullets.clear_bullets();

	Ships.draw_ships();
	Bullets.draw_bullets();

	textcolor(G);
	gotoxy(74,15);
	cprintf("  ");
	gotoxy(74,15);
	cprintf("%d",Bullets.GetBulletCount(0));
	textcolor(RR);
	gotoxy(74,16);
	cprintf("  ");
	gotoxy(74,16);
	cprintf("%d",Bullets.GetBulletCount(1));
	textcolor(11);
	textbackground(B);
	gotoxy(73,3);
	cprintf("%d",Ships.getScore());
	textbackground(0);


}
char update_environment()
{
	Bullets.move_bullets();
	char f=Ships.move_ships();
	Ships.behaviour();
	if(f)
	{	you_lost();
		while(!kbhit());
	}
	return f;
}

void clear_memory()
{
	Ships.destroy_all();
	Bullets.delete_all();
}

void initialize_level(char difficulty,int& spawn_rate,char& max_no_ships)
{

	if(difficulty>0)
	{
		int k=-1;
		clear_memory();
		Ships.reset_counter();
		for(char i=0;i<difficulty+1;i++)
			Ships.new_ship(rand()%55+5,rand()%3+2,k),k*=-1;

		Bullets.initialize_bullets(10,10);
	}
	else
		difficulty*=-1;
	switch(difficulty)
	{
		case 1: spawn_rate=137;
			max_no_ships=5;
			Ships.difficulty_weights(49,123,27);
			break;

		case 2: spawn_rate=123;
			max_no_ships=10;
			Ships.difficulty_weights(41,101,19);
			break;

		case 3: spawn_rate=101;
			max_no_ships=15;
			Ships.difficulty_weights(37,99,13);
			break;

		case 9: spawn_rate=50;                    	//will not survive for long
			max_no_ships=CHAR_MAX;
			Ships.difficulty_weights(20,20,2);
			break;

	}
}
//##############################################################################
void Game()
{
	int spawn_rate=0;;
	char max_no_ships=0;
	char k=-1,f1=0,f2=0;
	char difficulty=1;
	initialize_level(difficulty,spawn_rate,max_no_ships);
	clock_t s,e;
	s=clock();
	difficulty*=-1;
	char st=0;
	while(!f1&&!f2)
	{
		screen_render();
		f2=update_environment();

		if(kbhit())
			f1=user_input();

		if(Ships.Ships_onScreen()<max_no_ships&&!(rand()%spawn_rate))
			Ships.new_ship(rand()%55+5,rand()%3+2,k),k*=-1;

		if(difficulty!=-9)
		{
			e=clock();
			if(((e-s)/60)>(1+st))
			{
				initialize_level(difficulty,spawn_rate,max_no_ships);
				s+=1;
				--difficulty;
				if(difficulty==-4)
					difficulty=-9;
			}
		}

	}
	clear_memory();
	H.check_score(Ships.getScore());

}

//***********************************************************************************
//#################################################################################
//displays the main menu
void display_main_menu()
{
	top:

	win(1);
	box(5,3,76,22,R);
	box(3,2,78,23,Y);
	box(1,1,80,24,G);
	box(30,6,50,18,B);
	box(32,7,48,17,LB);
	window(33,8,47,16);
	clrscr();

	//menu
	const int item_number=4;
	char menu_items[item_number][25]={ 	" NEW GAME ",
						" HIGH SCORES ",
						" HOW TO PLAY ",
						" CREDITS "	};

	char f=0;


	char flag=TRUE;
	while(flag)
	{

		for(int j=0;j<item_number;j++)
		{
			textbackground(0);
			textcolor(PK);
			gotoxy(2,2+j*2);
			if(f==j)
			{
				textbackground(W);
				textcolor(0);

			}
			cprintf("%s",menu_items[j]);
		}


		if(kbhit())
		{

			char ch=getch();
			switch(ch)
			{
				case 0 :  ch=getch();
					  switch(ch)
					  {
						case UP  :  if(f==0)            //top position
								f=item_number-1;
							    else
								f--;
							    break;

						case DOWN:  if(f==item_number-1) //bottom position
								f=0;
							    else
								f++;
							    break;
					  }
					  break;

				case ENTER: switch(f)
					    {
						case 0: game_page();
							Game();
							break;
						case 1: H.high_scores();
							break;
						case 2: how_to_play();
							break;
						case 3: credits();
							break;

					    };

					    goto top;


				case ESC: if(confirmation())
						flag=FALSE;
					  else
					  {
						 window(33,8,47,16);
						 goto top;
					  }
					  break;

			}//end of outer switch



		}

	}
	win();


}

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

void main()
{
	time_t t;
	srand((unsigned)t);
	_setcursortype(_NOCURSOR);

	loading();
	display_title();
	display_main_menu();

	win(1);

}



