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

/*ĳ���Ϳ� ���� �ִϸ��̼ǿ� ���Ǵ� ����*/
GLfloat M_Arm = 0, M_Leg = 0, M_Motion = 0; 
GLboolean M_ArmCheck = TRUE, M_LegCheck = TRUE, M_MoCheck =TRUE; 
GLfloat P_Arm = 0, P_Leg = 0, P_Head = 0;
GLboolean P_ArmCheck = TRUE, P_LegCheck = TRUE, P_HeadCheck = TRUE;
GLboolean StopCheck = TRUE;  //ĳ���Ͱ� �̵��� �� ���� �����̵���

/*ĳ���� ���� �ִϸ��̼�*/
GLboolean Click = FALSE , StickCheck = TRUE, StickArmCheck = TRUE;
GLfloat Stick = 0, StickArm = 0;
GLint StickCount = 0; 
//StickCount�� 50�� ���� �ʾ����� ���͸� ���� �� �ִ�.
//�� �̿ܿ��� �÷��̾��� ����� ���δ�.

GLboolean MenuCheck = FALSE;
GLint MenuCount = 1;

GLuint tex[texize];   // Texture Mapping�� �ϱ� ���� Texture �̹����� ������ ���� �迭 ����
GLUquadricObj *PlayerFace = gluNewQuadric();
GLUquadricObj *BoxImage = gluNewQuadric();

/*�̷� ���� (���� 2��)*/
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


//������ ���� ���� ��������� �˱� ���� ����
int arr3_amount=0;
int arr4_amount=0;
int arr5_amount=0;
int arr6_amount=0;
//���Ϳ� ������ ��ġ������ �����ϰ� �ִ� �迭
double arr3_position[10][6] = {0,};
double arr4_position[10][6] = {0,};
double arr5_position[10][6] = {0,};
double arr6_position[10][6] = {0,};

AUX_RGBImageRec *LoadBMP(char *Filename) {  // Bitmap �̹����� ȣ���Ѵ�.
     FILE *File=NULL;

     if (!Filename) return NULL;
     File=fopen(Filename,"r");
     if (File) {
          fclose(File);
          return auxDIBImageLoad(Filename);
     }

     return NULL;
}

void LoadGLtex( ) {  // Bitmap �̹��� ȣ���Ͽ� Texture �̹����� ��ȯ�Ѵ�.
     AUX_RGBImageRec *texRec[texize];
     memset(texRec, 0, sizeof(void *)*texize);                                                  

	 //��� ��� ����
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


//�� �����
void MakeCube(double wallSize, int texIndex)
{
	//�ո�
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

	 //�޸�
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

	 //����
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

	 //�Ʒ���
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

	 //�޸�
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

	 //������
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

//�ٴڸ����
void drawFloor(double wallSize, int texIndex)
{
   //1��
	glEnable( GL_TEXTURE_2D );  
	//��� �̹���
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
   //2��
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



//���� �׸���
void Monster()
{	
	glColor3f(1.0,1.0,1.0);
	glPushMatrix();
		//���� ��ü
		glRotatef(180, 0.0, 1.0, 0.0);
		glRotatef(-100, 1.0, 0.0, 0.0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, tex[3]);
			gluSphere(PlayerFace, 0.23, 30, 30);
			glDisable(GL_TEXTURE_2D);
		glRotatef(100, 1.0, 0.0, 0.0); //��������
		glRotatef(-180, 0.0, 1.0, 0.0);

		glRotatef(M_Arm,1.0,0.0,1.0); //�ڵ�ȸ��
		glTranslatef(0.36,0.0,0.0);
			//���� �� right
			glPushMatrix();
			glScalef(1.0,0.2,0.2);
			glutSolidCube(0.2);
			glPopMatrix();
		glTranslatef(-0.36,0.0,0.0); //�������� 
		glRotatef(-M_Arm,1.0,0.0,1.0); //�ڵ�ȸ�� ����

		glRotatef(M_Arm,1.0,0.0,1.0); //�ڵ�ȸ��
		glTranslatef(-0.36,0.0,0.0);
			//���� �� left
			glPushMatrix();
			glScalef(1.0,0.2,0.2);
			glutSolidCube(0.2);
			glPopMatrix();
		glTranslatef(0.36,0.0,0.0); //��������
		glRotatef(-M_Arm,1.0,0.0,1.0); //�ڵ�ȸ�� ����

		glRotatef(M_Leg,1.0,0.0,0.0); //�ڵ�ȸ��
		glTranslatef(0.1,-0.28,0.0);
			//�����ٸ� right
			glPushMatrix();
			glScalef(0.2,0.7,0.2);
			glutSolidCube(0.2);
			glPopMatrix();
		glTranslatef(-0.1,0.28,0.0); //��������
		glRotatef(-M_Leg,1.0,0.0,0.0);

		glRotatef(-M_Leg,1.0,0.0,0.0);//�ڵ�ȸ��
		glTranslatef(-0.1,-0.28,0.0);
			//�����ٸ� left
			glPushMatrix();
			glScalef(0.2,0.7,0.2);
			glutSolidCube(0.2);
			glPopMatrix();
	glPopMatrix();
}

//�÷��̾�
void Player()
{
	gluQuadricTexture(PlayerFace, GL_TRUE);
	glPushMatrix();
		glScalef(1.0, 1.0, 1.0);
		//���� 
		glPushMatrix();
			glColor3f(1.0, 0.4,0.1);
			glScalef(0.4, 1.3, 0.4);
			glutSolidCube(0.5);
		glPopMatrix();

		//������ ��
		glPushMatrix();
			// //�ڵ�ȸ��
			glRotatef(-30, 1.0, 0.0, 1.0);
			glRotatef(P_Arm,1.0,0.0,1.0);
			glTranslatef(0.12, 0.3, 0.1);
			glColor3f(1.0, 0.4, 0.1);
			glScalef(1.0, 0.2, 0.2);
			glutSolidCube(0.3);
			//������ ��
			glPushMatrix();
				glColor3f(1.0, 0.95, 0.8);
				glTranslatef(0.25, -0.1, 0.0);
				glTranslatef(0.0, P_Head+0.1, 0.0);
				glScalef(0.2, 1.0, 1.0);
				glutSolidSphere(0.3, 30, 30);
			glPopMatrix();
		glPopMatrix();

		if(Click == TRUE) //������ �ִϸ��̼�
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
					//���� ��
				glPushMatrix();
					glColor3f(1.0, 0.95, 0.8);
					glTranslatef(0.0, P_Head+0.1, 0.0);
					glTranslatef(-0.25, -0.1, 0.0);
					glScalef(0.2, 1.0, 1.0);
					glutSolidSphere(0.3, 30, 30);
					//������
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
				//���� ��
			glPushMatrix();
				glColor3f(1.0, 0.95, 0.8);
				glTranslatef(0.0, P_Head+0.1, 0.0);
				glTranslatef(-0.25, -0.1, 0.0);
				glScalef(0.2, 1.0, 1.0);
				glutSolidSphere(0.3, 30, 30);
				//������
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
		//������ �ٸ�
		glPushMatrix();
			glColor3f(1.0, 0.95, 0.8);
			glTranslatef(0.06, -0.55, 0.0);
			glScalef(0.25,1.5,0.25);
			glutSolidCube(0.3);
			//������ ��
			glPushMatrix();
				glTranslatef(0.0, -0.15, 0.0);
				glScalef(3.0, 0.25, 3.0);
				glColor3f(1.0, 0.4, 0.1);
				glutSolidCube(0.1);
			glPopMatrix();
		glPopMatrix();
		glRotatef(-P_Leg,1.0,0.0,0.0);
		
		glRotatef(-P_Leg,1.0,0.0,0.0);
		//���� �ٸ�
		glPushMatrix();
			glColor3f(1.0, 0.95, 0.8);
			glTranslatef(-0.06, -0.55, 0.0);
			glScalef(0.25,1.5,0.25);
			glutSolidCube(0.3);
			//���� ��
			glPushMatrix();
				glTranslatef(0.0, -0.15, 0.0);
				glScalef(3.0, 0.25, 3.0);
				glColor3f(1.0, 0.4, 0.1);
				glutSolidCube(0.1);
			glPopMatrix();
		glPopMatrix();
		glRotatef(P_Leg,1.0,0.0,0.0);
		//�Ӹ�
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

	//�� �����̴� �ִϸ��̼�
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

	//�ٸ� �����̴� �ִϸ��̼�
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

	//���� �����̴� �ִϸ��̼�
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
	
	//�÷��̾��� �������� ��Ʈ��
	//TRUE : ����ڰ� Ű���� ����Ű�� ������ ���� ������ ����
	//FALSE: �÷��̾� �ִϸ޴ϼ� �۵�
	if(StopCheck == FALSE)
	{
		//��
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

		//�ٸ�
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

		//�Ӹ� + ��
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

	//�÷��̾��� ���� �ִϸ��̼� üũ
	//C��ư�� ������ ��츸 ����
	if((Click == TRUE) && (StickCount != 50))
	{
		//�÷��̾��� ���� �̵�
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

		//�÷��̾� ���� ���� ȸ��
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

//�� �����Լ�
void drawMaze()
{
	arr3_amount=0;//����, ������ ��ġ ���ιޱ�                         ---------------�߰�
    arr4_amount=0;
    arr5_amount=0;
	arr6_amount=0;
	int x, z;
    gluQuadricTexture(BoxImage, GL_TRUE);
   for (x = 0; x < MAZE_SIZE; x++){
	   for (z = 0; z < MAZE_SIZE; z++){
		   if (maze[z][x] == 1)//1�� ��� ���� �׸���
		   {
		      glPushMatrix();
			  glTranslated(x * wallSize, 0, z * wallSize);
			  glEnable(GL_TEXTURE_2D);
			  MakeCube(wallSize*0.5, 4);
			  glDisable(GL_TEXTURE_2D);
		      glPopMatrix();
			}
		   else if(maze[z][x] == 2)//2�ϰ�� 2������ �ö󰡴� ��ܸ��
		   {
				 glPushMatrix();	 
				 glTranslated(x * wallSize, -wallSize / 2.0, z * wallSize+5.5);
				 glRotated(45,1,0,0);
				 glEnable(GL_TEXTURE_2D);
				 MakeCube(wallSize*(1.5/2), 5);
				 glDisable(GL_TEXTURE_2D);
				 glPopMatrix();
		   }
		   else if(maze[z][x] == 3)//�����۱���
		   {
				 glPushMatrix();	 
				 glTranslated(x * wallSize,-4, z * wallSize);
				 glRotated(90,1,0,0);
				 glRotated((GLfloat)((Time*1)%360),0,0,1);
					 glEnable(GL_TEXTURE_2D);
					 MakeCube(0.4, 1);
					 glDisable(GL_TEXTURE_2D);
				 glPopMatrix();

				 for(int j=0; j<6; j++)//������ ��ġ������ �Է¹޴´�
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
		   else if(maze[z][x] == 4)//4�� ���� �׸��� �κ�
		   {
			   glPushMatrix();
				glTranslated(x * wallSize,-4, z * wallSize);
				glScalef(4.0, 4.0, 4.0);			   
				glTranslatef(0.0,0.0,M_Motion);
				Monster();
			   glPopMatrix();
			   for(int j=0; j<6; j++)//4�� ������ ��ġ������ �޴´�.
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
			     arr4_amount++;//4�� ������ ��ü��
		   }
		    else if(maze[z][x] == 5)//5�� ���� �׸���
		   {
			   glPushMatrix();
				glTranslated(x * wallSize,-4, z * wallSize);
				glScalef(4.0, 4.0, 4.0);		   
				glTranslatef(M_Motion,0.0,0.0);
				glRotatef(-90, 0.0, 1.0, 0.0);
				Monster();
			   glPopMatrix();

			   for(int j=0; j<6; j++)//5�� ������ ��ġ������ �޴´�.
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
   
   
   for (x = 0; x < MAZE_SIZE; x++){//2�� �� �׸��� �κ�
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
				   for(int j=0; j<6; j++)//5�� ������ ��ġ������ �޴´�.
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
   
   //�ٴ� �׸���
	drawFloor(wallSize, 5);
}

//�浹 üũ �Լ�
double crash(float x, float y, float z){//��ġ��ǥ���� ����� ���Ͽ� �Ÿ��� �˾Ƴ���
	double distance,distance_result;
	distance = (x-copx)*(x-copx) + (y-copy)*(y-copy) + (z-copz)*(z-copz);
	distance_result = sqrt(distance);
	return distance_result;
}
//������ ������ ������ �Ǵ��ϴ� �Լ�
void attack(){
	for(int i =0; i<10; i++){//������ ���
		int z_position = arr3_position[i][0];
		int x_position = arr3_position[i][1];
		if(maze[z_position][x_position] == 3 && crash(arr3_position[i][2],arr3_position[i][3],arr3_position[i][4])<7.5){
			maze[z_position][x_position] = 0;
			printf("��������!!!");
		}
	}
	for(int i =0; i<10; i++){//���� 4�� ���
		int z_position = arr4_position[i][0];
		int x_position = arr4_position[i][1];
		if(maze[z_position][x_position] == 4 && crash(arr4_position[i][2],arr4_position[i][3],arr4_position[i][4])<7.5){
			maze[z_position][x_position] = 0;
			printf("��������!!!");
		}	
	}
	for(int i =0; i<10; i++){//���� 5�� ���
		int z_position = arr5_position[i][0];
		int x_position = arr5_position[i][1];
		if(maze[z_position][x_position] == 5 && crash(arr5_position[i][2],arr5_position[i][3],arr5_position[i][4])<7.5){
			maze[z_position][x_position] = 0;
			printf("��������!!!");
		}	
	}

}
//���ͳ� ������ �浹�� üũ�ϴ� �Լ�
void crash_check(){
	for(int i =0; i<10; i++){
		int z_position = arr3_position[i][0];
		int x_position = arr3_position[i][1];
		if(maze[z_position][x_position] == 3 && crash(arr3_position[i][2],arr3_position[i][3],arr3_position[i][4])<6)
			printf("�浹!!!!\n");
	}
	for(int i =0; i<10; i++){
		int z_position = arr4_position[i][0];
		int x_position = arr4_position[i][1];
		if(maze[z_position][x_position] == 4 && crash(arr4_position[i][2],arr4_position[i][3],arr4_position[i][4])<6)
			printf("�浹!!!!\n");
	}
	for(int i =0; i<10; i++){
		int z_position = arr5_position[i][0];
		int x_position = arr5_position[i][1];
		if(maze[z_position][x_position] == 5 && crash(arr5_position[i][2],arr5_position[i][3],arr5_position[i][4])<6)
			printf("�浹!!!!\n");
	}
	
	for(int i =0; i<10; i++){
		int z_position = arr6_position[i][0];
		int x_position = arr6_position[i][1];
		if(maze_2[z_position][x_position] == 6 && crash(arr6_position[i][2],arr6_position[i][3],arr6_position[i][4])<6)
			printf("������ ã�ҽ��ϴ�.\n");
	}
}

//��� ȭ�� ���
void AllDisplay()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();

   //���� ���� �� �ѱ� 
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

   glPushMatrix();//ī�޶�κ��� ��ȯ�� ���� �÷��� ��ġ
		glTranslated(0, -2.0, -5);
		glRotatef(30, 1.0, 0.0, 0.0);
   		glRotatef(180, 0.0, 1.0, 0.0);
		Player();
   glPopMatrix();
   glRotated(30, 1, 0, 0);//���� ��ȯ�� ���� ��������
   glRotated(-horangle, 0, 1, 0);//������ ������ȯ�� ���� ���� ����
   glTranslated(-copx, -copy, -copz);//ī�޶� �����̵�
 
   drawMaze();//�� �׸���
   
   /* white walls, only lit by GL_LIGHT0 */
   GLfloat rgb[4] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat low_shininess[] = { 100.0 }; //������-> ���ݻ�
   glMaterialfv(GL_FRONT, GL_SPECULAR, rgb);
   glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);

   glDisable(GL_LIGHT0);
   glutSwapBuffers();
}

void reshape(int width, int height)
{
   //���ο� ����Ʈ ��ȯ
   screenWidth = width;
   screenHeight = height;
   glViewport(0, 0, width, height);
   
   //���ο� projection���� �����̵�
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(60, screenWidth / screenHeight, .1, 200);
   glMatrixMode(GL_MODELVIEW);
}

// w=����, a=��ȸ��, d=��ȸ��, s=����, c= Ÿ��
void MyKeyboard(unsigned char key, int x, int u){
   double cspeed = SPEED * (cos((horangle / 180)*PI));
   double sspeed = SPEED * (sin((horangle / 180)*PI));
   crash_check();

	switch(key){
		case 'W':
		case 'w':
			 {
			   StopCheck = FALSE;
			   if (maze[(int)(copz - cspeed + 5) / 10][(int)(copx - sspeed + 5) / 10] == 1 )//���ϰ��
				   printf("wall");
			   else if(maze[(int)(copz - cspeed + 5) / 10][(int)(copx - sspeed + 5) / 10] == 2)//����� ���
			   {
					copz -= cspeed;
					copx -= sspeed;
					copy -= cspeed*1.05;
					
					if(cspeed>0)
					verangle = -60;
					else
					verangle = -0;
			   }
			   else		//�ƹ��͵� �ƴҰ��
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

//ó�� ����ȭ�� ���
void MenuDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(MenuCount == 1)
	{
	 glEnable( GL_TEXTURE_2D );    // Enable texture mapping
	 glDisable(GL_DEPTH_TEST);
	//��� �̹���
	 glPushMatrix();
	  glBlendFunc(GL_ONE, GL_ONE);
	  glBindTexture(GL_TEXTURE_2D, tex[2]);
	  glBegin(GL_QUADS); //1, 2, 3, 4�� �̹�����  �»��, ����, ���ϴ�, ���ϴ� ��
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
	//��� �̹���
	 glPushMatrix();
	  glBlendFunc(GL_ONE, GL_ONE);
	  glBindTexture(GL_TEXTURE_2D, tex[6]);
	  glBegin(GL_QUADS); //1, 2, 3, 4�� �̹�����  �»��, ����, ���ϴ�, ���ϴ� ��
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

//���콺�� ���� �������ϴ� �Լ�
void mouse(int btn, int state, int x, int y)
{
   if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)//���ʹ�ư�� �������
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

//�����۵��� ȸ���ϱ� ���� �Լ�
void MyTimer(int value){
		Time=(Time+13)%360;
		glutPostRedisplay();
		glutTimerFunc(50,MyTimer,1);
}

//���콺 ȸ���� ���� ��������
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
	//viewport ��ȯ �κ�
    glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(60, screenWidth / screenHeight, .1, 200);
   glMatrixMode(GL_MODELVIEW);

   //�ĸ�����
   glCullFace(GL_BACK);
   glEnable(GL_CULL_FACE);

   //������Ʈ���� ���̸� ǥ���Ͽ� �ڿ��ִ� ���� �Ⱥ���
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
   worldSize = WORLD_SIZE;//��ü ������
   wallSize = worldSize / MAZE_SIZE;//���ϳ��� ������/
   screenWidth = 800;
   screenHeight = 800;

   //�������� ��ǥ ����
   copx = 8.0;
   copy = 0.0;
   copz = -11.0;
   
   //���۽��� ���� ����
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