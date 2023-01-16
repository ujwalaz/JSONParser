#pragma once
#include<iostream>
#include <string>
#include<map>
#include<list>
#include<algorithm>
#include "JSONObject.h"
#include "JSONArray.h"


	JSONObject::JSONObject()
	{
	}

	JSONObject::~JSONObject()
	{
	}

	string JSONObject::toString()
	{
		string jsonstring = "", key, value;
		jsonstring.append("{");
		auto itr = keyStringValueMap.begin();
		while (itr != keyStringValueMap.end())
		{
			key = itr->first;
			value = itr->second;
			jsonstring.append("\"" + key + "\"" + ":" + "\"" + value + "\",");
			itr++;
		}
		auto itr2 = keyIntValueMap.begin();
		while (itr2 != keyIntValueMap.end())
		{
			key = itr2->first;
			value = itr2->second;
			jsonstring.append("\"" + key + "\"" + ":" + "\"" + value + "\",");
			itr2++;
		}
		auto itr3 = keyJsonObjectsMap.begin();
		while (itr3 != keyJsonObjectsMap.end())
		{
			key = itr3->first;
			value = itr3->second.toString();
			jsonstring.append("\"" + key + "\"" + ":" + "\"" + value + "\",");
			itr3++;
		}
		auto itr4 = booleanValuesMap.begin();
		while (itr4 != booleanValuesMap.end())
		{
			key = itr4->first;
			value = itr4->second ? "true" : "false";
			jsonstring.append("\"" + key + "\"" + ":" + "\"" + value + "\",");
			itr4++;
		}
		jsonstring.pop_back();
		jsonstring.append("}");
		return jsonstring;
	}

	JSONObject::JSONObject(string json)
	{
		size_t length = json.length();

		if (json.at(0) != '{' || json.at(length - 1) != '}')
		{
			cerr << "INVALID JSON";
			cerr.flush();
			return;
		}

		size_t index = 1;
		char c;
		size_t endOfValue = 7000000;
		string key, value;
		int intValue;
		bool booleanValue;
		while (index < length - 1)
		{
			c = json.at(index);
			if (c == '\n' || c == ' ')
			{
				index++;
				continue;
			}

			if (c == '"')
			{//get key value pair
				//get key
				endOfValue = json.find('"', index + 1);
				key = json.substr(index + 1, endOfValue - index - 1);

				//next should be :
				index = endOfValue + 1;
				if (readColon(&index, json) == -1)
					return;
				c = json.at(index);
				//get value
				//get string value
				if (c == '"')
				{
					endOfValue = json.find('"', index + 1);
					value = json.substr(index + 1, endOfValue - index - 1);
					index = endOfValue + 1;
					keyStringValueMap.insert({ key, value });
				}
				//get json
				else if (c == '{')
				{
					//substring between {} 
					// create new json
					readJson(&index, json, key);
				}
				else if (c == '[')
				{
					//substring between {} 
					// create new json
					readJsonArray(&index, json, key);
				}
				//get int
				else if (isdigit(c))
				{
					endOfValue = json.find(',', index + 1);
					if (endOfValue == -1)
						endOfValue = json.find('}', index + 1);
					value = json.substr(index, endOfValue - index);
					intValue = atoi(value.c_str());
					keyIntValueMap.insert({ key, intValue });
					index = endOfValue;
				}
				//get boolean
				else
				{
					endOfValue = json.find(',', index + 1);
					if (endOfValue == -1)
						endOfValue = json.find('}', index + 1);
					value = json.substr(index, endOfValue - index);
					index = endOfValue;
					if (value == "true")
						booleanValuesMap.insert({ key, true });
					else if (value == "false")
						booleanValuesMap.insert({ key, false });
					else
					{
						cerr << "INVALID JSON Object"<<endl;
						cerr.flush();
						return;
					}
				}
			}

			//next should be :
			if (json.at(index) != ',' && json.at(index) != '}')
			{
				cerr << "INVALID SYNTAX";
				cerr.flush();
				return;
			}
			index++;

		}
	}

	bool JSONObject::readColon(size_t* index, string json)
	{
		char c = json.at(*index);
		while (c != ':')
		{
			if (c != '\n' && c != ' ')
			{
				cerr << "INVALID SYNTAX";
				cerr.flush();
				return false;
			}
			c = json.at(++(*index));
		}
		(*index)++;
		c = json.at(*index);
		while (c == '\n' || c == ' ')
		{
			(*index)++;
			c = json.at(*index);
		}
		return true;
	}

	bool JSONObject::readJson(size_t* index, string json, string key)
	{
		size_t startingIndex = *index;
		size_t count = 1, length = json.length();
		(*index)++;
		while (count > 0 && *index < length)
		{
			/*if (json.find('{', *index + 1) == string::npos)
			{
				*index = json.find('}', *index);
				break;
			}*/
			if (json.find('}', *index) < json.find('{', *index))
			{
				count--;
				*index = json.find('}', *index)+1;
			}
			else
			{
				count++;
				*index = json.find('{', *index)+1;
			}
		}
		if (count > 0)
			return false;

		string value = json.substr(startingIndex, *index - startingIndex );
		JSONObject innerObject = JSONObject(value);
		keyJsonObjectsMap.insert({ key, innerObject });
		return true;
	}

	bool JSONObject::readJsonArray(size_t* index, string json, string key)
	{
		size_t startingIndex = *index;
		size_t count = 1, length = json.length();
		(*index)++;
		//find the actual end of current json array 
		while (count > 0 && *index < length)
		{
			/*if (json.find('[', *index + 1) == string::npos)
			{
				*index = json.find(']', *index);
				break;
			}*/
			if (json.find(']', *index) < json.find('[', *index))
			{
				count--;
				*index = json.find(']', *index) + 1;
			}
			else
			{
				count++;
				*index = json.find('[', *index) + 1;
			}
		}
		if (count > 0)
			return false;
		string value = json.substr(startingIndex, *index - startingIndex );
		JSONArray* innerObject = new JSONArray(value);
		keyJsonArrayMap.insert({ key, innerObject });
		return true;
	}
	 
	string JSONObject::getStringValue(string key)
	{
		size_t pos;
		string subKey, remainingKey;

		if ((pos = key.find('.', 0)) != string::npos)
		{
			subKey = key.substr(0, pos - 0);
			remainingKey = key.substr(pos + 1, key.length() - pos);

			auto itr1 = keyJsonObjectsMap.find(subKey);
			if (itr1 != keyJsonObjectsMap.end())
			{
				JSONObject j = itr1->second;
				return j.getStringValue(remainingKey);
			}

			auto itr2 = keyJsonArrayMap.find(subKey);
			if (itr2 != keyJsonArrayMap.end())
			{
				JSONArray j = *(itr2->second);
				return j.getStringValue(remainingKey);
			}
		}

		auto itr = keyStringValueMap.find(key);
		if (itr != keyStringValueMap.end())
		{
			return itr->second;
		}

		auto itr2 = keyJsonObjectsMap.find(key);
		if (itr2 != keyJsonObjectsMap.end())
		{
			return itr2->second.toString();
		}

		auto itr3 = keyJsonArrayMap.find(key);
		if (itr3 != keyJsonArrayMap.end())
		{
			return (*itr3->second).toString();
		}

		auto itr4 = booleanValuesMap.find(key);
		if (itr4 != booleanValuesMap.end())
		{
			return itr4->second ? "true" : "false";
		}

		return string();
	}

	int JSONObject::getIntValue(string key)
	{
		size_t pos;
		string subKey, remainingKey;
		int value = 0;

		if ((pos = key.find('.', 0)) != string::npos)
		{
			subKey = key.substr(0, pos - 0);
			remainingKey = key.substr(pos + 1, key.length() - pos);
			auto itr1 = keyJsonObjectsMap.find(subKey);
			if (itr1 != keyJsonObjectsMap.end())
			{
				JSONObject j = itr1->second;
				value = j.getIntValue(remainingKey);
				return value;
			}
		}
		auto itr = keyIntValueMap.find(key);
		if (itr != keyIntValueMap.end())
		{
			value = itr->second;
			return value;
		}
	}

	bool JSONObject::getBooleanValue(string key)
	{
		size_t pos;
		string subKey, remainingKey;
		if ((pos = key.find('.', 0)) != string::npos)
		{
			subKey = key.substr(0, pos - 0);
			remainingKey = key.substr(pos + 1, key.length() - pos);
			auto itr1 = keyJsonObjectsMap.find(subKey);
			if (itr1 != keyJsonObjectsMap.end())
			{
				JSONObject j = itr1->second;
				return j.getBooleanValue(remainingKey);
			}
		}
		bool value;
		auto itr = booleanValuesMap.find(key);
		if (itr != booleanValuesMap.end())
		{
			value = itr->second;
			return value;
		}

	}
	
	
	int main() {
		std::cout << "start." << endl;
		cout.flush();
		int a;
		JSONObject j = JSONObject("{}");

		JSONObject s = JSONObject("{\"key\":\"value\",\"int\":1}");
		string value = s.getStringValue("key");
		std::cout << "value : " << value << endl;

		JSONObject n = JSONObject("{\"key\":\"value\",\"int\":1,\"key2\":{\"ik\":\"v1\"}}");
		value = n.getStringValue("key2");
		std::cout << "key2 json value : " << value << endl;

		JSONObject test = JSONObject("{\"message\": \"Order already exist with this cec.\",\"code\" : \"9067\",\"dc\" : \"AB062\",\"r_id\" : \"47d5121a-876e-43f0-8127-50c7c44ec219\",\"ir\" : false}");
		std::cout << "message : " << test.getStringValue("message") << endl;
		std::cout << "ir : " << test.getBooleanValue("ir") << endl;

		JSONObject o = JSONObject("{\"key\":\"value\",\"int\":1,\"key2\":{\"innerKey\":{\"k1\":10000}}}");
		std::cout << "key2.ik : " << o.getStringValue("key2.ik") << endl;
		std::cout << "key2.innerKey.k1 : " << o.getIntValue("key2.innerKey.k1") << endl;

		o = JSONObject("{\"Status\": true,\"Message\": \"Success\",\"Detail\": {\"Brand\": [{\"Id\": 1,\"Name\": \"Samsung\"},{\"Id\": 2,\"Name\": \"LG\"},{\"Id\": 4,\"Name\": \"Voltas\"},{\"Id\": 2005,\"Name\": \"Whirlpool\"},{\"Id\": 2006,\"Name\": \"Panasonic\"},{\"Id\": 2007,\"Name\": \"Sony\"},{\"Id\": 2008,\"Name\": \"Others\"}]}}");
		std::cout << "Status (string): " << o.getStringValue("Status") << endl;
		std::cout << "Status (boolean): " << o.getBooleanValue("Status") << endl;
		std::cout << "Detail.Brand[3].Name" << o.getStringValue("Detail.Brand.3.Name") << endl;

		//std::cout << "Detail.Brand.0.Id : " << o.getIntValue("Detail.Brand.0.Id") << endl;
		o = JSONObject("{\"r_id\":\"23a242d0-ea9a-3a016a911101\",\"r_it\":false,\"dt\":[{\"id\":\"CSH16794\",\"name\":\"OnePlus 6 (6 GB/64 GB)\",\"variantName\":\"6 GB/64 GB\",\"brandId\":\"CSH90\",\"brandName\":\"OnePlus\",\"image\":\"https://s3n.cashify.in/cashify/product/img/xxhdpi/csh-j46ejfgj-pgtk.png\"},{\"id\":\"CSH16795\",\"name\":\"OnePlus 6 (8 GB/128 GB)\",\"variantName\":\"8 GB/128 GB\",\"brandId\":\"CSH90\",\"brandName\":\"OnePlus\",\"image\":\"https://s3n.cashify.in/cashify/product/img/xxhdpi/csh-j46ejfgj-pgtk.png\"},{\"id\":\"CSH16842\",\"name\":\"OnePlus 6 (8 GB/256 GB)\",\"variantName\":\"8 GB/256 GB\",\"brandId\":\"CSH90\",\"brandName\":\"OnePlus\",\"image\":\"https://xyz.com/xx/product/img/xxhdpi/csh-j46ejfgj-pgtk.png\"}]}");
		std::cout << "dt.0.id : " << o.getStringValue("dt.0.id")<<" "<< o.getStringValue("dt.0.name") << endl;
		std::cout << "dt.1.id : " << o.getStringValue("dt.1.id") << " " << o.getStringValue("dt.1.name") << endl;
		std::cout << "dt.2.image : " << o.getStringValue("dt.2.image") << endl;
		std::cin >> a;

		return 0;
	}

