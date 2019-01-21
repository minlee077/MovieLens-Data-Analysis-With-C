# MovieLens-Data-Analysis-With-C

This project was part of the programming course in 2018-2
## Index
### 1. Objective
### 2. Dataset
### 3. Functionalites

## 1. Objectives
1. Read two files in Movielens Data set
2. Construct appropriate structures for managing these data.
3. With user query(tag) from command, find appropriate movies with tags.
4. With user query(movie) from command, suggest the opinions with tags.
5. suggest some additional functionalities.

## 2. Dataset (Format CSV with delimeter '::' )
Original source : https://movielens.org/
(Quotation of README.html file)
Summary
This data set contains 95580 tags applied to 10681 movies by 71567 users of the online movie recommender service MovieLens.

Users were selected at random for inclusion. All users selected had rated at least 20 movies. Unlike previous MovieLens data sets, no demographic information is included. Each user is represented by an id, and no other information is provided.

The data are contained in two files, movies.dat and tags.dat. More details about the contents and use of all these files follows.

This and other GroupLens data sets are publicly available for download at GroupLens Data Sets.


### Movies.dat ( Movie ID :: Movie Name :: Genres (with delimeter |) (about 10000 lines)

sample data

1::Toy Story (1995)::Adventure|Animation|Children|Comedy|Fantasy

2::Jumanji (1995)::Adventure|Children|Fantasy

3::Grumpier Old Men (1995)::Comedy|Romance

4::Waiting to Exhale (1995)::Comedy|Drama|Romance

5::Father of the Bride Part II (1995)::Comedy

### tags.dat (User ID :: Movie ID :: tag :: timeStamp) (about 95000 lines)

sample data

15::4973::excellent!::1215184630

20::1747::politics::1188263867

20::1747::satire::1188263867

25::6709::Johnny Depp::1162147221

31::65::buddy comedy::1188263759

31::546::strangely compelling::1188263674


## 3. Functionalities

### Basic Functions

#### -Tag query Fuction
Tag query by movie names

#### -Movie query Fuction
Movie query by tags

### Additional Fuctions
#### -Movie name search 
When user input part of the movie name, The full name of movie come out

#### -Movie Recommendation
When User Id is inputted, The program recommendate a movie which is based on users preference based on genre
