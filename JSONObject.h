#pragma once
#include<map>
#ifndef FILE2_H
#define FILE2_H

//file contents here
using namespace std;

class JSONArray;

class JSONObject
{
	//friend class JSONArray;
public:
	JSONObject();
	~JSONObject();
	JSONObject(string json);

	string toString();

	string getStringValue(string key);
	bool getBooleanValue(string key);
	int getIntValue(string key);

	bool readColon(size_t* index, string json);
	bool readJson(size_t* index, string json, string key);
	bool readJsonArray(size_t* index, string json, string key);
	
	map<string, string> keyStringValueMap;
	map<string, int> keyIntValueMap;
	map<string, JSONObject> keyJsonObjectsMap;
	map<string, bool> booleanValuesMap;
	map<string, JSONArray*> keyJsonArrayMap;

};
#endif
