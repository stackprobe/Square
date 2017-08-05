enum // SANWA Pad Btnno
{
	SNWPB_DIR_2,
	SNWPB_DIR_4,
	SNWPB_DIR_6,
	SNWPB_DIR_8,
	SNWPB_A1, // ���i��
	SNWPB_A2, // ���i��
	SNWPB_A3, // ���i�E
	SNWPB_B1, // ��i��
	SNWPB_B2, // ��i��
	SNWPB_B3, // ��i�E
	SNWPB_L,
	SNWPB_R,
	SNWPB_USTART, // ��X�^�[�g
	SNWPB_DSTART, // ���X�^�[�g
};

#define PAD_MAX 16
#define PAD_BUTTON_MAX 32

int GetPadCount(void);
void PadEachFrame(void);
int GetPadInput(int padId, int btnId);
int GetPadPound(int padId, int btnId);
