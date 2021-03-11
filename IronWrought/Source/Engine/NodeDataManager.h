#pragma once

class CNodeDataManager
{
public:
	enum class EDataType
	{
		EFloat,
		EInt,
		EBool,
		EStart,
		EVector3,
		EList,
		ECount
	};

	~CNodeDataManager();
	static void Create() { ourInstance = new CNodeDataManager(); }
	static CNodeDataManager* Get() { return ourInstance; }
	void ClearStoredData();

	template <typename T>
	void SetData(const std::string aNodeDataKey, EDataType aDataType, T& aValue)
	{

		auto it = myNodeDataMap.find(aNodeDataKey);
		if (it == myNodeDataMap.end())
		{
				myNodeDataMap[aNodeDataKey] = new T(aValue);
				myNodeDataTypeMap[aNodeDataKey] = aDataType;
		}
		else
		{
			*(reinterpret_cast<T*>((*it).second)) = aValue;
		}
	}
	
	template <typename T>
	T GetData(const std::string aNodeDataKey)
	{
		auto it = myNodeDataMap.find(aNodeDataKey);
		return *(reinterpret_cast<T*>((*it).second));
	}

	void SaveDataTypesToJson();

private:
	std::unordered_map<std::string, void*> myNodeDataMap;
	std::unordered_map<std::string, EDataType> myNodeDataTypeMap;
	static CNodeDataManager* ourInstance;
};