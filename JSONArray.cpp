#include "JSONArray.h"
#include <iostream>
#include <algorithm>
#include <string>
#include "JSONObject.h"

using namespace std;

JSONArray::JSONArray()
{
}


JSONArray::~JSONArray()
{
}

JSONArray::JSONArray(string arrayString)
{
	size_t length = arrayString.length();

	if (arrayString.at(0) != '[' || arrayString.at(length - 1) != ']')
	{
		cerr << "INVALID JSONArray"<<endl;
		cerr.flush();
		return;
	}
	if (length == 2)
		return;
	size_t sec, index = 1;
	char c = arrayString.at(index);
	string value;
	int intValue;
	while (c == '\n' || c == ' ')
		index++;
	arrayString = arrayString.substr(index, length - index - 1);
	if (c == '"')
	{
		if (!readStringArray(arrayString))
		{
			cerr << "Not able to parse string Array " << endl;
			cerr.flush();
			return;
		}
	}
	//get json
	else if (c == '{')
	{
		if (!readJsonArray(arrayString))
		{
			cerr << "Not able to parse Json Array " << endl;
			cerr.flush();
			return;
		}
	}
	//get int
	else if (isdigit(c))
	{
		if (!readIntArray(arrayString))
		{
			cerr << " Invalid int array" << endl;
			cerr.flush();
			return;
		}
	}
	else
	{
		cerr << "Can not parse : " << endl;
		cerr.flush();
		return;
	}

}

string JSONArray::toString()
{
	string arrayString = "[";
	for (string s : stringValues)
	{
		arrayString.append("\"").append(s).append("\"").append(",");
	}

	for (int s : intValues)
	{
		arrayString.append("\"").append(to_string(s)).append("\"").append(",");
	}
	arrayString.pop_back();
	arrayString.append("]");
	return arrayString;
}

bool JSONArray::readJsonArray(string arrayString)
{
	size_t length = arrayString.length();

	size_t sec, index = 0;
	char c;
	string value;

	while (index < length - 1)
	{
		c = arrayString.at(index);
		if (c == '\n' || c == ' ')
		{
			index++;
			continue;
		}
		//read value
		if (c == '{')
			readJson(&index, arrayString);
		else
			return false;

		//readComma
		while (index != length && arrayString.at(index) != ',')
		{
			if (arrayString.at(index) != ',' && arrayString.at(index) != ' '  && arrayString.at(index) != '\n')
				return false;
			index++;
		}
		index++;
	}
	return true;
}

bool JSONArray::readJson(size_t* index, string json)
{
	size_t startingIndex = *index;
	int count = 1;
	(*index)++;
	while (count > 0)
	{
		if (json.find('}', *index + 1) < json.find('{', *index + 1))
		{
			count--;
			*index = json.find('}', *index) + 1;
		}
		else
		{
			count++;
			*index = json.find('{', *index) + 1;
		}
	}
	//size_t sec = json.find('}', index + 1);
	string value = json.substr(startingIndex, *index - startingIndex);
	JSONObject* innerObject = new JSONObject(value);
	jsonObjects.push_back(innerObject);
	return true;
}

bool JSONArray::readIntArray(string arrayString)
{
	arrayString.erase(remove(arrayString.begin(), arrayString.end(), ' '), arrayString.end());
	arrayString.erase(remove(arrayString.begin(), arrayString.end(), '\n'), arrayString.end());

	size_t length = arrayString.length();
	size_t sec, index = 0;
	char c;
	int intValue;
	string value;

	while (index < length)
	{
		sec = arrayString.find(',', index + 1);
		if (sec == string::npos)
			break;
		intValue = atoi(arrayString.substr(index, sec - index).c_str());
		intValues.push_back(intValue);
		index = sec + 1;
	}
	sec = arrayString.find_last_of(',');
	intValue = atoi(arrayString.substr(sec, length - 1).c_str());
	intValues.push_back(intValue);
	return true;
}

bool JSONArray::readStringArray(string arrayString)
{
	size_t length = arrayString.length();
	size_t sec, index = 0;
	char c;
	int intValue;
	string value;

	while (index < length - 1)
	{
		while (arrayString.at(index) == ' ' || arrayString.at(index) == '\n')
			index++;

		if (arrayString.at(index) == '"')
		{
			sec = arrayString.find('"', index + 1);
			value = arrayString.substr(index + 1, sec - index - 1);
			index = sec + 1;
			stringValues.push_back(value);
		}
		while (index != length && arrayString.at(index) != ',')
		{
			if (arrayString.at(index) != ' ' && arrayString.at(index) != '\n')
				return false;
			index++;
		}
		index++;
	}
	return true;
}

string JSONArray::stringAt(int index)
{
	size_t pos = 0;
	auto itr = stringValues.begin();
	while (itr != stringValues.end() && pos < index)
	{
		pos++;
		itr++;
	}
	if (pos == index)
		return *itr;
	if (itr == stringValues.end())
	{
		pos = 0;
		auto itr2 = jsonObjects.begin();
		while (itr2 != jsonObjects.end() && pos < index)
		{
			pos++;
			itr2++;
		}
		if (pos == index)
			return (*itr2)->toString();
	}
}

JSONObject* JSONArray::jsonAt(int index)
{
	size_t pos = 0;
	auto itr2 = jsonObjects.begin();
	while (itr2 != jsonObjects.end() && pos < index)
	{
		pos++;
		itr2++;
	}
	if (pos == index)
		return *itr2;
	return new JSONObject();
}

int JSONArray::intAt(int index)
{
	size_t pos = 0;
	auto itr2 = intValues.begin();
	while (itr2 != intValues.end() && pos < index)
	{
		pos++;
		itr2++;
	}
	if (pos == index)
		return *itr2;
	return 0;
}

string JSONArray::getStringValue(string key)
{
	size_t pos;
	string subKey, remainingKey;

	if ((pos = key.find('.', 0)) != string::npos)
	{
		subKey = key.substr(0, pos - 0);
		remainingKey = key.substr(pos + 1, key.length() - pos);
		if (isdigit(subKey.at(0)))
		{
			return jsonAt(atoi(subKey.c_str()))->getStringValue(remainingKey);
		}
		else
		{
			cerr << "Invalid JSONArray path " << key << endl;
			cerr.flush();
			return string();
		}
	}
	if (isdigit(key.at(0)))
		return stringAt(atoi(key.c_str()));
	else
	{
		cerr << "Invalid JSONArray path " << key << endl;
		cerr.flush();
		return string();
	}
}


//
//int main() {
//	std::cout << "start." << endl;
//	cout.flush();
//	int a;
//	JSONArray j = JSONArray("[]");
//	JSONArray s = JSONArray("[\"dfd\",\"fdf\"]");
//	JSONArray n = JSONArray("[1, 2, 45, 135, 0]");
//	JSONArray o = JSONArray("[{\"dfd\":\"fdf\"}, {\"dfd\":\"fdf\"},  {\"dfd\":\"fdf\"}]");
//	JSONArray b = JSONArray("[{\"text\":\"allianceMobileSwitchOn\",\"value\":true},{\"text\":\"allianceFunctionalCondition\",\"value\":true},{\"text\":\"allianceBodyCondition\",\"value\":true},{\"text\":\"allianceScreenCondition\",\"value\":true}]");
//	cout << s.stringAt(0)<<endl;
//	cout << n.intAt(3)<<endl;
//	cout << o.stringAt(1)<<endl;
//	cout << b.jsonAt(3)->getBooleanValue("value")<<endl;
//	cin >> a;
//
//	return 0;
//}
