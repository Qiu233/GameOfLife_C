#pragma once
#include <windows.h>
#include <math.h>
#include <iostream>
#include <thread>
#include <ShlObj.h>
#include "glut.h"

void drawCNString(const char* str) {
	int len, i;
	wchar_t* wstring;
	HDC hDC = wglGetCurrentDC();
	GLuint list = glGenLists(1);

	len = 0;
	for (i = 0; str[i] != '\0'; ++i)
	{
		if (IsDBCSLeadByte(str[i]))
			++i;
		++len;
	}

	wstring = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, -1, wstring, len);
	wstring[len] = L'\0';

	for (i = 0; i<len; ++i)
	{
		wglUseFontBitmapsW(hDC, wstring[i], 1, list);
		glCallList(list);
	}

	free(wstring);
	glDeleteLists(list, 1);
}


class Button
{
private:
	int X, Y;
	int WIDTH, HEIGHT;
public:
	int winid;
	Button(int, int, int, int, int, void(*)(), void(*)(int, int, int, int));
	void btn_destroy();
};


Button::Button(int own, int X, int Y, int WIDTH, int HEIGHT, void(*disFunc)(), void(*mouseFunc)(int, int, int, int))
{
	this->X = X;
	this->Y = Y;
	this->WIDTH = WIDTH;
	this->HEIGHT = HEIGHT;
	this->winid = glutCreateSubWindow(own, X, Y, WIDTH, HEIGHT);
	glutDisplayFunc(disFunc);
	glutMouseFunc(mouseFunc);
}



void Button::btn_destroy()
{
	glutDestroyWindow(winid);
}


