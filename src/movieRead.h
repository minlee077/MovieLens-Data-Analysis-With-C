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
	int tagSize; // tagPtrList의 할당된 last index
	tags * tagPtrList[256]; // tag 파일 읽을때 할당 // 태그 최대 256개 할당 가정

}movie;

typedef struct _user{
	int uId;
	int genreCount[19];


}user;


