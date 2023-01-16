#pragma once
#ifndef FILE_H
#define FILE_H
#include <list>

using namespace std;

class JSONObject;

class JSONArray 
{
public:
	JSONArray();
	~JSONArray();
	JSONArray(string arrayString);

	string toString();

	string stringAt(int key);
	JSONObject* jsonAt(int key);
	int intAt(int key);
	string getStringValue(string key);

	bool readJsonArray(string arrayString);
	bool readIntArray(string arrayString);
	bool readStringArray(string arrayString);


	list<JSONObject*> jsonObjects;
	bool readJson(size_t * index, string json);
	list<string> stringValues;
	list<int> intValues;
	

};
#endif
