#include <stdio.h>
#include <stdlib.h> //malloc.h
#include <conio.h>
#include <Windows.h>
#pragma warning (disable: 4996)

#define X_START 0
#define Y_START 0
#define MAP_INDEX (map_size * y + x)

#define ARROW_UP     72
#define ARROW_DOWN   80
#define ARROW_RIGHT  77
#define ARROW_LEFT   75
#define ESC          27
#define SPACE        32
#define KEY_Z_UNDER  122
#define ENTER        13

#define LIST_DEFAULT_SIZE  4
#define EXTEND_DEFAULT     -1 //list(동적 어레이)의 크기를 단순히 2배 늘릴 때의 매개변수이다.

const int INT_BITSIZE = sizeof(int);
int player_x = X_START;
int player_y = Y_START;
int game_end = 0;       //esc키를 누를 경우 값이 1이 되며 while루프를 빠져나온다. 였는데, 메모리 해제를 esc키 누르면 바로 하는걸로 바뀌어서 지금은 쓰이는 곳이 없다.
                        //허나 장차 쓰일 일이 생길 것 같기 때문에 남겨두었다.
int setting_end = 0;    //enter키를 누를 경우 값이 1이 되며 퍼즐 셋팅이 끝난다.
int map_size = 0;
int pos_index = 0;

int* map = NULL;
int**** p3DimArr;

enum BLOCK
{
	BLANK,
	CHECK_O,
	CHECK_X
};
enum SYSTEM_EXIT_CODE
{
	MALLOC_FAIL_EXIT,
	ESC_EXIT
};

void Initialize(int***, int***);
//*** 퍼즐 작성 파트 ***//
void SetPuzzle();
void KeyProcess();
void Move(int);
void PrintMap();
void SetCursorInfo_invisible();
void Cursor_gotoxy(int, int);
//*** 힌트 계산 파트 ***//
void GetColumnHint(int***);
void GetRowHint(int***);
//*** List 관리 파트 ***//
void EnsureCapacity(int, int**);
int* GetAssignedPointer(int _size);
void GetAssignedHintList_Column(int***);
void GetAssignedHintList_Row(int***);
//*** 메모리 관리 파트 ***//
void FreeMemory();
void FreeList(int**);
void FreeList(int***); //**주의** 오버로딩은 CPP의 문법이므로 C에서는 동작하지 않는다.
void FreeList(int****);

int main()
{

	/*
	int** hintlist_column;
	int** hintlist_row;
	Initialize(&hintlist_column, &hintlist_row);
	while (!game_end)
	{
		SetPuzzle();
		GetColumnHint(&hintlist_column);
		break;
	}
	FreeMemory();*/
	return 0;
}

void Initialize(int*** hintlist_column_ptr, int*** hintlist_row_ptr)
{
	SetCursorInfo_invisible();
	map_size = 0;
	printf("맵의 크기를 입력하세요.\n");
	scanf("%d", &map_size);
	const int LENGTH_BITSIZE_INT = map_size * INT_BITSIZE;
	system("cls");

	map = (int*)malloc(map_size * map_size * INT_BITSIZE);
	*hintlist_column_ptr = (int**)malloc(LENGTH_BITSIZE_INT);
	*hintlist_row_ptr = (int**)malloc(LENGTH_BITSIZE_INT);
	p3DimArr = (int****)malloc(LIST_DEFAULT_SIZE * INT_BITSIZE);
	if (map != NULL && *hintlist_column_ptr != NULL && *hintlist_row_ptr != NULL && p3DimArr != NULL)
	{
		memset(map, 0, map_size * map_size * INT_BITSIZE);
		memset(*hintlist_column_ptr, 0, LENGTH_BITSIZE_INT);
		memset(*hintlist_row_ptr, 0, LENGTH_BITSIZE_INT);
		memset(p3DimArr, 0, LIST_DEFAULT_SIZE * INT_BITSIZE);
	}
	else
	{
		printf("메모리 할당 실패. 게임을 종료합니다.\n");
		exit(MALLOC_FAIL_EXIT);
	}

	p3DimArr[0] = hintlist_column_ptr;
	p3DimArr[1] = hintlist_row_ptr;
}
//*** 퍼즐 작성 파트 ***//
void SetPuzzle()
{
	while (!setting_end)
	{
		pos_index = map_size * player_y + player_x;
		KeyProcess();
		PrintMap();
	}
}
void KeyProcess()
{
	Cursor_gotoxy(0, map_size + 1);
	printf("맵 작성을 끝내기 위해 ENTER를 누르세요. 게임을 끝내기 위해 ESC를 누르세요.\n");
	int select = -1;
	if (_kbhit())
	{
		select = getch();
		if (select == 224)
		{
			select = getch();
			Move(select);
		}
		else 
		{
			switch (select)
			{
			case ESC :
				printf("게임을 종료합니다.\n");
				FreeMemory();
				exit(ESC_EXIT);
				break;
			case SPACE :
				if (map[pos_index] == CHECK_O)
					map[pos_index] = BLANK;
				else
					map[pos_index] = CHECK_O;
				break;
			case KEY_Z_UNDER :
				if (map[pos_index] == CHECK_X)
					map[pos_index] = BLANK;
				else
					map[pos_index] = CHECK_X;
			case ENTER :
				setting_end = 1;
				break;
			}
		}
	}
}
void Move(int select)
{
	switch (select) 
	{
	case (ARROW_UP) :
		if (player_y - 1 >= 0)
			player_y--;
		break;
	case (ARROW_DOWN) :
		if (player_y + 1 < map_size)
			player_y++;
		break;
	case (ARROW_RIGHT) :
		if (player_x + 1 < map_size)
			player_x++;
		break;
	case (ARROW_LEFT) :
		if (player_x - 1 >= 0)
			player_x--;
		break;
	default: break;
	}
}
void PrintMap()
{
	for (int y = 0; y < map_size; y++)
	{
		for (int x = 0; x < map_size; x++)
		{
			Cursor_gotoxy(x, y);
			if (map[MAP_INDEX] == BLANK)
			{
				if (x == player_x && y == player_y)
					printf("◇");
				else
					printf("□");
			}
			else if (map[MAP_INDEX] == CHECK_O)
			{
				if (x == player_x && y == player_y)
					printf("◆");
				else
					printf("■");
			}
			else if (map[MAP_INDEX] == CHECK_X)
			{
				if (x == player_x && y == player_y)
					printf("◈");
				else
					printf("▩");
			}
		}
	}
}
void SetCursorInfo_invisible()
{
	CONSOLE_CURSOR_INFO cursor_info;
	cursor_info.bVisible = false;
	cursor_info.dwSize = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}
void Cursor_gotoxy(int x, int y)
{
	COORD coord = { x * 2, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
//*** 힌트 계산 파트 ***//
void GetColumnHint(int ***hintlist_column_ptr)
{
	GetAssignedHintList_Column(hintlist_column_ptr);

	int block_num = 0;
	int blockset_num;
	for (int x = 0; x < map_size; x++)
	{
		blockset_num = 0;
		for (int y = 0; y < map_size; y++)
		{
			if (map[MAP_INDEX] == CHECK_O)
				block_num++;
			else
			{
				if (block_num != 0)
				{
					(*hintlist_column_ptr)[x][blockset_num++] = block_num;
					block_num = 0;
				}
			}
		}
	}
}
void GetRowHint(int ***hintlist_row_ptr)
{
	GetAssignedHintList_Row(hintlist_row_ptr);

	int block_num = 0;
	int blockset_num;
	for (int y = 0; y < map_size; y++)
	{
		blockset_num = 0;
		for (int x = 0; x < map_size; x++)
		{
			if (map[MAP_INDEX] == CHECK_O)
				block_num++;
			else
			{
				if (block_num != 0)
				{
					(*hintlist_row_ptr)[y][blockset_num++] = block_num;
					block_num = 0;
				}
			}
		}
	}
}
//*** List 관리 파트 ***//
void EnsureCapacity(int _size, int** _list)
{
	int list_bitsize = _msize(_list);
	int** temp;

	temp = (int**)malloc(_size == EXTEND_DEFAULT ? list_bitsize * 2 : list_bitsize + _size * INT_BITSIZE);
	if (temp != NULL)
	{
		memset(temp, 0, list_bitsize * 2);
		for (int i = 0; i < list_bitsize / INT_BITSIZE; i++)
		{
			temp[i] = _list[i];
		}
		free(_list);
		_list = temp;
	}
	else
	{
		printf("메모리 할당 실패. 게임을 종료합니다.\n");
		exit(MALLOC_FAIL_EXIT);
	}
}
int* GetAssignedPointer(int _size)
{
	int* temp;
	temp = (int*)malloc(_size * INT_BITSIZE);
	if (temp != NULL)
	{
		memset(temp, 0, _size * INT_BITSIZE);
		return temp;
	}
	else
	{
		printf("메모리 할당 실패. 게임을 종료합니다.\n");
		exit(MALLOC_FAIL_EXIT);
	}
}
void GetAssignedHintList_Column(int ***hintlist_column_ptr)
{
	int block_num = 0;
	int blockset_num;
	for (int x = 0; x < map_size; x++)
	{
		blockset_num = 0;
		for (int y = 0; y < map_size; y++)
		{
			if (map[MAP_INDEX] == CHECK_O)
				block_num++;
			else
			{
				if (block_num != 0)
				{
					blockset_num++;
					block_num = 0;
				}
			}
		}
		(*hintlist_column_ptr)[x] = GetAssignedPointer(blockset_num);
	}
}
void GetAssignedHintList_Row(int ***hintlist_row)
{
	int block_num = 0;
	int blockset_num;
	for (int y = 0; y < map_size; y++)
	{
		blockset_num = 0;
		for (int x = 0; x < map_size; x++)
		{
			if (map[MAP_INDEX] == CHECK_O)
				block_num++;
			else
			{
				if (block_num != 0)
				{
					blockset_num++;
					block_num = 0;
				}
			}
		}
		(*hintlist_row)[y] = GetAssignedPointer(blockset_num);
	}
}
//*** 메모리 관리 파트 ***//
void FreeMemory()
{
	free(map);
	FreeList(p3DimArr);
}
void FreeList(int** _list)
{
	for (int i = 0; i < _msize(_list) / INT_BITSIZE; i++)
	{
		free(_list[i]);
	}
	free(_list);
}
void FreeList(int*** _list)
{
	for (int i = 0; i < _msize(_list) / INT_BITSIZE; i++)
	{
		FreeList(_list[i]);
	}
	free(_list);
}
void FreeList(int**** _list)
{
	for (int i = 0; i < _msize(_list) / INT_BITSIZE; i++)
	{
		FreeList(_list[i]);
	}
	free(_list);
}
