#include "all.h"

/*
	padId: 0 - (PAD_MAX - 1) -> DX_INPUT_PAD1 ...
*/
#define PadId2InputType(padId) \
	((padId) + 1)

static int ButtonStatus[PAD_MAX][PAD_BUTTON_MAX];
static uint PadStatus[PAD_MAX];

int GetPadCount(void)
{
	static int num = -1;

	if(num == -1)
	{
		num = GetJoypadNum();
		errorCase(num < 0 || PAD_MAX < num);
	}
	return num;
}
void PadEachFrame(void)
{
	for(int padId = 0; padId < GetPadCount(); padId++)
	{
		uint status;

		if(WindowIsActive)
		{
			status = (uint)GetJoypadInputState(PadId2InputType(padId));
		}
		else // ? ��A�N�e�B�u
		{
			status = 0u; // ������
		}
		if(status)
		{
			for(int btnId = 0; btnId < PAD_BUTTON_MAX; btnId++)
			{
				updateInput(ButtonStatus[padId][btnId], status & 1u << btnId);
			}
		}
		else
		{
			for(int btnId = 0; btnId < PAD_BUTTON_MAX; btnId++)
			{
				updateInput(ButtonStatus[padId][btnId], 0);
			}
		}

		if(Gnd.PrimaryPadId == -1 && 10 < ProcFrame && !PadStatus[padId] && status) // �ŏ��Ƀ{�^�������������p�b�h�� PrimaryPadId �ɃZ�b�g
			Gnd.PrimaryPadId = padId;

		PadStatus[padId] = status;
	}
}
int GetPadInput(int padId, int btnId)
{
	if(padId == -1) // ? �����蓖��
		padId = 0;

	if(btnId == -1) // ? ���蓖�ăi�V
		return 0;

	errorCase(padId < 0 || PAD_MAX <= padId);
	errorCase(btnId < 0 || PAD_BUTTON_MAX <= btnId);

	return FreezeInputFrame ? 0 : ButtonStatus[padId][btnId];
}
int GetPadPound(int padId, int btnId)
{
	return isPound(GetPadInput(padId, btnId));
}
