#define IsPlayerWall(cellType) \
	((cellType) == CT_WALL || m_isRange((cellType), ET_COOKIE_FIRST, ET_COOKIE_LAST))

#define EDIT_WIN_W 240
#define EDIT_WIN_H ((SCREEN_H / 16) * 16)

void GameMain(void);
