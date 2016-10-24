#include <Windows.h>
#include <gl/GLU.h>
#include <gl/glut.h>
#include <stdio.h>
#include <gl/GLAUX.H>
#include <math.h>

#define MonAngle 20
#define PlayAngle 10
#define PI         3.1459
#define WORLD_SIZE   100.0
#define MAZE_SIZE   10
#define texize 7

GLfloat m_fFontColor[3] = {1.0,1.0,0.0};

/*캐릭터와 몬스터 애니메이션에 사용되는 변수*/
GLfloat M_Arm = 0, M_Leg = 0, M_Motion = 0; 
GLboolean M_ArmCheck = TRUE, M_LegCheck = TRUE, M_MoCheck =TRUE; 
GLfloat P_Arm = 0, P_Leg = 0, P_Head = 0;
GLboolean P_ArmCheck = TRUE, P_LegCheck = TRUE, P_HeadCheck = TRUE;
GLboolean StopCheck = TRUE;  //캐릭터가 이동할 때 마다 움직이도록

/*캐릭터 공격 애니메이션*/
GLboolean Click = FALSE , StickCheck = TRUE, StickArmCheck = TRUE;
GLfloat Stick = 0, StickArm = 0;
GLint StickCount = 0; 
//StickCount가 50을 넘지 않았으면 몬스터를 죽일 수 있다.
//그 이외에는 플레이어의 목숨이 깎인다.

GLboolean MenuCheck = FALSE;
GLint MenuCount = 1;

GLuint tex[texize];   // Texture Mapping을 하기 위한 Texture 이미지의 개수를 위한 배열 변수
GLUquadricObj *PlayerFace = gluNewQuadric();
GLUquadricObj *BoxImage = gluNewQuadric();

/*미로 제작 (현재 2층)*/
static int maze[MAZE_SIZE][MAZE_SIZE] = {

	{1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 3, 0, 0, 0, 0, 3, 1, 1, 1},
	{1, 1, 4, 1, 0, 1, 0, 0, 1, 1},
	{1, 1, 0, 1, 0, 1, 1, 4, 0, 1},
	{1, 0, 0, 1, 1, 0, 1, 1, 0, 1},
	{1, 0, 1, 0, 5, 0, 1, 1, 0, 1},
	{1, 0, 0, 0, 1, 1, 0, 5, 0, 1},
	{1, 1, 1, 1, 0, 1, 0, 1, 1, 1},
	{1, 0, 0, 5, 0, 0, 3, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 2, 1}
};
static int maze_2[MAZE_SIZE][MAZE_SIZE] = {
	{1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
	{1, 0, 1, 1, 0, 1, 0, 0, 0, 1},
	{1, 0, 0, 1, 0, 1, 1, 0, 1, 1},
	{1, 1, 1, 1, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 1, 0, 1, 0, 1},
	{1, 0, 1, 1, 0, 1, 0, 1, 0, 1},
	{1, 1, 0, 0, 0, 1, 0, 1, 0, 1},
	{1, 6, 0, 1, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

static int Time=0;
double SPEED = 0.8;
double worldSize, wallSize;
double screenWidth, screenHeight;
double sx,sy,sz,copx, copy, copz, horangle, verangle;
int lastX, lastY, tempX, tempY;
bool mousedown;


//몬스터의 수와 몬스터 변경사항을 알기 위한 변수
int arr3_amount=0;
int arr4_amount=0;
int arr5_amount=0;
int arr6_amount=0;
//몬스터와 보물의 위치정보를 저장하고 있는 배열
double arr3_position[10][6] = {0,};
double arr4_position[10][6] = {0,};
double arr5_position[10][6] = {0,};
double arr6_position[10][6] = {0,};

AUX_RGBImageRec *LoadBMP(char *Filename) {  // Bitmap 이미지를 호출한다.
     FILE *File=NULL;

     if (!Filename) return NULL;
     File=fopen(Filename,"r");
     if (File) {
          fclose(File);
          return auxDIBImageLoad(Filename);
     }

     return NULL;
}

void LoadGLtex( ) {  // Bitmap 이미지 호출하여 Texture 이미지로 변환한다.
     AUX_RGBImageRec *texRec[texize];
     memset(texRec, 0, sizeof(void *)*texize);                                                  

	 //상대 경로 지정
     if((texRec[0]=LoadBMP("..\\img\\playerFace2.bmp"))
		 &&(texRec[1]=LoadBMP("..\\img\\Box.bmp"))
		 &&(texRec[2]=LoadBMP("..\\img\\MENU.bmp"))
		 &&(texRec[3]=LoadBMP("..\\img\\monster.bmp"))
		 &&(texRec[4]=LoadBMP("..\\img\\Wall.bmp"))
		 &&(texRec[5]=LoadBMP("..\\img\\ground.bmp"))
		 &&(texRec[6]=LoadBMP("..\\img\\Guide.bmp"))
		 )
	 {
          for(int i=0; i<texize; i++) {
			  glGenTextures(1, &tex[i]); 
               glBindTexture(GL_TEXTURE_2D, tex[i]);
               glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
               glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
              glTexImage2D(GL_TEXTURE_2D, 0, 3, texRec[i]->sizeX, texRec[i]->sizeY, 0,GL_RGB, GL_UNSIGNED_BYTE, texRec[i]->data);
          }
     }
     for(int i=0; i<texize; i++) {                                                              
          if(texRec[i]) {
               if(texRec[i]->data) free(texRec[i]->data);
               free(texRec[i]);
          } 
     }
     glEnable(GL_TEXTURE_2D);
     glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}


//벽 만들기
void MakeCube(double wallSize, int texIndex)
{
	//앞면
	 glBindTexture(GL_TEXTURE_2D, tex[texIndex]);
	 glBegin(GL_QUADS);
	 {
	  glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	  glTexCoord2f(1,1);
	  glVertex3f(wallSize,wallSize,wallSize);
	  glTexCoord2f(0,1);
	  glVertex3f(-wallSize,wallSize,wallSize);
	  glTexCoord2f(0,0);
	  glVertex3f(-wallSize,-wallSize,wallSize);
	  glTexCoord2f(1,0);
	  glVertex3f(wallSize,-wallSize,wallSize);
	 }
	 glEnd ();

	 //뒷면
	 glBindTexture(GL_TEXTURE_2D, tex[texIndex]);
	 glBegin(GL_QUADS);
	 {
	  glTexCoord2f(1,1);
	  glVertex3f(-wallSize,wallSize,-wallSize);
	  glTexCoord2f(0,1);
	  glVertex3f(wallSize,wallSize,-wallSize);
	  glTexCoord2f(0,0);
	  glVertex3f(wallSize,-wallSize,-wallSize);
	  glTexCoord2f(1,0);
	  glVertex3f(-wallSize,-wallSize,-wallSize);
	 }
	 glEnd ();

	 //윗면
	 glBindTexture(GL_TEXTURE_2D, tex[texIndex]);
	 glBegin(GL_QUADS);
	 {
	  glTexCoord2f(1,1);
	  glVertex3f(-wallSize,wallSize,wallSize);
	  glTexCoord2f(0,1);
	  glVertex3f(wallSize,wallSize,wallSize);
	  glTexCoord2f(0,0);
	  glVertex3f(wallSize,wallSize,-wallSize);
	  glTexCoord2f(1,0);
	  glVertex3f(-wallSize,wallSize,-wallSize);
	 }
	 glEnd ();

	 //아랫면
	 glBindTexture(GL_TEXTURE_2D, tex[texIndex]);
	 glBegin(GL_QUADS);
	 {
	  glTexCoord2f(1,1);
	  glVertex3f(wallSize,-wallSize,-wallSize);
	  glTexCoord2f(0,1);
	  glVertex3f(wallSize,-wallSize,wallSize);
	  glTexCoord2f(0,0);
	  glVertex3f(-wallSize,-wallSize,wallSize);
	  glTexCoord2f(1,0);
	  glVertex3f(-wallSize,-wallSize,-wallSize);
	 }
	 glEnd ();

	 //왼면
	 glBindTexture(GL_TEXTURE_2D, tex[texIndex]);
	 glBegin(GL_QUADS);
	 {
	  glTexCoord2f(1,1);
	  glVertex3f(-wallSize,wallSize,wallSize);
	  glTexCoord2f(0,1);
	  glVertex3f(-wallSize,wallSize,-wallSize);
	  glTexCoord2f(0,0);
	  glVertex3f(-wallSize,-wallSize,-wallSize);
	  glTexCoord2f(1,0);
	  glVertex3f(-wallSize,-wallSize,wallSize);
	 }
	 glEnd ();

	 //오른면
	 glBindTexture(GL_TEXTURE_2D, tex[texIndex]);
	 glBegin(GL_QUADS);
	 {
	  glTexCoord2f(1,1);
	  glVertex3f(wallSize,wallSize,-wallSize);
	  glTexCoord2f(0,1);
	  glVertex3f(wallSize,wallSize,wallSize);
	  glTexCoord2f(0,0);
	  glVertex3f(wallSize,-wallSize,wallSize);
	  glTexCoord2f(1,0);
	  glVertex3f(wallSize,-wallSize,-wallSize);
	 }
	 glEnd ();
	 glPopMatrix ();
}

//바닥만들기
void drawFloor(double wallSize, int texIndex)
{
   //1층
	glEnable( GL_TEXTURE_2D );  
	//출력 이미지
	glBlendFunc(GL_ONE, GL_ONE);
   glBindTexture(GL_TEXTURE_2D, tex[texIndex]);
   glBegin(GL_QUADS);
	{
		glTexCoord2f(1,1);
		glVertex3d(-5, -wallSize / 2.0, -30);
		glTexCoord2f(0,1);
		glVertex3d(-5, -wallSize / 2.0, 105);
		glTexCoord2f(0,0);
		glVertex3d(105, -wallSize / 2.0, 105);
		glTexCoord2f(1,0);
		glVertex3d(105, -wallSize / 2.0, -30);
	} 
  glEnd();
  glDisable( GL_TEXTURE_2D );
   //2층
   glEnable( GL_TEXTURE_2D );
   glBindTexture(GL_TEXTURE_2D, tex[texIndex]);
   glBegin(GL_QUADS);
	{
		glTexCoord2f(1,1);
		glVertex3d(-5, 10.5-wallSize / 2.0, 95);
		glTexCoord2f(0,1);
		glVertex3d(-5, 10.5-wallSize / 2.0, 195);
		glTexCoord2f(0,0);
		glVertex3d(95, 10.5-wallSize / 2.0, 195);
		glTexCoord2f(1,0);
		glVertex3d(95, 10.5-wallSize / 2.0, 95);
	}
   glEnd();
   glDisable( GL_TEXTURE_2D );
}



//몬스터 그리기
void Monster()
{	
	glColor3f(1.0,1.0,1.0);
	glPushMatrix();
		//괴물 몸체
		glRotatef(180, 0.0, 1.0, 0.0);
		glRotatef(-100, 1.0, 0.0, 0.0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, tex[3]);
			gluSphere(PlayerFace, 0.23, 30, 30);
			glDisable(GL_TEXTURE_2D);
		glRotatef(100, 1.0, 0.0, 0.0); //원점으로
		glRotatef(-180, 0.0, 1.0, 0.0);

		glRotatef(M_Arm,1.0,0.0,1.0); //자동회전
		glTranslatef(0.36,0.0,0.0);
			//괴물 팔 right
			glPushMatrix();
			glScalef(1.0,0.2,0.2);
			glutSolidCube(0.2);
			glPopMatrix();
		glTranslatef(-0.36,0.0,0.0); //원점으로 
		glRotatef(-M_Arm,1.0,0.0,1.0); //자동회전 중지

		glRotatef(M_Arm,1.0,0.0,1.0); //자동회전
		glTranslatef(-0.36,0.0,0.0);
			//괴물 팔 left
			glPushMatrix();
			glScalef(1.0,0.2,0.2);
			glutSolidCube(0.2);
			glPopMatrix();
		glTranslatef(0.36,0.0,0.0); //원점으로
		glRotatef(-M_Arm,1.0,0.0,1.0); //자동회전 중지

		glRotatef(M_Leg,1.0,0.0,0.0); //자동회전
		glTranslatef(0.1,-0.28,0.0);
			//괴물다리 right
			glPushMatrix();
			glScalef(0.2,0.7,0.2);
			glutSolidCube(0.2);
			glPopMatrix();
		glTranslatef(-0.1,0.28,0.0); //원점으로
		glRotatef(-M_Leg,1.0,0.0,0.0);

		glRotatef(-M_Leg,1.0,0.0,0.0);//자동회전
		glTranslatef(-0.1,-0.28,0.0);
			//괴물다리 left
			glPushMatrix();
			glScalef(0.2,0.7,0.2);
			glutSolidCube(0.2);
			glPopMatrix();
	glPopMatrix();
}

//플레이어
void Player()
{
	gluQuadricTexture(PlayerFace, GL_TRUE);
	glPushMatrix();
		glScalef(1.0, 1.0, 1.0);
		//몸통 
		glPushMatrix();
			glColor3f(1.0, 0.4,0.1);
			glScalef(0.4, 1.3, 0.4);
			glutSolidCube(0.5);
		glPopMatrix();

		//오른쪽 팔
		glPushMatrix();
			// //자동회전
			glRotatef(-30, 1.0, 0.0, 1.0);
			glRotatef(P_Arm,1.0,0.0,1.0);
			glTranslatef(0.12, 0.3, 0.1);
			glColor3f(1.0, 0.4, 0.1);
			glScalef(1.0, 0.2, 0.2);
			glutSolidCube(0.3);
			//오른쪽 손
			glPushMatrix();
				glColor3f(1.0, 0.95, 0.8);
				glTranslatef(0.25, -0.1, 0.0);
				glTranslatef(0.0, P_Head+0.1, 0.0);
				glScalef(0.2, 1.0, 1.0);
				glutSolidSphere(0.3, 30, 30);
			glPopMatrix();
		glPopMatrix();

		if(Click == TRUE) //몽둥이 애니메이션
		{
			glPushMatrix();
				glRotatef(-60, 1.0, .0, 1.0);
				glTranslatef(-0.3, -0.1, 0.0);
				glRotatef(-30, 0.0, 0.0, 1.0);
				glTranslatef(-0.15, -0.2, 0.0);

				glRotatef(30+Stick, 1.0, 1.0, 1.0);
				glTranslatef(StickArm,0.0,0.0);
				glRotatef(30, 1.0, 0.0, 1.0);
				glRotatef(P_Arm,1.0,0.0,1.0);
				glTranslatef(-0.12, 0.3, 0.0);
				glColor3f(1.0, 0.4, 0.1);
				glScalef(1.0, 0.2, 0.2);
				glutSolidCube(0.3);
					//왼쪽 손
				glPushMatrix();
					glColor3f(1.0, 0.95, 0.8);
					glTranslatef(0.0, P_Head+0.1, 0.0);
					glTranslatef(-0.25, -0.1, 0.0);
					glScalef(0.2, 1.0, 1.0);
					glutSolidSphere(0.3, 30, 30);
					//몽둥이
					glPushMatrix();
						glColor3f(0.7, 0.3, 0.7);
						glTranslatef(0.0, 1.2, 0.0);
						glScalef(0.8, 6.0, 0.8);
						glutSolidSphere(0.3, 30, 30);
					glPopMatrix();
				glPopMatrix();
			glPopMatrix();
		}
		else
		{
		glPushMatrix();
			glRotatef(30, 1.0, 0.0, 1.0);
			glRotatef(P_Arm,1.0,0.0,1.0);
			glTranslatef(-0.12, 0.3, 0.0);
			glColor3f(1.0, 0.4, 0.1);
			glScalef(1.0, 0.2, 0.2);
			glutSolidCube(0.3);
				//왼쪽 손
			glPushMatrix();
				glColor3f(1.0, 0.95, 0.8);
				glTranslatef(0.0, P_Head+0.1, 0.0);
				glTranslatef(-0.25, -0.1, 0.0);
				glScalef(0.2, 1.0, 1.0);
				glutSolidSphere(0.3, 30, 30);
				//몽둥이
				glPushMatrix();
					glColor3f(0.7, 0.3, 0.7);
					glTranslatef(0.0, 1.2, 0.0);
					glScalef(0.8, 6.0, 0.8);
					glutSolidSphere(0.3, 30, 30);
				glPopMatrix();
			glPopMatrix();
		glPopMatrix();
		}

		glRotatef(P_Leg,1.0,0.0,0.0);
		//오른쪽 다리
		glPushMatrix();
			glColor3f(1.0, 0.95, 0.8);
			glTranslatef(0.06, -0.55, 0.0);
			glScalef(0.25,1.5,0.25);
			glutSolidCube(0.3);
			//오른쪽 발
			glPushMatrix();
				glTranslatef(0.0, -0.15, 0.0);
				glScalef(3.0, 0.25, 3.0);
				glColor3f(1.0, 0.4, 0.1);
				glutSolidCube(0.1);
			glPopMatrix();
		glPopMatrix();
		glRotatef(-P_Leg,1.0,0.0,0.0);
		
		glRotatef(-P_Leg,1.0,0.0,0.0);
		//왼쪽 다리
		glPushMatrix();
			glColor3f(1.0, 0.95, 0.8);
			glTranslatef(-0.06, -0.55, 0.0);
			glScalef(0.25,1.5,0.25);
			glutSolidCube(0.3);
			//왼쪽 발
			glPushMatrix();
				glTranslatef(0.0, -0.15, 0.0);
				glScalef(3.0, 0.25, 3.0);
				glColor3f(1.0, 0.4, 0.1);
				glutSolidCube(0.1);
			glPopMatrix();
		glPopMatrix();
		glRotatef(P_Leg,1.0,0.0,0.0);
		//머리
		glPushMatrix();
			glTranslatef(0.0, 0.62, 0.0);
			glRotatef(-100, 1.0, 0.0, 0.0);
			glColor3f(1.0,1.0,1.0);
			glTranslatef(0.0, P_Head, 0.0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, tex[0]);
			gluSphere(PlayerFace, 0.3, 30, 30);
			glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	glPopMatrix();
}

void MonsterTimer(int value){

	//팔 움직이는 애니메이션
	if((M_ArmCheck == TRUE)&&(M_Arm <= MonAngle))
	{
		M_Arm += 3;	
		if(M_Arm > MonAngle)
			M_ArmCheck = FALSE;
	}
	else if((M_ArmCheck == FALSE))
	{
		M_Arm -= 3;
		if(M_Arm <= -MonAngle)
			M_ArmCheck = TRUE;
	}

	//다리 움직이는 애니메이션
	if((M_LegCheck == TRUE) && (M_Leg <= MonAngle))
	{
		M_Leg += 3;
		if(M_Leg > MonAngle)
			M_LegCheck = FALSE;
	}
	else if((M_LegCheck == FALSE))
	{
		M_Leg -= 3;
		if(M_Leg <= -MonAngle)
			M_LegCheck = TRUE;
	}

	//몬스터 움직이는 애니메이션
	if((M_MoCheck == TRUE) && (M_Motion <= 1.3))
	{
		M_Motion += 0.05;
		if(M_Motion > 1.3)
			M_MoCheck = FALSE;
	}
	else if(M_MoCheck == FALSE)
	{
		M_Motion -= 0.05;
		if(M_Motion <= - 3)
			M_MoCheck = TRUE;
	}

	glutPostRedisplay();
	glutTimerFunc(50, MonsterTimer, 1);
	
}

void PlayerTimer(int value){
	
	//플레이어의 움직임을 컨트롤
	//TRUE : 사용자가 키보드 방향키를 누르고 있지 않으니 멈춤
	//FALSE: 플레이어 애니메니션 작동
	if(StopCheck == FALSE)
	{
		//팔
		if((P_ArmCheck == TRUE)&&(P_Arm <= PlayAngle))
		{
			P_Arm += 2;	
			if(P_Arm > PlayAngle)
				P_ArmCheck = FALSE;
		}
		else if((P_ArmCheck == FALSE))
		{
			P_Arm -= 2;
			if(P_Arm <= -PlayAngle)
				P_ArmCheck = TRUE;
		}

		//다리
		if((P_LegCheck == TRUE) && (P_Leg <= PlayAngle))
		{
			P_Leg += 2;
			if(P_Leg > PlayAngle)
				P_LegCheck = FALSE;
		}
		else if((P_LegCheck == FALSE))
		{
			P_Leg -= 2;
			if(P_Leg <= -PlayAngle)
				P_LegCheck = TRUE;
		}

		//머리 + 손
		if((P_Head < 0.1) && (P_HeadCheck == TRUE))
		{	
			P_Head += 0.02;
			if(P_Head >= 0.1)
				P_HeadCheck = FALSE;
		}
		else if (P_HeadCheck == FALSE)
		{
			P_Head -= 0.02;
			if(P_Head < -0.1)
				P_HeadCheck = TRUE;
		}
	
	}

	//플레이어의 무기 애니메이션 체크
	//C버튼이 눌렸을 경우만 동작
	if((Click == TRUE) && (StickCount != 50))
	{
		//플레이어의 팔을 이동
		if((StickArmCheck == TRUE) && (StickArm <= 0.4))
		{
			StickArm += 0.05;
			if(StickArm > 0.4)
				StickArmCheck = FALSE;
		}
		else if(StickArmCheck  == FALSE)
		{
			StickArm -= 0.05;
			if(StickArm < -0.4)
				StickArmCheck = TRUE;
		}

		//플레이어 팔의 각도 회전
		if((StickCheck == TRUE) && (Stick <= 80))
		{	
			Stick += 10;
			
			if(Stick > 80)
				StickCheck = FALSE;
		}

		else if(StickCheck == FALSE)
		{
			Stick -= 10;
			if(Stick < -60)
				StickCheck = TRUE;
		}
		StickCount++;
	}
	else 
		Click = FALSE;

		StopCheck = TRUE;
		glutPostRedisplay();
		glutTimerFunc(50, PlayerTimer, 1);
}

//맵 구현함수
void drawMaze()
{
	arr3_amount=0;//몬스터, 아이템 위치 새로받기                         ---------------추가
    arr4_amount=0;
    arr5_amount=0;
	arr6_amount=0;
	int x, z;
    gluQuadricTexture(BoxImage, GL_TRUE);
   for (x = 0; x < MAZE_SIZE; x++){
	   for (z = 0; z < MAZE_SIZE; z++){
		   if (maze[z][x] == 1)//1일 경우 벽을 그린다
		   {
		      glPushMatrix();
			  glTranslated(x * wallSize, 0, z * wallSize);
			  glEnable(GL_TEXTURE_2D);
			  MakeCube(wallSize*0.5, 4);
			  glDisable(GL_TEXTURE_2D);
		      glPopMatrix();
			}
		   else if(maze[z][x] == 2)//2일경우 2층으로 올라가는 계단모양
		   {
				 glPushMatrix();	 
				 glTranslated(x * wallSize, -wallSize / 2.0, z * wallSize+5.5);
				 glRotated(45,1,0,0);
				 glEnable(GL_TEXTURE_2D);
				 MakeCube(wallSize*(1.5/2), 5);
				 glDisable(GL_TEXTURE_2D);
				 glPopMatrix();
		   }
		   else if(maze[z][x] == 3)//아이템구현
		   {
				 glPushMatrix();	 
				 glTranslated(x * wallSize,-4, z * wallSize);
				 glRotated(90,1,0,0);
				 glRotated((GLfloat)((Time*1)%360),0,0,1);
					 glEnable(GL_TEXTURE_2D);
					 MakeCube(0.4, 1);
					 glDisable(GL_TEXTURE_2D);
				 glPopMatrix();

				 for(int j=0; j<6; j++)//보물의 위치정보를 입력받는다
				 {
					 switch(j)
					 {
					 case 0:
						 arr3_position[arr3_amount][j] = z;
						 break;
					 case 1:
						  arr3_position[arr3_amount][j] = x;
						 break;
					 case 2:
						  arr3_position[arr3_amount][j] = x*wallSize;
						 break;
					 case 3:
						  arr3_position[arr3_amount][j] = -4;
						 break;
					 case 4:
						  arr3_position[arr3_amount][j] = z*wallSize;
						 break;
					 }
				 }
				 arr3_amount++;
		   }
		   else if(maze[z][x] == 4)//4인 몬스터 그리는 부분
		   {
			   glPushMatrix();
				glTranslated(x * wallSize,-4, z * wallSize);
				glScalef(4.0, 4.0, 4.0);			   
				glTranslatef(0.0,0.0,M_Motion);
				Monster();
			   glPopMatrix();
			   for(int j=0; j<6; j++)//4인 몬스터의 위치정보를 받는다.
				 {
					 switch(j)
					 {
					 case 0:
						 arr4_position[arr4_amount][j] = z;
						 break;
					 case 1:
						  arr4_position[arr4_amount][j] = x;
						 break;
					 case 2:
						  arr4_position[arr4_amount][j] = x*wallSize;
						 break;
					 case 3:
						  arr4_position[arr4_amount][j] = -4;
						 break;
					 case 4:
						  arr4_position[arr4_amount][j] = z*wallSize+M_Motion;
						 break;
					 }
				 }
			     arr4_amount++;//4인 몬스터의 개체수
		   }
		    else if(maze[z][x] == 5)//5인 몬스터 그리기
		   {
			   glPushMatrix();
				glTranslated(x * wallSize,-4, z * wallSize);
				glScalef(4.0, 4.0, 4.0);		   
				glTranslatef(M_Motion,0.0,0.0);
				glRotatef(-90, 0.0, 1.0, 0.0);
				Monster();
			   glPopMatrix();

			   for(int j=0; j<6; j++)//5인 몬스터의 위치정보를 받는다.
				 {
					 switch(j)
					 {
					 case 0:
						 arr5_position[arr5_amount][j] = z;
						 break;
					 case 1:
						  arr5_position[arr5_amount][j] = x;
						 break;
					 case 2:
						  arr5_position[arr5_amount][j] = x*wallSize+M_Motion;
						 break;
					 case 3:
						  arr5_position[arr5_amount][j] = -4;
						 break;
					 case 4:
						  arr5_position[arr5_amount][j] = z*wallSize;
						 break;
					 }
				 }
			   arr5_amount++;
		   }
	   }
   }
   
   
   for (x = 0; x < MAZE_SIZE; x++){//2층 맵 그리는 부분
	   for (z = 0; z < MAZE_SIZE; z++){
		   if (maze_2[z][x] == 1)
			   {
				  glPushMatrix();
				  glTranslated(x * wallSize, 10.5, z * wallSize+100);
				  glEnable(GL_TEXTURE_2D);
				  MakeCube(wallSize*0.5, 4);
				 glDisable(GL_TEXTURE_2D);
				  glPopMatrix();
				}
		   if (maze_2[z][x] == 6)
			   {
				  glPushMatrix();
				  glTranslated(x * wallSize, 6.5, z * wallSize+100);
				  glutSolidSphere(1,10,10);
				  //glEnable(GL_TEXTURE_2D);
				  //MakeCube(wallSize*0.5, 4);
				  //glDisable(GL_TEXTURE_2D);
				  glPopMatrix();
				   for(int j=0; j<6; j++)//5인 몬스터의 위치정보를 받는다.
				 {
					 switch(j)
					 {
					 case 0:
						 arr6_position[arr6_amount][j] = z;
						 break;
					 case 1:
						  arr6_position[arr6_amount][j] = x;
						 break;
					 case 2:
						  arr6_position[arr6_amount][j] = x*wallSize;
						 break;
					 case 3:
						  arr6_position[arr6_amount][j] = 10.5;
						 break;
					 case 4:
						  arr6_position[arr6_amount][j] = z*wallSize+100;
						 break;
					 }
				 }
			   arr6_amount++;
				}
	   }
   }
   
   //바닥 그리기
	drawFloor(wallSize, 5);
}

//충돌 체크 함수
double crash(float x, float y, float z){//위치좌표와의 계산을 통하여 거리를 알아낸다
	double distance,distance_result;
	distance = (x-copx)*(x-copx) + (y-copy)*(y-copy) + (z-copz)*(z-copz);
	distance_result = sqrt(distance);
	return distance_result;
}
//공격을 했을시 성공을 판단하는 함수
void attack(){
	for(int i =0; i<10; i++){//보물일 경우
		int z_position = arr3_position[i][0];
		int x_position = arr3_position[i][1];
		if(maze[z_position][x_position] == 3 && crash(arr3_position[i][2],arr3_position[i][3],arr3_position[i][4])<7.5){
			maze[z_position][x_position] = 0;
			printf("없어져라!!!");
		}
	}
	for(int i =0; i<10; i++){//몬스터 4일 경우
		int z_position = arr4_position[i][0];
		int x_position = arr4_position[i][1];
		if(maze[z_position][x_position] == 4 && crash(arr4_position[i][2],arr4_position[i][3],arr4_position[i][4])<7.5){
			maze[z_position][x_position] = 0;
			printf("없어져라!!!");
		}	
	}
	for(int i =0; i<10; i++){//몬스터 5일 경우
		int z_position = arr5_position[i][0];
		int x_position = arr5_position[i][1];
		if(maze[z_position][x_position] == 5 && crash(arr5_position[i][2],arr5_position[i][3],arr5_position[i][4])<7.5){
			maze[z_position][x_position] = 0;
			printf("없어져라!!!");
		}	
	}

}
//몬스터나 보물과 충돌을 체크하는 함수
void crash_check(){
	for(int i =0; i<10; i++){
		int z_position = arr3_position[i][0];
		int x_position = arr3_position[i][1];
		if(maze[z_position][x_position] == 3 && crash(arr3_position[i][2],arr3_position[i][3],arr3_position[i][4])<6)
			printf("충돌!!!!\n");
	}
	for(int i =0; i<10; i++){
		int z_position = arr4_position[i][0];
		int x_position = arr4_position[i][1];
		if(maze[z_position][x_position] == 4 && crash(arr4_position[i][2],arr4_position[i][3],arr4_position[i][4])<6)
			printf("충돌!!!!\n");
	}
	for(int i =0; i<10; i++){
		int z_position = arr5_position[i][0];
		int x_position = arr5_position[i][1];
		if(maze[z_position][x_position] == 5 && crash(arr5_position[i][2],arr5_position[i][3],arr5_position[i][4])<6)
			printf("충돌!!!!\n");
	}
	
	for(int i =0; i<10; i++){
		int z_position = arr6_position[i][0];
		int x_position = arr6_position[i][1];
		if(maze_2[z_position][x_position] == 6 && crash(arr6_position[i][2],arr6_position[i][3],arr6_position[i][4])<6)
			printf("보물을 찾았습니다.\n");
	}
}

//모든 화면 출력
void AllDisplay()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();

   //조명 설정 및 켜기 
   GLfloat lightPosition[4] = { 0.0, 5.0, 0.0, 1.0 };
   GLfloat ambient[4] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat spotlightDrirection[]={0.0,-1.0,0.0};
   GLfloat cutoff[]={90.0};

   glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
   glLightfv(GL_LIGHT0, GL_SPECULAR, ambient);
   glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION,spotlightDrirection);
   glLightfv(GL_LIGHT0, GL_SPOT_CUTOFF,cutoff);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);

   glPushMatrix();//카메라로부터 변환된 곳에 플레어 배치
		glTranslated(0, -2.0, -5);
		glRotatef(30, 1.0, 0.0, 0.0);
   		glRotatef(180, 0.0, 1.0, 0.0);
		Player();
   glPopMatrix();
   glRotated(30, 1, 0, 0);//시점 변환을 위한 변수지정
   glRotated(-horangle, 0, 1, 0);//가로축 시점변환을 위한 변수 지정
   glTranslated(-copx, -copy, -copz);//카메라 시점이동
 
   drawMaze();//맵 그리기
   
   /* white walls, only lit by GL_LIGHT0 */
   GLfloat rgb[4] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat low_shininess[] = { 100.0 }; //저광택-> 난반사
   glMaterialfv(GL_FRONT, GL_SPECULAR, rgb);
   glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);

   glDisable(GL_LIGHT0);
   glutSwapBuffers();
}

void reshape(int width, int height)
{
   //새로운 뷰포트 변환
   screenWidth = width;
   screenHeight = height;
   glViewport(0, 0, width, height);
   
   //새로운 projection으로 시점이동
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(60, screenWidth / screenHeight, .1, 200);
   glMatrixMode(GL_MODELVIEW);
}

// w=전진, a=좌회전, d=우회전, s=후진, c= 타격
void MyKeyboard(unsigned char key, int x, int u){
   double cspeed = SPEED * (cos((horangle / 180)*PI));
   double sspeed = SPEED * (sin((horangle / 180)*PI));
   crash_check();

	switch(key){
		case 'W':
		case 'w':
			 {
			   StopCheck = FALSE;
			   if (maze[(int)(copz - cspeed + 5) / 10][(int)(copx - sspeed + 5) / 10] == 1 )//벽일경우
				   printf("wall");
			   else if(maze[(int)(copz - cspeed + 5) / 10][(int)(copx - sspeed + 5) / 10] == 2)//계단일 경우
			   {
					copz -= cspeed;
					copx -= sspeed;
					copy -= cspeed*1.05;
					
					if(cspeed>0)
					verangle = -60;
					else
					verangle = -0;
			   }
			   else		//아무것도 아닐경우
			   {
				   copz -= cspeed;
				   copx -= sspeed;
				  verangle = -30;
			   }
			  break;
			 }
		case 'A':
		case 'a':
		   {
				StopCheck = FALSE;
			  if (maze[(int)(copz + sspeed + 5) / 10][(int)(copx - cspeed + 5) / 10] == 1)
				  printf("Wall!\n");
			  else if(maze[(int)(copz + sspeed + 5) / 10][(int)(copx - cspeed + 5) / 10] == 2){
					 copx -= cspeed;
					copz += sspeed;
			  }
			  else
			  {
					copx -= cspeed;
					copz += sspeed;
			  }
			  break;
		   }
		case 'S':
		case 's':
		   {
				StopCheck = FALSE;
			  if (maze[(int)(copz + cspeed + 5 ) / 10][(int)(copx + sspeed + 5) / 10] == 1)
				printf("Wall\n");
			  else if(maze[(int)(copz + cspeed + 5) / 10][(int)(copx + sspeed + 5) / 10] == 2){
				 copz += cspeed;
				 copx += sspeed; 
				 copy += cspeed*1.05;
			  }else
			  {
				 copz += cspeed;
				 copx += sspeed;
			  }
			  break;
		   }
		case 'D':
		case 'd':
		   {
				StopCheck = FALSE;
			  if (maze[(int)(copz - sspeed + 5) / 10][(int)(copx + cspeed + 5) / 10] == 1)
				 printf("Wall\n");
			  else if(maze[(int)(copz - sspeed + 5) / 10][(int)(copx + cspeed + 5) / 10] == 2){
				  copx += cspeed;
				  copz -= sspeed;
			  }
			  else 
			  {
				  copx += cspeed;
				  copz -= sspeed;
			  }
			  break;
		   }
		case 'o':
		case 'O':
			{
				exit(0);
			}
		case 'c':
		case 'C':
			{
				Click = TRUE;
				StickCount = 0;
				attack();
				break;
			}
		 default: break;
	}
}

//처음 시작화면 출력
void MenuDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(MenuCount == 1)
	{
	 glEnable( GL_TEXTURE_2D );    // Enable texture mapping
	 glDisable(GL_DEPTH_TEST);
	//출력 이미지
	 glPushMatrix();
	  glBlendFunc(GL_ONE, GL_ONE);
	  glBindTexture(GL_TEXTURE_2D, tex[2]);
	  glBegin(GL_QUADS); //1, 2, 3, 4는 이미지의  좌상단, 우상단, 좌하단, 우하단 순
	  glTexCoord2f(0.0f, 0.0f); glVertex3f(-1, -1, 0); //3
	  glTexCoord2f(1.0f, 0.0f); glVertex3f(1, -1, 0); //4
	  glTexCoord2f(1.0f, 1.0f); glVertex3f(1, 1, 0); //2
	  glTexCoord2f(0.0f, 1.0f); glVertex3f(-1, 1, 0); //1
	  glEnd();
	  glPopMatrix();
	  glDisable( GL_TEXTURE_2D );
	}
	
  glutSwapBuffers();
}

void GuideDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(MenuCount == 2)
	{
	 glEnable( GL_TEXTURE_2D );    // Enable texture mapping
	 glDisable(GL_DEPTH_TEST);
	//출력 이미지
	 glPushMatrix();
	  glBlendFunc(GL_ONE, GL_ONE);
	  glBindTexture(GL_TEXTURE_2D, tex[6]);
	  glBegin(GL_QUADS); //1, 2, 3, 4는 이미지의  좌상단, 우상단, 좌하단, 우하단 순
	  glTexCoord2f(0.0f, 0.0f); glVertex3f(-1, -1, 0); //3
	  glTexCoord2f(1.0f, 0.0f); glVertex3f(1, -1, 0); //4
	  glTexCoord2f(1.0f, 1.0f); glVertex3f(1, 1, 0); //2
	  glTexCoord2f(0.0f, 1.0f); glVertex3f(-1, 1, 0); //1
	  glEnd();
	  glPopMatrix();
	  glDisable( GL_TEXTURE_2D );
	}
	glutSwapBuffers();
}

//마우스로 인한 시점변하는 함수
void mouse(int btn, int state, int x, int y)
{
   if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)//왼쪽버튼을 누를경우
   {
      lastX = x;
      //lastY = y;
      mousedown = true;

   }
   else if (btn == GLUT_LEFT_BUTTON) 
   {
	   mousedown = false;
   }
   //glutPostRedisplay();
}

//아이템들이 회전하기 위한 함수
void MyTimer(int value){
		Time=(Time+13)%360;
		glutPostRedisplay();
		glutTimerFunc(50,MyTimer,1);
}

//마우스 회전에 대한 각도조정
void motion(int ex, int ey)
{
   horangle -= 0.5* (ex - lastX); // tang K = 0.1 x K
   lastX = ex;

  // verangle -= 0.5* (ey - lastY);
   //lastY = ey;

   glutPostRedisplay();
}

void gamePageInit()
{
	//viewport 변환 부분
    glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(60, screenWidth / screenHeight, .1, 200);
   glMatrixMode(GL_MODELVIEW);

   //후면제거
   glCullFace(GL_BACK);
   glEnable(GL_CULL_FACE);

   //오브젝트들의 깊이를 표시하여 뒤에있는 것은 안보임
   glEnable(GL_DEPTH_TEST);

   /* use smooth shading */
   glShadeModel(GL_SMOOTH);

   /* initialise call back functions */
   glutKeyboardFunc(MyKeyboard);
   glutMouseFunc(mouse);
   glutMotionFunc(motion);
   glutDisplayFunc(AllDisplay);
   glutReshapeFunc(reshape);
   glutTimerFunc(50, MonsterTimer, 1);
   glutTimerFunc(50, PlayerTimer, 1);
   glutTimerFunc(50, MyTimer,1);
}

void MenuKeyboard(unsigned char key, int x, int u){

	switch(key){
	case 'P':
	case 'p':
		{
			MenuCount++;

			if(MenuCount == 2)
				glutDisplayFunc(GuideDisplay);
			
			if(MenuCount == 3 && MenuCheck == FALSE)
			{
				MenuCheck = TRUE;
				printf("Load gamePageInit()");
				gamePageInit();
			}

		}
		 default: break;
	}
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
   /* initialise variables */
   printf(" Treasure Hunter \n");
   worldSize = WORLD_SIZE;//전체 사이즈
   wallSize = worldSize / MAZE_SIZE;//벽하나의 사이즈/
   screenWidth = 800;
   screenHeight = 800;

   //시작지점 좌표 지정
   copx = 8.0;
   copy = 0.0;
   copz = -11.0;
   
   //시작시점 방향 지정
   horangle = 180.0;
   verangle = -50.0;

   glutInit(&argc, argv);
   glutInitWindowPosition(500, 100);
   glutInitWindowSize(screenWidth, screenHeight);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutCreateWindow("Huns");
   glClearColor(1.0, 1.0, 1.0, 1.0);
 
	LoadGLtex();

   if(MenuCheck == FALSE)
   {
	    glutKeyboardFunc(MenuKeyboard);
		glutDisplayFunc(MenuDisplay);
   }

   /* loop forever */
   glutMainLoop();
   return 0;
}