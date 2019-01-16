#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "movieRead.h"

#define FILE_NAME1 "movies.dat"
#define FILE_NAME2 "tags.dat"
#define MOVIE_BLOCK_SIZE 15000 // 10681개
#define TAGS_BLOCK_SIZE 100000 // 95580개
#define USER_BLOCK_SIZE 5000 // 4009명



void movieTokenizing(char * buffer, char * mNameToken, char(*genreToken)[64], int*indexToken)
{
	int count = 0;
	for (int j = 0; buffer[j] != '\0'; j++) {
		if (buffer[j] == ':')
			count++;
	}

	char *genre;



	char *ptr = strtok(buffer, ":");
	*indexToken = atoi(ptr);
	ptr = strtok(NULL, ":");
	strcpy(mNameToken, ptr);
	for (int i = 0; i < count - 4; i++)
	{
		ptr = strtok(NULL, ":");
		strcat(mNameToken, ":");
		strcat(mNameToken, ptr);
	}
	ptr = strtok(NULL, ":");
	genre = ptr;

	ptr = strtok(genre, "|");

	for (int i = 0; i < 10; i++)
	{
		ptr = strtok(NULL, "|");
		if (!ptr)
		{
			for (int j = i; j < 10;j++)
				strcpy(genreToken[j], "");
			break;

		}


		strcpy(genreToken[i], ptr);
	}



}
void tagTokenizing(char * buffer, int * uId, int * mId, char * tagToken, int * uIdFlag)
{
	int count = 0;
	for (int j = 0; buffer[j] != '\0'; j++) {
		if (buffer[j] == ':')
			count++;
	}


	char *ptr = strtok(buffer, ":");
	if (*uId != atoi(ptr))
	{
		*uId = atoi(ptr);
		*uIdFlag = 1;
	}
	else
		*uIdFlag = 0;

	ptr = strtok(NULL, ":");
	*mId = atoi(ptr);
	ptr = strtok(NULL, ":");
	strcpy(tagToken, ptr);

	for (int i = 0; i < count - 6; i++)
	{
		ptr = strtok(NULL, ":");
		strcat(tagToken, ":");
		strcat(tagToken, ptr);
	}

}

movie * findMovieBlock(int idx, movie * movieBlock[MOVIE_BLOCK_SIZE], int lastArrayIdx)
{

	int low = 0;
	int high = lastArrayIdx;
	int mid = 0;
	while (low <= high) {
		mid = (low + high) / 2;

		if (movieBlock[mid]->mId > idx)
		{
			high = mid - 1;
		}
		else if (movieBlock[mid]->mId < idx)
			low = mid + 1;
		else
		{
			return movieBlock[mid];
		}

	}

	printf("There isn't the movie Index %d \n", idx);
	return NULL;
}

void initMovieBlock(movie * movieBlock[MOVIE_BLOCK_SIZE], int * lastIdx)
{
	char buffer[1024] = "";
	char mNameToken[256] = "";
	char genreToken[10][64] = { "", };
	int indexToken = 0;

	FILE * fp = fopen(FILE_NAME1, "r");
	if (fp == NULL)
	{
		printf("File isnt exist");
		exit(-1);
	}
	for (int bIdx = 0; MOVIE_BLOCK_SIZE > bIdx; bIdx++)
	{
		if (fgets(buffer, sizeof(buffer) - 1, fp) == NULL)
		{
			printf("Last Block Index : %d \n", bIdx - 1);
			*lastIdx = bIdx - 1;
			fclose(fp);
			return;// 파일에서 더이상 읽을 수 있는 데이터가 없을때, NULL
		}
		buffer[strlen(buffer) - 1] = '\0'; // 개행문자제거

		movieTokenizing(buffer, mNameToken, genreToken, &indexToken);
		struct _movie * myMovie = malloc(sizeof(struct _movie)); // 964 Byte

		myMovie->mId = indexToken;
		strcpy(myMovie->name, mNameToken);
		for (int i = 0; i < 10; i++)
		{
			if (strlen(genreToken[i]) == 0)
			{
				for (int j = i; j < 10; j++)
					strcpy(myMovie->genre[j], "");
				break;
			}
			strcpy(myMovie->genre[i], genreToken[i]); // to be modify || || || || tokenizing

		}

		myMovie->tagSize = 0;
		movieBlock[bIdx] = myMovie;
		//Push(&(lengthList[strlen(mNameToken)]->moviePtrStack),myMovie);// length 기준으로 length List에 추가  이후 search시에는 1. length기준으로 영화 이름에 해당하는  block에서 movie Ptr찾기 2. 찾은 ptr기준으로 태그출력
	}
	// MOVIE_BLOCK_SIZE 넘을때.

	*lastIdx = MOVIE_BLOCK_SIZE - 1;
	fclose(fp);
	return;
	//if eof break
}


void initTagBlock(tags* tagBlock[TAGS_BLOCK_SIZE], movie * movieBlock[MOVIE_BLOCK_SIZE], user * userBlock[USER_BLOCK_SIZE], int *lastIdx, int * userLastIdx, int lastMovieBlockIndex)
{

	FILE * fp = fopen(FILE_NAME2, "r");
	if (fp == NULL)
	{
		printf("File isnt exist");
		exit(-1);
	}
	char buffer[1024];
	//tokenizing
	int uId = 0;
	int mId = 0;
	int uIdFlag = 0;
	char tagToken[256];
	int uBIdx = -1;
	for (int bIdx = 0; bIdx < TAGS_BLOCK_SIZE; bIdx++)
	{
		if (fgets(buffer, sizeof(buffer) - 1, fp) == NULL)
		{
			*lastIdx = bIdx - 1;
			*userLastIdx = uBIdx;
			printf("Last tag Block Index : %d \n", bIdx - 1);
			fclose(fp);
			return;// 파일에서 더이상 읽을 수 있는 데이터가 없을때, 
		}
		buffer[strlen(buffer) - 1] = '\0'; // 개행문자제거
		tagTokenizing(buffer, &uId, &mId, tagToken, &uIdFlag);


		struct _tags * myTag = malloc(sizeof(struct _tags)); // 964 Byte


		myTag->uId = uId;
		myTag->mId = mId;
		strcpy(myTag->tag, tagToken);

		movie * temp = findMovieBlock(mId, movieBlock, lastMovieBlockIndex);
		if (temp)
		{
			if (temp->tagSize < 256)
				temp->tagPtrList[temp->tagSize++] = myTag;
			else
				;	//printf("tag are more than 256 for the movie so that rest of tag pointer are not saved at %s movie structure  \n", temp->name);
			if (uIdFlag) // 새로운 유저
			{
				uBIdx++;
				struct _user * myUser = malloc(sizeof(struct _user));
				myUser->uId = uId;
				for (int i = 0; i < 19; i++)
					myUser->genreCount[i] = 0;
				userBlock[uBIdx] = myUser;

			}
			for (int i = 0; i < 10; i++)
			{
				if (strlen(temp->genre[i]) == 0)
					break;

				temp->genre[i];

				if (strcmp(temp->genre[i], "Adventure") == 0)
				{
					userBlock[uBIdx]->genreCount[0] += 1;
				}
				else if (strcmp(temp->genre[i], "Comedy") == 0)
				{
					userBlock[uBIdx]->genreCount[1] += 1;
				}
				else if (strcmp(temp->genre[i], "Action") == 0)
				{
					userBlock[uBIdx]->genreCount[2] += 1;

				}
				else if (strcmp(temp->genre[i], "Animation") == 0)
				{
					userBlock[uBIdx]->genreCount[3] += 1;

				}
				else if (strcmp(temp->genre[i], "Drama") == 0)
				{
					userBlock[uBIdx]->genreCount[4] += 1;

				}
				else if (strcmp(temp->genre[i], "Crime") == 0)
				{
					userBlock[uBIdx]->genreCount[5] += 1;

				}
				else if (strcmp(temp->genre[i], "Children") == 0)
				{
					userBlock[uBIdx]->genreCount[6] += 1;

				}
				else if (strcmp(temp->genre[i], "Documentary") == 0)
				{
					userBlock[uBIdx]->genreCount[7] += 1;

				}
				else if (strcmp(temp->genre[i], "Mystery") == 0)
				{

					userBlock[uBIdx]->genreCount[8] += 1;
				}
				else if (strcmp(temp->genre[i], "Thriller") == 0)
				{
					userBlock[uBIdx]->genreCount[9] += 1;

				}
				else if (strcmp(temp->genre[i], "Horror") == 0)
				{
					userBlock[uBIdx]->genreCount[10] += 1;

				}
				else if (strcmp(temp->genre[i], "Fantasy") == 0)
				{
					userBlock[uBIdx]->genreCount[11] += 1;

				}
				else if (strcmp(temp->genre[i], "Western") == 0)
				{
					userBlock[uBIdx]->genreCount[12] += 1;

				}
				else if (strcmp(temp->genre[i], "Romance") == 0)
				{

					userBlock[uBIdx]->genreCount[13] += 1;
				}
				else if (strcmp(temp->genre[i], "War") == 0)
				{

					userBlock[uBIdx]->genreCount[14] += 1;
				}
				else if (strcmp(temp->genre[i], "Musical") == 0)
				{
					userBlock[uBIdx]->genreCount[15] += 1;

				}
				else if (strcmp(temp->genre[i], "IMAX") == 0)
				{
					userBlock[uBIdx]->genreCount[16] += 1;

				}
				else if (strcmp(temp->genre[i], "Sci-Fi") == 0)
				{
					userBlock[uBIdx]->genreCount[17] += 1;

				}
				else if (strcmp(temp->genre[i], "Film-Noir") == 0)
				{
					userBlock[uBIdx]->genreCount[18] += 1;

				}
				else /* default: */
				{
					break;
				}
			}

		}
		else {
			//printf("mId : %d , moive didnt exist", mId);
		}



		tagBlock[bIdx] = myTag;




	}

	printf("tags data are more than 100000.");
	fclose(fp);
	return;

}

user * findUserBlock(int uid, user * userBlock[USER_BLOCK_SIZE], int userLastIdx)
{
	int low = 0;
	int high = userLastIdx;
	int mid = 0;
	while (low <= high) {
		mid = (low + high) / 2;

		if (userBlock[mid]->uId > uid)
		{
			high = mid - 1;
		}
		else if (userBlock[mid]->uId < uid)
			low = mid + 1;
		else
		{
			return userBlock[mid];
		}

	}

	printf("There isn't the user Index %d \n", uid);
	return NULL;
}

void showAllMovieComponet(movie * m)
{
	printf("Genre : ");
	for (int i = 0; i < 10; i++)
	{
		if (m->genre[i] != NULL)
			printf("%s ", m->genre[i]);
		else
			break;
	}
	printf("\n");
	printf("Movie Name : %s", m->name);
	printf("\n");
	printf("Tags \n", m->name);
	if (m->tagSize != 0)
	{
		for (int i = 0; i < m->tagSize; i++)
		{
			printf("[%d] %s \n", i + 1, m->tagPtrList[i]->tag);
		}
	}
	else
	{
		printf("There aren't Tags For \"%s \"", m->name);
	}
	return;
}

void userSearch(user * userBlock[USER_BLOCK_SIZE], movie * movieBlock[MOVIE_BLOCK_SIZE], int userLastIdx, int movieLastIdx)
{
	char buffer[256];

	printf("User ID를 입력해주세요   \n");

	fgets(buffer, sizeof(buffer), stdin);
	buffer[strlen(buffer) - 1] = 0;
	int uid = atoi(buffer);
	user * myUser = NULL;
	if (!(myUser = findUserBlock(uid, userBlock, userLastIdx)))
		return;

	int mostCount = 0;
	int most = -1;
	for (int i = 0; i < 19; i++)
	{

		if (mostCount < myUser->genreCount[i])
		{
			mostCount = myUser->genreCount[i];
			most = i;
		}

	}
	char mostString[255];
	switch (most)
	{
	case 0:
		strcpy(mostString, "Adventure");
		break;
	case 1:
		strcpy(mostString, "Comedy");
		break;
	case 2:
		strcpy(mostString, "Action");
		break;
	case 3:
		strcpy(mostString, "Animation");
		break;
	case 4:
		strcpy(mostString, "Drama");
		break;
	case 5:
		strcpy(mostString, "Crime");
		break;
	case 6:
		strcpy(mostString, "Children");
		break;
	case 7:
		strcpy(mostString, "Documentary");
		break;
	case 8:
		strcpy(mostString, "Mystery");
		break;
	case 9:
		strcpy(mostString, "Thriller");
		break;
	case 10:
		strcpy(mostString, "Horror");
		break;
	case 11:
		strcpy(mostString, "Fantasy");
		break;
	case 12:
		strcpy(mostString, "Western");
		break;
	case 13:
		strcpy(mostString, "Romance");
		break;
	case 14:
		strcpy(mostString, "War");
		break;
	case 15:
		strcpy(mostString, "Musical");
		break;
	case 16:
		strcpy(mostString, "IMAX");
	case 17:
		strcpy(mostString, "Sci-Fi");
		break;
	case 18:
		strcpy(mostString, "Film-Noir");
		break;
	}

	printf("User ID : %d\n", myUser->uId);
	printf("User Favorite Genre: %s\n", mostString);

	int myBool = 0;
	int accessingBlockPoint;

	while (myBool == 0)
	{
		srand(time(NULL));
		accessingBlockPoint = (rand() % (movieLastIdx + 1)); //0~movie Lastidx 까지의 난수 생성

		for (int i = 0; i < 10; i++)
		{
			if (strlen(movieBlock[accessingBlockPoint]->genre[i]) != 0) // 장르확인
			{
				if (!strcmp(movieBlock[accessingBlockPoint]->genre[i], mostString))
				{
					myBool = 1;
					break;
				}
			}
			else
			{
				break;
			}
		}
	}

	showAllMovieComponet(movieBlock[accessingBlockPoint]);

}
int Select() {
	int num;
	char buffer[16];
	printf("Movie search Program 0:EXIT\n");
	printf("1: 입력된 영화에 달린 태그 검색 2: 입력된 태그가 달린 영화 검색 3: 영화 이름 검색 4: 태그 내용 검색 5: 영화 추천 받기\n");
	fgets(buffer, sizeof(buffer), stdin);

	num = atoi(buffer);
	return num;
}

void searchMovie(movie * movieBlock[MOVIE_BLOCK_SIZE], tags * tagBlock[TAGS_BLOCK_SIZE], int movieLastIdx, int tagLastIdx)
{


	char buffer[256];

	printf("영화를 입력해주세요.   \n");

	fgets(buffer, sizeof(buffer), stdin);
	buffer[strlen(buffer) - 1] = 0;

	movie *mov;
	tags *tag;

	clock_t startTime, endTime;
	double nProcessExcuteTime;
	startTime = clock();
	for (int i = 0; i < movieLastIdx; i++)
	{
		mov = movieBlock[i];
		
		if (strcmp(mov->name, buffer) == 0)
		{
			for (int j = 0; j < tagLastIdx; j++)
			{
				tag = tagBlock[j];
				if (mov->mId == tag->mId)
				{
					printf("%s\n", tag->tag);
				}
			}
		}
	}

	endTime = clock();

	nProcessExcuteTime = ((double)(endTime - startTime)) / CLOCKS_PER_SEC;
	printf("Excute time: %f\n", nProcessExcuteTime);

}	


void searchMovie1(movie * movieBlock[MOVIE_BLOCK_SIZE], tags * tagBlock[TAGS_BLOCK_SIZE], int movieLastIdx, int tagLastIdx)
{
	
	
	char buffer[256];

	printf("검색하고자 하는 단어를 입력해주세요 \n");

	fgets(buffer, sizeof(buffer), stdin);
	buffer[strlen(buffer) - 1] = 0;

	movie *mov;
	tags *tag;
	int n = strlen(buffer);

	for (int i = 0; i < movieLastIdx; i++)
	{
		mov = movieBlock[i];

		if (strncmp(mov->name, buffer, n) == 0)
		{
			printf("%s\n", mov->name);

		}
	}

}

void searchMovie2(movie * movieBlock[MOVIE_BLOCK_SIZE], tags * tagBlock[TAGS_BLOCK_SIZE], int movieLastIdx, int tagLastIdx)
{
	
	char buffer[256];

	printf("영화를 입력해주세요.   \n");

	fgets(buffer, sizeof(buffer), stdin);
	buffer[strlen(buffer) - 1] = 0;

	movie *mov;
	tags *tag;
	
	clock_t startTime, endTime;
	double nProcessExcuteTime;
	startTime = clock();


	for (int i = 0; i < movieLastIdx; i++)
	{
		mov = movieBlock[i];

		if (strncmp(mov->name, buffer, 1) == 0)
		{
				if (strcmp(mov->name, buffer) == 0)
				{
					for (int j = 0; j < tagLastIdx; j++)
					{
						tag = tagBlock[j];
						if (mov->mId == tag->mId)
						{
							printf("%s\n", tag->tag);
						}
					}
				}
		}
	}

	endTime = clock();

	nProcessExcuteTime = ((double)(endTime - startTime)) / CLOCKS_PER_SEC;
	printf("Excute time: %f\n", nProcessExcuteTime);
}

void searchTag(movie * movieBlock[MOVIE_BLOCK_SIZE], tags * tagBlock[TAGS_BLOCK_SIZE], int movieLastIdx, int tagLastIdx)
{

	
	char buffer[256];

	printf("태그를 입력해주세요. \n");


	fgets(buffer, sizeof(buffer), stdin);
	buffer[strlen(buffer) - 1] = 0;

	movie *mov;
	tags *tag;


	clock_t startTime, endTime;
	double nProcessExcuteTime;
	startTime = clock();

	for (int i = 0; i < tagLastIdx; i++)
	{
		tag = tagBlock[i];


		if (strcmp(tag->tag, buffer) == 0)
		{
			printf("%s\n", findMovieBlock(tag->mId, movieBlock, movieLastIdx)->name);
			
			/*	for (int j = 0; j < movieLastIdx; j++)
			{
				mov = movieBlock[j];
				if (tag->mId == mov->mId)
				{
					printf("%s\n", mov->name);
				}
			}
			*/
		}

	}

	endTime = clock();

	nProcessExcuteTime = ((double)(endTime - startTime)) / CLOCKS_PER_SEC;
	printf("Excute time: %f\n", nProcessExcuteTime);

	
}

void searchTag1(movie * movieBlock[MOVIE_BLOCK_SIZE], tags * tagBlock[TAGS_BLOCK_SIZE], int movieLastIdx, int tagLastIdx)
{
	char buffer[256];

	printf("단어를 입력해주세요. \n");


	fgets(buffer, sizeof(buffer), stdin);
	buffer[strlen(buffer) - 1] = 0;

	movie *mov;
	tags *tag;
	int n = strlen(buffer);

	for (int i = 0; i < tagLastIdx; i++)
	{
		tag = tagBlock[i];


		if (strncmp(tag->tag, buffer, n) == 0)
		{
			printf("%s\n", tag->tag);
		}

	}
}

void searchTag2(movie * movieBlock[MOVIE_BLOCK_SIZE], tags * tagBlock[TAGS_BLOCK_SIZE], int movieLastIdx, int tagLastIdx)
{
	
	
	char buffer[256];

	printf("태그를 입력해주세요. \n");

	fgets(buffer, sizeof(buffer), stdin);
	buffer[strlen(buffer) - 1] = 0;

	movie *mov;
	tags *tag;
	
	clock_t startTime, endTime;
	double nProcessExcuteTime;
	startTime = clock();

	for (int i = 0; i < tagLastIdx; i++)
	{
		tag = tagBlock[i];

		if (strncmp(tag->tag, buffer, 1) == 0)
		{
			
				if (strcmp(tag->tag, buffer) == 0)
				{
				
					printf("%s\n", findMovieBlock(tag->mId, movieBlock, movieLastIdx)->name);
				}
			
		}

	}

	endTime = clock();

	nProcessExcuteTime = ((double)(endTime - startTime)) / CLOCKS_PER_SEC;
	printf("Excute time: %f\n", nProcessExcuteTime);
}

int main(void)
{
	//lengthS * lengthList[256]; // 글자수 기준으로 관리 [0] - 1글자이름의 영화들 , [1] - 2글자이름의 영화들 ,...  (실제 최대 길이 153)

	clock_t timing;




	movie * movieBlock[MOVIE_BLOCK_SIZE]; // 15000개 (실질데이터 약10700개)14.74MB
	int movieLastIdx = 0;
	int tagLastIdx = 0;
	int userLastIdx = 0;
	initMovieBlock(movieBlock, &movieLastIdx);

	printf("movie data Fetching Finished \n");
	printf("movie data Last Index: %d \n", movieLastIdx);
	timing = clock();
	tags * tagBlock[TAGS_BLOCK_SIZE]; // 100000개 (실질데이터 약9.5만개)24.06MB 
	user * userBlock[USER_BLOCK_SIZE]; // 5000개 (실질데이터,약4000개)0.313MB
	initTagBlock(tagBlock, movieBlock, userBlock, &tagLastIdx, &userLastIdx, movieLastIdx);
	printf("Tag, User loading Time %f \n", (double)(clock() - timing));
	printf("user data Fetching Finished \n");
	printf("tag data Fetching Finished \n");
	printf("tag data Last Index: %d \n", tagLastIdx);

	printf("loading Finished\n");



	int key = 0;

	while ((key = Select()) != 0)
	{
		switch (key)
		{
		case 1: searchMovie(movieBlock, tagBlock, movieLastIdx, tagLastIdx); break;
		case 2: searchTag(movieBlock, tagBlock, movieLastIdx, tagLastIdx); break;
		case 3: searchMovie1(movieBlock, tagBlock, movieLastIdx, tagLastIdx); break;
		case 4: searchTag1(movieBlock, tagBlock, movieLastIdx, tagLastIdx); break;
		case 5: userSearch(userBlock, movieBlock, userLastIdx, movieLastIdx); break;
		default: printf("NO NUMBER\n"); break;


		}
	}


	return 0;

}
