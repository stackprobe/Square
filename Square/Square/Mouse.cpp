#include "all.h"

int MouseRot;

static int MouseStatus[MOUBTN_MAX];

void MouseEachFrame(void)
{
	int status;

	if(WindowIsActive)
	{
		MouseRot = GetMouseWheelRotVol();
		status = GetMouseInput();
	}
	else // ? ��A�N�e�B�u -> ������
	{
		MouseRot = 0;
		status = 0;
	}
	m_range(MouseRot, -IMAX, IMAX);

	updateInput(MouseStatus[MOUBTN_L], status & MOUSE_INPUT_LEFT);
	updateInput(MouseStatus[MOUBTN_M], status & MOUSE_INPUT_MIDDLE);
	updateInput(MouseStatus[MOUBTN_R], status & MOUSE_INPUT_RIGHT);
}
int GetMouInput(int mouBtnId)
{
	errorCase(mouBtnId < 0 || MOUBTN_MAX <= mouBtnId);

	return FreezeInputFrame ? 0 : MouseStatus[mouBtnId];
}
int GetMouPound(int mouBtnId)
{
	return isPound(GetMouInput(mouBtnId));
}

int MouseX = SCREEN_CENTER_X;
int MouseY = SCREEN_CENTER_Y;

void UpdateMousePos(void)
{
	errorCase(GetMousePoint(&MouseX, &MouseY)); // ? ���s

	MouseX *= SCREEN_W;
	MouseX /= Gnd.RealScreen_W;
	MouseY *= SCREEN_H;
	MouseY /= Gnd.RealScreen_H;
}
void ApplyMousePos(void)
{
	int mx = MouseX;
	int my = MouseY;

	mx *= Gnd.RealScreen_W;
	mx /= SCREEN_W;
	my *= Gnd.RealScreen_H;
	my /= SCREEN_H;

	errorCase(SetMousePoint(mx, my)); // ? ���s
}

int MouseMoveX;
int MouseMoveY;

void UpdateMouseMove(void)
{
	static int lastFrame = -IMAX;
	static int centerX = SCREEN_CENTER_X;
	static int centerY = SCREEN_CENTER_Y;

	errorCase(ProcFrame <= lastFrame); // ? 2��ȏ�X�V�����B

	UpdateMousePos();

	MouseMoveX = MouseX - centerX;
	MouseMoveY = MouseY - centerY;

	MouseX = centerX;
	MouseY = centerY;

	ApplyMousePos();

	if(lastFrame + 1 < ProcFrame) // ? 1�t���[���ȏ�X�V���Ȃ������B
	{
		MouseMoveX = 0;
		MouseMoveY = 0;
	}
	lastFrame = ProcFrame;
}
