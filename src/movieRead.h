#pragma once
typedef struct _tags {
	int uId;
	int mId;
	char tag[256];
}tags;

typedef struct _movie {
	int mId;
	char name[256];
	char genre[10][16]; // 640 Byte
	int tagSize; // tagPtrList�� �Ҵ�� last index
	tags * tagPtrList[256]; // tag ���� ������ �Ҵ� // �±� �ִ� 256�� �Ҵ� ����

}movie;

typedef struct _user{
	int uId;
	int genreCount[19];


}user;


