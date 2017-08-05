/*
	�Œ菬���_�� int �\���ł� denom
	1 == 0.001
	IMAX == 1,000,000
*/
#define IDENOM 1000

// fixed Point Int
typedef int fPInt_t; // (int)1 is (fPInt_t)IDENOM

#define m_fpi2i(value) \
	(((value) + IDENOM / 2) / IDENOM)

//----

#define CELLSIZE 32

#define JUMPFRAME_MAX 22
#define BORNFRAME_MAX 30
#define DEADFRAME_MAX 50
#define DEADFRAME_BURN 30
#define FLOORCLEARFRAME_ERASE 20
#define FLOORCLEARFRAME_MAX 100

#define MAP_W_MIN 40
#define MAP_W_MAX 1000
#define MAP_H_MIN 30
#define MAP_H_MAX 1000

#define FLOOR_NUM 9

enum // CellType
{
	// CellType -->

	CT_EMPTY, // as 0
	CT_WALL,
	CT_DEATH,
	CT_START,
	CT_GOAL,

	// EnemyType -->

	/*
		�A���J�m�C�h
		�����͏����x�N�g��
	*/
	ET_ARKANOID_6,
	ET_ARKANOID_3,
	ET_ARKANOID_2,
	ET_ARKANOID_1,
	ET_ARKANOID_4,
	ET_ARKANOID_7,
	ET_ARKANOID_8,
	ET_ARKANOID_9,

#define ET_ARKANOID_FIRST ET_ARKANOID_6
#define ET_ARKANOID_LAST ET_ARKANOID_9

	/*
		�p�^�p�^
	*/
	ET_LPATA,
	ET_LPATA_SLOW,
	ET_LPATA_HISPEED,
	ET_RPATA,
	ET_RPATA_SLOW,
	ET_RPATA_HISPEED,

#define ET_PATA_FIRST ET_LPATA
#define ET_PATA_LAST ET_RPATA_HISPEED

	/*
		�N�b�L�[
		�����͏����p�x(����)
		HT�͔����v���
	*/
	ET_COOKIE_6,
	ET_COOKIE_3,
	ET_COOKIE_2,
	ET_COOKIE_1,
	ET_COOKIE_4,
	ET_COOKIE_7,
	ET_COOKIE_8,
	ET_COOKIE_9,
	ET_HTCOOKIE_6,
	ET_HTCOOKIE_3,
	ET_HTCOOKIE_2,
	ET_HTCOOKIE_1,
	ET_HTCOOKIE_4,
	ET_HTCOOKIE_7,
	ET_HTCOOKIE_8,
	ET_HTCOOKIE_9,

#define ET_COOKIE_FIRST ET_COOKIE_6
#define ET_COOKIE_LAST ET_HTCOOKIE_9

#define ET_FIRST ET_ARKANOID_6
#define ET_LAST ET_HTCOOKIE_9

	CT_MAX, // num of (CT_* + ET_*)
};

#define CT_EDITABLE_FIRST CT_WALL
#define CT_EDITABLE_LAST ET_HTCOOKIE_9

typedef enum Touch_et
{
	TOUCH_NONE,
	TOUCH_L,
	TOUCH_R,
}
Touch_t;

#define METEOR_P_MAX 9 // P_METEOR_00 �̐F��
#define METEOR_D_MAX 20 // P_METEOR_00 �P�F�̖���
