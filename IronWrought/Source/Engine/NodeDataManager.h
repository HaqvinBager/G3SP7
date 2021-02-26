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
		ECount
	};

	~CNodeDataManager();
	static void Create() { ourInstance = new CNodeDataManager(); }
	static CNodeDataManager* Get() { return ourInstance; }
	void ClearStoredData();

	void SetData(const std::string aNodeDataKey, EDataType aDataType, void* aValue);
	void* GetData(const std::string aNodeDataKey, EDataType aDataType);

	void SaveDataTypesToJson();

private:
	std::unordered_map<std::string, float> myNodeFloatDataMap;
	std::unordered_map<std::string, int> myNodeIntDataMap;
	std::unordered_map<std::string, bool> myNodeBoolDataMap;
	std::unordered_map<std::string, bool> myNodeStartDataMap;
	static CNodeDataManager* ourInstance;
};