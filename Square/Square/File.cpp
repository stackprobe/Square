#include "all.h"

#define FIND_MAX 10000

/*
	unsigned attrib;
		_A_ARCH
		_A_HIDDEN
		_A_NORMAL
		_A_RDONLY
		_A_SUBDIR
		_A_SYSTEM

	time_t time_create;
	time_t time_access;
	time_t time_write;
	_fsize_t size;
	char name[_MAX_PATH];
*/
struct _finddata_t lastFindData;

/*
	dir �����̃T�u�f�B���N�g���E�t�@�C���̃��X�g��Ԃ��B
	�Ԃ��T�u�f�B���N�g���E�t�@�C���́u���[�J�����v�Ȃ̂Œ��ӁI

	subDirs: NULL == �o�͂��Ȃ��B
	files: NULL == �o�͂��Ȃ��B
*/
void getFileList(char *dir, autoList<char *> *subDirs, autoList<char *> *files)
{
	errorCase(m_isEmpty(dir));

	char *wCard = xcout("%s\\*", dir);
	intptr_t h = _findfirst(wCard, &lastFindData);
	memFree(wCard);

	if(h != -1)
	{
		int foundNum = 0;

		do
		{
			char *name = lastFindData.name;

			if(name[0] == '.' && (name[1] == '\0' || name[1] == '.' && name[2] == '\0')) // ".", ".." �����O
				continue;

			// app >

			if(!_stricmp(name, "_Dummy_.txt"))
				continue;

			/*
				������ ProcessMessage() ���Ă΂Ȃ��� BGM �����������Ȃ邩�Ǝv�������ǁA�����Ɠr�؂ꂸ���[�v�����B
			*/
//			Sleep(1000); // test

			// < app

			errorCase(m_isEmpty(name));
			errorCase(strchr(name, '?')); // ? �ςȕ������܂�

			if(lastFindData.attrib & _A_SUBDIR) // ? subDir
			{
				if(subDirs)
					subDirs->AddElement(strx(name));
			}
			else // ? file
			{
				if(files)
					files->AddElement(strx(name));
			}
			foundNum++;

			if(FIND_MAX <= foundNum)
				break;
		}
		while(_findnext(h, &lastFindData) == 0);

		_findclose(h);
	}
}
void updateFindData(char *path)
{
	errorCase(m_isEmpty(path));

	intptr_t h = _findfirst(path, &lastFindData);
	errorCase(h == -1);
	_findclose(h);
}

// ----

int accessible(char *path)
{
	return !_access(path, 0);
}
char *refLocalPath(char *path)
{
	char *p = jstrrchr(path, '\\');

	if(p)
		return p + 1;

	return path;
}
