#include "main.h"
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")


const float SCALE_BIG_BIG = 1, SCALE_BIG = 2, SCALE_MID = 4, SCALE_SMALL = 8, SCALE_SMALL_SMALL = 16;
float scale = SCALE_BIG_BIG;
float interval = 0.001;
const float mag = 1.5;
unsigned int SLOTNUMW = 576*2, SLOTNUMH = 288*2;
size_t width = SLOTNUMW*mag, height = SLOTNUMH*mag;

bool **SLOT;
void drawSlots();
bool ld = 0, rd = 0;
bool running = false;
int mainWindow;
int controler;
int frameWindow;
Button *run, *clear, *speed, *label_Speed, *label_stepCount, *save, *load, *reshape, *next;
int playDelay;
const int SPEED_FAST_FAST = 1, SPEED_FAST = 10, SPEED_MID = 50, SPEED_SLOW = 100;
long step = 0;
HWND hwnd;
int offsetX = (SLOTNUMW / 2) - (SLOTNUMW / scale / 2);
int offsetY = (SLOTNUMH / 2) - (SLOTNUMH / scale / 2);
bool reshapeMode = false;
int MouseX, MouseY;
int unitMove = 2;
long timeCount = 0;

char version[] = "Release 1.0";

void calculate();
void drawFrame();
void drawReshapeFrame();
void setScale(float scale);
void saveFile();
void openFile();
int formatOffX(int);
int formatOffY(int);

void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)
{
	timeCount++;
	glutPostWindowRedisplay(mainWindow);
	glutPostWindowRedisplay(controler);
	glutPostWindowRedisplay(run->winid);
	glutPostWindowRedisplay(label_Speed->winid);
	glutPostWindowRedisplay(label_stepCount->winid);
}

void moveY(int off)
{
	offsetY = offsetY + off;
	if (offsetY > SLOTNUMH - SLOTNUMH / scale)
	{
		offsetY = SLOTNUMH - SLOTNUMH / scale;
	}
}

void moveX(int off)
{
	offsetX = offsetX + off;
	if (offsetX > SLOTNUMW - SLOTNUMW / scale)
	{
		offsetX = SLOTNUMW - SLOTNUMW / scale;
	}
}

void keyProcess(unsigned char key, int x, int y)
{
	if (key == 27)
		exit(0);
	else if (key == 'e')
		reshapeMode = !reshapeMode;
	else if (key == 'w')
	{
		moveY(-unitMove);
	}
	else if (key == 's')
	{
		moveY(unitMove);
	}
	else if (key == 'a')
	{
		moveX(-unitMove);
	}
	else if (key == 'd')
	{
		moveX(unitMove);
	}
	else if (key == ' ')
	{
		running = !running;
	}
}

void onMousePass(int x, int y)
{
	MouseX = x;
	MouseY = y;
}

void display()
{
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glLineWidth(interval);
	if (running)
		glColor3f(0.0f, 1.0f, 0.0f);
	else
		glColor3f(1.0f, 1.0f, 0.0f);
	if (running)
		calculate();
	drawSlots();
	glColor3f(0.6, 0.6, 0.6);
	drawFrame();
	if (reshapeMode)
		drawReshapeFrame();
	glFlush();
}
void mdisplay()
{
}
void controler_display()
{
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
}
bool checkId(unsigned int i, unsigned int j)
{
	if ((i >= 0 && j >= 0) && (i < SLOTNUMW  &&j < SLOTNUMH))
	{
		return true;
	}
	else
	{
		return false;
	}
}
int getNeighborsCount(int i, int j, bool** SLOT)
{
	int c = 0;
	if (checkId(i - 1, j - 1) && SLOT[i - 1][j - 1])
	{
		c++;
	}
	if (checkId(i, j - 1) && SLOT[i][j - 1])
	{
		c++;
	}
	if (checkId(i + 1, j - 1) && SLOT[i + 1][j - 1])
	{
		c++;
	}
	if (checkId(i - 1, j) && SLOT[i - 1][j])
	{
		c++;
	}
	if (checkId(i + 1, j) && SLOT[i + 1][j])
	{
		c++;
	}
	if (checkId(i - 1, j + 1) && SLOT[i - 1][j + 1])
	{
		c++;
	}
	if (checkId(i, j + 1) && SLOT[i][j + 1])
	{
		c++;
	}
	if (checkId(i + 1, j + 1) && SLOT[i + 1][j + 1])
	{
		c++;
	}
	return c;
}
void _calc()
{
	bool **tmp = (bool**)malloc(SLOTNUMW * 4);
	for (size_t i = 0; i < SLOTNUMW; i++)
	{
		tmp[i] = (bool*)malloc(SLOTNUMH * sizeof(bool));
		for (size_t j = 0; j < SLOTNUMH; j++)
		{
			tmp[i][j] = SLOT[i][j];
		}
	}
	for (size_t i = 0; i < SLOTNUMW; i++)
	{
		for (size_t j = 0; j < SLOTNUMH; j++)
		{
			int n = getNeighborsCount(i, j, tmp);
			if ((n > 3 || n < 2) && SLOT[i][j] == true)
			{
				SLOT[i][j] = false;
			}
			else if (n == 3 && SLOT[i][j] == false)
			{
				SLOT[i][j] = true;
			}
		}
	}
	for (size_t i = 0; i < SLOTNUMW; i++)
	{
		free(tmp[i]);
	}
	free(tmp);
	step++;
}
void calculate()
{
	if (timeCount%playDelay != 0)return;
	_calc();
}
void drawReshapeFrame()
{
	unsigned int i = MouseX / mag / scale;
	unsigned int j = MouseY / mag / scale;
	glLineWidth(0.5);
	glColor3f(0, 0, 1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(i - SLOTNUMW / scale / 4, j - SLOTNUMH / scale / 4);
	glVertex2f(i - SLOTNUMW / scale / 4, j + SLOTNUMH / scale / 4);
	glVertex2f(i + SLOTNUMW / scale / 4, j + SLOTNUMH / scale / 4);
	glVertex2f(i + SLOTNUMW / scale / 4, j - SLOTNUMH / scale / 4);
	glEnd();
}
void drawFrame()
{
	for (size_t i = 0; i < SLOTNUMW / scale; i++)
	{
		glBegin(GL_LINES);
		glVertex2f(i, 0);
		glVertex2f(i, SLOTNUMH / scale);
		glEnd();
	}
	for (size_t i = 0; i < SLOTNUMH / scale; i++)
	{
		glBegin(GL_LINES);
		glVertex2f(0, i);
		glVertex2f(SLOTNUMW / scale, i);
		glEnd();
	}
}
void drawSlots()
{
	for (size_t i = 0; i < SLOTNUMW; i++)
	{
		for (size_t j = 0; j < SLOTNUMH; j++)
		{
			if (SLOT[i][j])
			{
				
				glBegin(GL_POLYGON);
				glVertex2f(i + interval - offsetX, j + interval - offsetY);
				glVertex2f(i + interval - offsetX, j + 1 - interval - offsetY);
				glVertex2f(i + 1 - interval - offsetX, j + 1 - interval - offsetY);
				glVertex2f(i + 1 - interval - offsetX, j + interval - offsetY);
				glEnd();
				/*
				glBegin(GL_POLYGON);
				glVertex2f(i + 0.001 - offsetX, j + 0.001 - offsetY);
				glVertex2f(i + 0.001 - offsetX, j + 1 - 0.001 - offsetY);
				glVertex2f(i + 1 - 0.001 - offsetX, j + 1- 0.001 - offsetY);
				glVertex2f(i + 1 - 0.001 - offsetX, j + 0.001 - offsetY);
				glEnd();
				*/
			}
		}
	}
}
void addSlot(int x, int y)
{
	if (running || reshapeMode)return;
	unsigned int i = x / mag / scale;
	unsigned int j = y / mag / scale;
	if (checkId(i, j))
		SLOT[offsetX + i][offsetY + j] = 1;
}
void delSlot(int x, int y)
{
	if (running || reshapeMode)return;
	unsigned int i = x / mag / scale;
	unsigned int j = y / mag / scale;
	if (checkId(i, j))
		SLOT[offsetX + i][offsetY + j] = 0;
}
int formatOffX(int offX)
{
	if (offX < 0)
		return 0;
	else if (offX > (int)SLOTNUMW - SLOTNUMW / scale / 2)
		return SLOTNUMW - SLOTNUMW / scale / 2;
	else
		return offX;
}
int formatOffY(int offY)
{
	if (offY < 0)
		return 0;
	else if (offY > (int)SLOTNUMH - SLOTNUMH / scale / 2)
		return SLOTNUMH - SLOTNUMH / scale / 2;
	else
		return offY;
}
void onMouseClick(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			ld = true;
			addSlot(x, y);
			if (reshapeMode&&scale < SCALE_SMALL_SMALL)
			{
				int i = x / mag / scale;
				int j = y / mag / scale;
				int offX = i - SLOTNUMW / scale / 4;
				int offY = j - SLOTNUMH / scale / 4;
				offsetX = formatOffX(offsetX + offX);
				offsetY = formatOffY(offsetY + offY);
				setScale(scale * 2);
			}
		}
		else if (button == GLUT_RIGHT_BUTTON)
		{
			rd = true;
			delSlot(x, y);
			if (reshapeMode&&scale > SCALE_BIG_BIG)
			{
				offsetX = 0;
				offsetY = 0;
				setScale(SCALE_BIG_BIG);
			}
		}
	}
	else if (state == GLUT_UP)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			ld = false;
		}
		else if (button == GLUT_RIGHT_BUTTON)
		{
			rd = false;
		}
	}
}
void controler_onMouseClick(int button, int state, int x, int y)
{
}
void onMouseDrag(int x, int y)
{
	if (ld)
		addSlot(x, y);
	else if (rd)
		delSlot(x, y);
}
void initSLOT()
{
	SLOT = (bool**)malloc(SLOTNUMW * 4);
	for (size_t i = 0; i < SLOTNUMW; i++)
	{
		SLOT[i] = (bool*)malloc(sizeof(bool)*SLOTNUMH);
		for (size_t j = 0; j < SLOTNUMH; j++)
		{
			SLOT[i][j] = 0;
		}
	}
}

void changeSize(int w, int h)
{
}
void btn1_dis()
{
	glClearColor(1, 1, 1, 0.0f);
	glColor3f(0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2f(-1, -0.5);
	if (!running)
	{
		drawCNString("开始");
	}
	else
		drawCNString("暂停");
	glFlush();
}

void btn1_clicked(int button, int state, int x, int y)
{
	if (state == GLUT_UP)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			if (!running)
			{
				running = true;
			}
			else
			{
				running = false;
			}
		}
	}
}
void btn2_dis()
{
	glClearColor(1, 1, 1, 0.0f);
	glColor3f(0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2f(-1, -0.5);
	drawCNString("速度");
	glFlush();
}


void btn2_menu(int value)
{
	if (value == 1)
	{
		playDelay = SPEED_FAST_FAST;
	}
	else if (value == 2)
	{
		playDelay = SPEED_FAST;
	}
	else if (value == 3)
	{
		playDelay = SPEED_MID;
	}
	else if (value == 4)
	{
		playDelay = SPEED_SLOW;
	}
}


void setScale(float s)
{
	//SLOT = moveSLOTS(s);
	scale = s;
	glutSetWindow(frameWindow);
	glutDestroyWindow(mainWindow);

	mainWindow = glutCreateSubWindow(frameWindow, 0, 0, width, height);
	gluOrtho2D(0, SLOTNUMW / scale, SLOTNUMH / scale, 0);
	glutMotionFunc(onMouseDrag);
	glutMouseFunc(onMouseClick);
	glutReshapeFunc(changeSize);
	glutDisplayFunc(display);
	glutPassiveMotionFunc(onMousePass);
}


void btn4_dis()
{
	glClearColor(1, 1, 1, 0.0f);
	glColor3f(0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2f(-1, -0.5);
	drawCNString("清空");
	glFlush();
}
void clearSLOTS()
{
	running = false;
	step = 0;
	for (size_t i = 0; i < SLOTNUMW; i++)
	{
		for (size_t j = 0; j < SLOTNUMH; j++)
		{
			SLOT[i][j] = 0;
		}
	}
}
void btn4_clicked(int button, int state, int x, int y)
{
	if (state == GLUT_UP)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			clearSLOTS();
		}
	}
}

void btn5_dis()
{
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
	glColor3f(0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2f(-1, -0.5);
	char txt[20];
	if (playDelay == SPEED_FAST_FAST)
	{
		strcpy(txt, "极快");
	}
	else if (playDelay == SPEED_FAST)
	{
		strcpy(txt, "快速");
	}
	else if (playDelay == SPEED_MID)
	{
		strcpy(txt, "中速");
	}
	else if (playDelay == SPEED_SLOW)
	{
		strcpy(txt, "慢速");
	}
	drawCNString(txt);
	glFlush();
}



void btn7_dis()
{
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
	glColor3f(0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2f(-1, -0.5);
	char txt[20];
	sprintf(txt, "步数:%d", step);
	drawCNString(txt);
	glFlush();
}

void btn8_dis()
{
	glClearColor(1, 1, 1, 0.0f);
	glColor3f(0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2f(-1, -0.5);
	drawCNString("保存");
	glFlush();
}

void saveFile()
{
	char szPathName[MAX_PATH];
	OPENFILENAME ofn = { OPENFILENAME_SIZE_VERSION_400 };
	ofn.hwndOwner = GetForegroundWindow();
	strcpy(szPathName, "");
	ofn.lpstrFile = szPathName;
	ofn.nMaxFile = sizeof(szPathName);
	ofn.lpstrTitle = "选择文件";
	ofn.lpstrFilter = "*.gol\0*.gol\0*.*\0*.*\0\0";
	//
	char szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	ofn.lpstrInitialDir = szCurDir;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
	BOOL bOK = GetSaveFileName(&ofn);
	if (bOK)
	{
		FILE* fp = fopen(szPathName, "wb+");
		for (size_t i = 0; i < SLOTNUMW; i++)
		{
			fwrite(SLOT[i], sizeof(bool), SLOTNUMH, fp);
		}
		fclose(fp);
		MessageBox(hwnd, "保存完毕", "", MB_OK);
	}
}

void btn8_clicked(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN&&button == GLUT_LEFT_BUTTON)
	{
		saveFile();
	}
}

void btn9_dis()
{
	glClearColor(1, 1, 1, 0.0f);
	glColor3f(0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2f(-1, -0.5);
	drawCNString("读取");
	glFlush();
}
void openFile()
{
	char szPathName[MAX_PATH];
	OPENFILENAME ofn = { OPENFILENAME_SIZE_VERSION_400 };
	ofn.hwndOwner = GetForegroundWindow();
	strcpy(szPathName, "");
	ofn.lpstrFile = szPathName;
	ofn.nMaxFile = sizeof(szPathName);
	ofn.lpstrTitle = "选择文件";
	ofn.lpstrFilter = "*.gol\0*.gol\0*.*\0*.*\0\0";
	//
	char szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	ofn.lpstrInitialDir = szCurDir;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
	BOOL bOK = GetOpenFileName(&ofn);
	if (bOK)
	{
		running = false;
		FILE* fp = fopen(szPathName, "rb+");
		clearSLOTS();
		for (size_t i = 0; i < SLOTNUMW; i++)
		{
			fread(SLOT[i], sizeof(bool), SLOTNUMH, fp);
		}
		fclose(fp);
	}
}
void btn9_clicked(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN&&button == GLUT_LEFT_BUTTON)
	{
		openFile();
	}
}

void btn10_dis()
{
	glClearColor(1, 1, 1, 0.0f);
	glColor3f(0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2f(-1, -0.5);
	drawCNString("放大镜");
	glFlush();
}
void btn10_clicked(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN&&button == GLUT_LEFT_BUTTON)
	{
		reshapeMode = !reshapeMode;
	}
}


void next_dis()
{
	glClearColor(1, 1, 1, 0.0f);
	glColor3f(0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2f(-1, -0.5);
	drawCNString("下一步");
	glFlush();
}
void next_clicked(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN&&button == GLUT_LEFT_BUTTON)
	{
		_calc();
	}
}

void create_buttons(int own)
{
	run = new Button(own, 10, 5, 40, 20, btn1_dis, btn1_clicked);
	clear = new Button(own, 60, 5, 40, 20, btn4_dis, btn4_clicked);
	reshape = new Button(own, 160, 5, 50, 20, btn10_dis, btn10_clicked);
	save = new Button(own, 220, 5, 40, 20, btn8_dis, btn8_clicked);
	load = new Button(own, 270, 5, 40, 20, btn9_dis, btn9_clicked);
	next = new Button(own, 160, 35, 50, 20, next_dis, next_clicked);
	speed = new Button(own, 110, 5, 40, 20, btn2_dis, 0);
	glutCreateMenu(btn2_menu);
	glutAddMenuEntry("极快", 1);
	glutAddMenuEntry("快速", 2);
	glutAddMenuEntry("中速", 3);
	glutAddMenuEntry("慢速", 4);
	glutAttachMenu(GLUT_LEFT_BUTTON);
	glutAttachMenu(GLUT_LEFT_BUTTON);

	label_Speed = new  Button(own, 110, 35, 40, 20, btn5_dis, 0);
	label_stepCount = new Button(own, 10, 35, 100, 20, btn7_dis, 0);

}
void main(int argc, char** argv)
{
	hwnd = FindWindow(NULL, "Game of life by Qiu");
	if (hwnd)
	{
		MessageBox(0, "已经有另一个相同程序正在运行", "", MB_OK);
		return;
	}
	char title[260];
	strcpy(title, "Game of life by Qiu");
	strcat(title, "   version:");
	strcat(title, version);
	//
	initSLOT();
	glutInit(&argc, argv);
	glutInitWindowSize(width, height + 65);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	frameWindow = glutCreateWindow(title);
	glutKeyboardFunc(keyProcess);
	glutDisplayFunc(mdisplay);
	mainWindow = glutCreateSubWindow(frameWindow, 0, 0, width, height);
	gluOrtho2D(0, SLOTNUMW / scale, SLOTNUMH / scale, 0);
	glutMotionFunc(onMouseDrag);
	glutMouseFunc(onMouseClick);
	glutPassiveMotionFunc(onMousePass);
	glutReshapeFunc(changeSize);
	glutDisplayFunc(display);
	//


	controler = glutCreateSubWindow(frameWindow, 0, height, width, 65);
	glutDisplayFunc(controler_display);
	glutMouseFunc(controler_onMouseClick);
	create_buttons(controler);

	hwnd = FindWindow(NULL, title);
	playDelay=SPEED_MID;
	SetTimer(hwnd, 1, 1, TimerProc);
	glutMainLoop();
}