#pragma once
#include "hasher.h"

class CGraphManager;

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
		EChildNodeData,
		ECount
	};

	struct SNodeData
	{
		std::string myNodeTypeName;
		void* myData;
		EDataType myDataType;
		bool myShouldSave;
	};

public:
	~CNodeDataManager();
	static void Create() { ourInstance = new CNodeDataManager(); }
	static CNodeDataManager* Get() { return ourInstance; }
	static std::string GetFolderPath() { return ourInstance->myCurrentFolderPath; }
	static void SetFolderPath(const std::string& aFolderPath) { ourInstance->myCurrentFolderPath = aFolderPath; }

public:
	template <typename T>
	T GetData(const std::string& aNodeTypeName)
	{
		size_t hash = Hasher::GetHashValue(aNodeTypeName);
		for (auto& data : myNodeData)
		{
			if (Hasher::GetHashValue(data.myNodeTypeName) == hash)
				return *(reinterpret_cast<T*>(data.myData));
			else
				hash;
		}
		return NULL;
	}

	template <typename T>
	void SetData(const std::string& aNodeTypeName, const EDataType& aDataType, const T& aValue, const bool& aWouldLikeToSave = true)
	{
		size_t hash = Hasher::GetHashValue(aNodeTypeName);

		for (size_t i = 0; i < myNodeData.size(); ++i)
		{
			if (Hasher::GetHashValue(myNodeData[i].myNodeTypeName) == hash)
			{
				*(reinterpret_cast<T*>(myNodeData[i].myData)) = aValue;
				return;
			}
		}

		myNodeData.push_back({ aNodeTypeName, new T(aValue), aDataType, aWouldLikeToSave });
	}

	void RemoveData(const std::string& aNodeTypeName)
	{
		if (myNodeData.size() > 0)
		{
			bool foundElement = false;
			int i;
			for (i = 0; i < myNodeData.size(); ++i)
			{
				if (myNodeData[i].myNodeTypeName == aNodeTypeName)
				{
					foundElement = true;
					break;
				}
			}

			if (foundElement)
			{
				for (int j = i; j < myNodeData.size() - 1; ++j)
					myNodeData[j] = myNodeData[j + 1];

				myNodeData.pop_back();
			}
		}
	}

public:
	void ClearStoredData();
	void RegisterNewDataNode(std::string aName, CGraphManager& aGraphManager);
	void SaveDataTypesToJson();

private:
	static CNodeDataManager* ourInstance;
	std::string myCurrentFolderPath;
	std::vector<SNodeData> myNodeData;
};