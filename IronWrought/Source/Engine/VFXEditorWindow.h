#pragma once
#include "Window.h"
#include "SerializerUtility.h"
#include "JsonReader.h"

typedef rapidjson::PrettyWriter<rapidjson::StringBuffer> rapidJsonWriter;

namespace IronWroughtImGui {

	enum class EVFXEditorMenu
	{
		MainMenu,
		VFXMeshView,
		ParticleEmitterView
	};

	struct SCurveData
	{
		SCurveData()
		{
			myPoints[0] = { 0.0f, 0.0f };
			myPoints[1] = { 0.33f, 0.3f };
			myPoints[2] = { 0.66f, 0.66f };
			myPoints[3] = { 1.0f, 1.0f };
			myShouldShowWindow = false;
			std::string defaultLabel = "Curve";
			ZeroMemory(myLabel, 32);
			memcpy(&myLabel[0], defaultLabel.c_str(), strlen(defaultLabel.c_str()));
		}

		SCurveData(SCurveData& anOther)
		{
			myPoints[0] = anOther.myPoints[0];
			myPoints[1] = anOther.myPoints[1];
			myPoints[2] = anOther.myPoints[2];
			myPoints[3] = anOther.myPoints[3];
			myShouldShowWindow = anOther.myShouldShowWindow;
			ZeroMemory(myLabel, 32);
			memcpy(&myLabel[0], anOther.myLabel, strlen(anOther.myLabel));
		}

		ImVec2 myPoints[4];
		char myLabel[32];
		bool myShouldShowWindow;
	};

	struct SVFXMeshData
	{
		Vector2 myScrollSpeed1;
		Vector2 myScrollSpeed2;
		Vector2 myScrollSpeed3;
		Vector2 myScrollSpeed4;
		float myUVScale1;
		float myUVScale2;
		float myUVScale3;
		float myUVScale4;
		float myOpacityStrength;
		char myTexturePath1[64];
		char myTexturePath2[64];
		char myTexturePath3[64];
		char myTextureMaskPath[64];
		char myMeshPath[64];

		void Init(const std::string& aPath)
		{
			rapidjson::Document document = CJsonReader::Get()->LoadDocument(ASSETPATH(aPath));
			ENGINE_BOOL_POPUP(!CJsonReader::HasParseError(document), "Invalid Json document: %s", aPath.c_str());

			myScrollSpeed1 = { document["Scroll Speed 1 X"].GetFloat(), document["Scroll Speed 1 Y"].GetFloat() };
			myScrollSpeed2 = { document["Scroll Speed 2 X"].GetFloat(), document["Scroll Speed 2 Y"].GetFloat() };
			myScrollSpeed3 = { document["Scroll Speed 3 X"].GetFloat(), document["Scroll Speed 3 Y"].GetFloat() };
			myScrollSpeed4 = { document["Scroll Speed 4 X"].GetFloat(), document["Scroll Speed 4 Y"].GetFloat() };
			myUVScale1 = document["UV Scale 1"].GetFloat();
			myUVScale2 = document["UV Scale 2"].GetFloat();
			myUVScale3 = document["UV Scale 3"].GetFloat();
			myUVScale4 = document["UV Scale 4"].GetFloat();
			myOpacityStrength = document["Opacity Strength"].GetFloat();

			ZeroMemory(myTexturePath1, 64);
			std::string path = document["Texture 1"].GetString();
			memcpy(&myTexturePath1[0], path.c_str(), strlen(path.c_str()));

			ZeroMemory(myTexturePath2, 64);
			path = document["Texture 2"].GetString();
			memcpy(&myTexturePath2[0], path.c_str(), strlen(path.c_str()));

			ZeroMemory(myTexturePath3, 64);
			path = document["Texture 3"].GetString();
			memcpy(&myTexturePath3[0], path.c_str(), strlen(path.c_str()));

			ZeroMemory(myTextureMaskPath, 64);
			path = document["Texture Mask"].GetString();
			memcpy(&myTextureMaskPath[0], path.c_str(), strlen(path.c_str()));

			ZeroMemory(myMeshPath, 64);
			path = document["Mesh Path"].GetString();
			memcpy(&myMeshPath[0], path.c_str(), strlen(path.c_str()));
		}

		void Serialize(rapidJsonWriter& aWriter)
		{
			aWriter.StartObject();

			aWriter.Key("Scroll Speed 1 X");
			aWriter.Double(myScrollSpeed1.x);
			aWriter.Key("Scroll Speed 1 Y");
			aWriter.Double(myScrollSpeed1.y);
			aWriter.Key("Scroll Speed 2 X");
			aWriter.Double(myScrollSpeed2.x);
			aWriter.Key("Scroll Speed 2 Y");
			aWriter.Double(myScrollSpeed2.y);
			aWriter.Key("Scroll Speed 3 X");
			aWriter.Double(myScrollSpeed3.x);
			aWriter.Key("Scroll Speed 3 Y");
			aWriter.Double(myScrollSpeed3.y);
			aWriter.Key("Scroll Speed 4 X");
			aWriter.Double(myScrollSpeed4.x);
			aWriter.Key("Scroll Speed 4 Y");
			aWriter.Double(myScrollSpeed4.y);

			aWriter.Key("UV Scale 1");
			aWriter.Double(myUVScale1);
			aWriter.Key("UV Scale 2");
			aWriter.Double(myUVScale2);
			aWriter.Key("UV Scale 3");
			aWriter.Double(myUVScale3);
			aWriter.Key("UV Scale 4");
			aWriter.Double(myUVScale4);

			aWriter.Key("Opacity Strength");
			aWriter.Double(myOpacityStrength);

			aWriter.Key("Texture 1");
			aWriter.String(myTexturePath1);
			aWriter.Key("Texture 2");
			aWriter.String(myTexturePath2);
			aWriter.Key("Texture 3");
			aWriter.String(myTexturePath3);
			aWriter.Key("Texture Mask");
			aWriter.String(myTextureMaskPath);
			aWriter.Key("Mesh Path");
			aWriter.String(myMeshPath);

			aWriter.EndObject();
		}
	};

	struct SParticleEmitterData
	{
		int myMaxNumberOfParticles;
		float mySpawnRate;
		float myLifetime;
		float myDefaultSpeed;
		Vector4 myStartColor;
		Vector4 myEndColor;
		float myUniformStartSize;
		float myUniformEndSize;
		Vector3 myStartDirection;
		Vector3 myEndDirection;
		float myLifetimeRandomizeFrom;
		float myLifetimeRandomizeTo;
		float mySpeedRandomizeFrom;
		float mySpeedRandomizeTo;
		Vector3 mySpawnPositionOffsetRandomizeFrom;
		Vector3 mySpawnPositionOffsetRandomizeTo;
		Vector3 mySpawnDirectionRandomizeFrom;
		Vector3 mySpawnDirectionRandomizeTo;
		char myTexturePath[64];

		SCurveData myColorCurve;
		SCurveData mySizeCurve;
		SCurveData myDirectionCurve;

		void Init(const std::string& aPath)
		{
			rapidjson::Document document = CJsonReader::Get()->LoadDocument(ASSETPATH(aPath));
			ENGINE_BOOL_POPUP(!CJsonReader::HasParseError(document), "Invalid Json document: %s", aPath.c_str());

			myMaxNumberOfParticles = document["Max Number Of Particles"].GetInt();

			mySpawnRate = document["Spawn Rate"].GetFloat();
			myLifetime = document["Lifetime"].GetFloat();
			myDefaultSpeed = document["Speed"].GetFloat();
			myStartColor = { document["Start Color R"].GetFloat(), document["Start Color G"].GetFloat(), document["Start Color B"].GetFloat(), document["Start Color A"].GetFloat() };
			myEndColor = { document["End Color R"].GetFloat(), document["End Color G"].GetFloat(), document["End Color B"].GetFloat(), document["End Color A"].GetFloat() };
			myUniformStartSize = document["Start Size"].GetFloat();
			myUniformEndSize = document["End Size"].GetFloat();
			myStartDirection = { document["Start Direction X"].GetFloat(), document["Start Direction Y"].GetFloat(), document["Start Direction Z"].GetFloat(), 0.0f };
			myEndDirection = { document["End Direction X"].GetFloat(), document["End Direction Y"].GetFloat(), document["End Direction Z"].GetFloat(), 0.0f };

			myLifetimeRandomizeFrom = document["Lifetime Minimum Offset"].GetFloat();
			myLifetimeRandomizeTo = document["Lifetime Maximum Offset"].GetFloat();
			mySpeedRandomizeFrom = document["Speed Minimum Offset"].GetFloat();
			mySpeedRandomizeTo = document["Speed Maximum Offset"].GetFloat();

			if (document.HasMember("Offset Spawn Position X Randomize From"))
			{
				mySpawnPositionOffsetRandomizeFrom = { document["Offset Spawn Position X Randomize From"].GetFloat(), document["Offset Spawn Position Y Randomize From"].GetFloat(), document["Offset Spawn Position Z Randomize From"].GetFloat() };
				mySpawnPositionOffsetRandomizeTo = { document["Offset Spawn Position X Randomize To"].GetFloat(), document["Offset Spawn Position Y Randomize To"].GetFloat(), document["Offset Spawn Position Z Randomize To"].GetFloat() };
			}

			mySpawnDirectionRandomizeFrom = { document["Spawn Direction X Randomize From"].GetFloat(), document["Spawn Direction Y Randomize From"].GetFloat(), document["Spawn Direction Z Randomize From"].GetFloat() };
			mySpawnDirectionRandomizeTo = { document["Spawn Direction X Randomize To"].GetFloat(), document["Spawn Direction Y Randomize To"].GetFloat(), document["Spawn Direction Z Randomize To"].GetFloat() };

			ZeroMemory(myTexturePath, 64);
			std::string path = document["Texture Path"].GetString();
			memcpy(&myTexturePath[0], path.c_str(), strlen(path.c_str()));

			if (document.HasMember("Color Curve"))
			{
				const auto& colorCurveArray = document["Color Curve"].GetArray();
				for (unsigned int i = 0; i < static_cast<unsigned int>(colorCurveArray.Size()); ++i)
				{
					myColorCurve.myPoints[i].x = colorCurveArray[i]["x"].GetFloat();
					myColorCurve.myPoints[i].y = colorCurveArray[i]["y"].GetFloat();
				}
				ZeroMemory(myColorCurve.myLabel, 32);
				std::string label = "Color Curve";
				memcpy(&myColorCurve.myLabel[0], label.c_str(), strlen(label.c_str()));
			}

			if (document.HasMember("Size Curve"))
			{
				const auto& sizeCurveArray = document["Size Curve"].GetArray();
				for (unsigned int i = 0; i < static_cast<unsigned int>(sizeCurveArray.Size()); ++i)
				{
					mySizeCurve.myPoints[i].x = sizeCurveArray[i]["x"].GetFloat();
					mySizeCurve.myPoints[i].y = sizeCurveArray[i]["y"].GetFloat();
				}
				ZeroMemory(mySizeCurve.myLabel, 32);
				std::string label = "Size Curve";
				memcpy(&mySizeCurve.myLabel[0], label.c_str(), strlen(label.c_str()));
			}

			if (document.HasMember("Direction Curve"))
			{
				const auto& directionCurveArray = document["Direction Curve"].GetArray();
				for (unsigned int i = 0; i < static_cast<unsigned int>(directionCurveArray.Size()); ++i)
				{
					myDirectionCurve.myPoints[i].x = directionCurveArray[i]["x"].GetFloat();
					myDirectionCurve.myPoints[i].y = directionCurveArray[i]["y"].GetFloat();
				}
				ZeroMemory(myDirectionCurve.myLabel, 32);
				std::string label = "Direction Curve";
				memcpy(&myDirectionCurve.myLabel[0], label.c_str(), strlen(label.c_str()));
			}
		}

		void Serialize(rapidJsonWriter& aWriter)
		{
			aWriter.StartObject();

			aWriter.Key("Max Number Of Particles");
			aWriter.Int(myMaxNumberOfParticles);
			aWriter.Key("Spawn Rate");
			aWriter.Double(mySpawnRate);
			aWriter.Key("Lifetime");
			aWriter.Double(myLifetime);
			aWriter.Key("Speed");
			aWriter.Double(myDefaultSpeed);

			aWriter.Key("Start Color R");
			aWriter.Double(myStartColor.x);
			aWriter.Key("Start Color G");
			aWriter.Double(myStartColor.y);
			aWriter.Key("Start Color B");
			aWriter.Double(myStartColor.z);
			aWriter.Key("Start Color A");
			aWriter.Double(myStartColor.w);

			aWriter.Key("End Color R");
			aWriter.Double(myEndColor.x);
			aWriter.Key("End Color G");
			aWriter.Double(myEndColor.y);
			aWriter.Key("End Color B");
			aWriter.Double(myEndColor.z);
			aWriter.Key("End Color A");
			aWriter.Double(myEndColor.w);

			aWriter.Key("Start Size");
			aWriter.Double(myUniformStartSize);
			aWriter.Key("End Size");
			aWriter.Double(myUniformEndSize);

			aWriter.Key("Start Direction X");
			aWriter.Double(myStartDirection.x);
			aWriter.Key("Start Direction Y");
			aWriter.Double(myStartDirection.y);
			aWriter.Key("Start Direction Z");
			aWriter.Double(myStartDirection.z);

			aWriter.Key("End Direction X");
			aWriter.Double(myEndDirection.x);
			aWriter.Key("End Direction Y");
			aWriter.Double(myEndDirection.y);
			aWriter.Key("End Direction Z");
			aWriter.Double(myEndDirection.z);

			aWriter.Key("Lifetime Minimum Offset");
			aWriter.Double(myLifetimeRandomizeFrom);
			aWriter.Key("Lifetime Maximum Offset");
			aWriter.Double(myLifetimeRandomizeTo);

			aWriter.Key("Speed Minimum Offset");
			aWriter.Double(mySpeedRandomizeFrom);
			aWriter.Key("Speed Maximum Offset");
			aWriter.Double(mySpeedRandomizeTo);

			aWriter.Key("Offset Spawn Position X Randomize From");
			aWriter.Double(mySpawnPositionOffsetRandomizeFrom.x);
			aWriter.Key("Offset Spawn Position X Randomize To");
			aWriter.Double(mySpawnPositionOffsetRandomizeTo.x);
			aWriter.Key("Offset Spawn Position Y Randomize From");
			aWriter.Double(mySpawnPositionOffsetRandomizeFrom.y);
			aWriter.Key("Offset Spawn Position Y Randomize To");
			aWriter.Double(mySpawnPositionOffsetRandomizeTo.y);
			aWriter.Key("Offset Spawn Position Z Randomize From");
			aWriter.Double(mySpawnPositionOffsetRandomizeFrom.z);
			aWriter.Key("Offset Spawn Position Z Randomize To");
			aWriter.Double(mySpawnPositionOffsetRandomizeTo.z);

			aWriter.Key("Spawn Direction X Randomize From");
			aWriter.Double(mySpawnDirectionRandomizeFrom.x);
			aWriter.Key("Spawn Direction X Randomize To");
			aWriter.Double(mySpawnDirectionRandomizeTo.x);
			aWriter.Key("Spawn Direction Y Randomize From");
			aWriter.Double(mySpawnDirectionRandomizeFrom.y);
			aWriter.Key("Spawn Direction Y Randomize To");
			aWriter.Double(mySpawnDirectionRandomizeTo.y);
			aWriter.Key("Spawn Direction Z Randomize From");
			aWriter.Double(mySpawnDirectionRandomizeFrom.z);
			aWriter.Key("Spawn Direction Z Randomize To");
			aWriter.Double(mySpawnDirectionRandomizeTo.z);

			aWriter.Key("Texture Path");
			aWriter.String(myTexturePath);

			Serializer<ImVec2*> curveSerializer;
			curveSerializer.Serialize("Color Curve", myColorCurve.myPoints, aWriter);
			curveSerializer.Serialize("Size Curve", mySizeCurve.myPoints, aWriter);
			curveSerializer.Serialize("Direction Curve", myDirectionCurve.myPoints, aWriter);

			aWriter.EndObject();
		}
	};

	struct SVFXMeshTransformData
	{
		char myPath[128] = "Assets/Graphics/VFX/JSON/Data/VFXMeshData_Ball.json";
		Vector3 myOffsetFromParent = { 0.0f, 0.0f, 0.0f };
		Vector3 myRotationAroundParent = { 0.0f, 0.0f, 0.0f };
		Vector3 myScale = { 1.0f, 1.0f, 1.0f };
		Vector3 myAngularSpeeds = { 0.0f, 0.0f, 0.0f };
		bool myShouldOrbit = true;
		float myDelay = 0.0f;
		float myDuration = 5.0f;

		void Serialize(rapidJsonWriter& aWriter)
		{
			aWriter.StartObject();

			aWriter.Key("Path");
			aWriter.String(myPath);
			aWriter.Key("Offset X");
			aWriter.Double(myOffsetFromParent.x);
			aWriter.Key("Offset Y");
			aWriter.Double(myOffsetFromParent.y);
			aWriter.Key("Offset Z");
			aWriter.Double(myOffsetFromParent.z);
			aWriter.Key("Rotation X");
			aWriter.Double(myRotationAroundParent.x);
			aWriter.Key("Rotation Y");
			aWriter.Double(myRotationAroundParent.y);
			aWriter.Key("Rotation Z");
			aWriter.Double(myRotationAroundParent.z);
			aWriter.Key("Scale X");
			aWriter.Double(myScale.x);
			aWriter.Key("Scale Y");
			aWriter.Double(myScale.y);
			aWriter.Key("Scale Z");
			aWriter.Double(myScale.z);
			aWriter.Key("Rotate X per second");
			aWriter.Double(myAngularSpeeds.x);
			aWriter.Key("Rotate Y per second");
			aWriter.Double(myAngularSpeeds.y);
			aWriter.Key("Rotate Z per second");
			aWriter.Double(myAngularSpeeds.z);
			aWriter.Key("Orbit");
			aWriter.Bool(myShouldOrbit);
			aWriter.Key("Delay");
			aWriter.Double(myDelay);
			aWriter.Key("Duration");
			aWriter.Double(myDuration);

			aWriter.EndObject();
		}
	};

	struct SParticleEmitterTransformData
	{
		char myPath[128] = "Assets/Graphics/VFX/JSON/Data/ParticleData_Pull.json";
		Vector3 myOffsetFromParent = { 0.0f, 0.0f, 0.0f };
		Vector3 myRotationAroundParent = { 0.0f, 0.0f, 0.0f };
		float myUniformScale = 1.0f;
		Vector3 myAngularSpeeds = { 0.0f, 0.0f, 0.0f };
		bool myShouldOrbit = true;
		float myDelay = 0.0f;
		float myDuration = 5.0f;

		void Serialize(rapidJsonWriter& aWriter)
		{
			aWriter.StartObject();

			aWriter.Key("Path");
			aWriter.String(myPath);
			aWriter.Key("Offset X");
			aWriter.Double(myOffsetFromParent.x);
			aWriter.Key("Offset Y");
			aWriter.Double(myOffsetFromParent.y);
			aWriter.Key("Offset Z");
			aWriter.Double(myOffsetFromParent.z);
			aWriter.Key("Rotation X");
			aWriter.Double(myRotationAroundParent.x);
			aWriter.Key("Rotation Y");
			aWriter.Double(myRotationAroundParent.y);
			aWriter.Key("Rotation Z");
			aWriter.Double(myRotationAroundParent.z);
			aWriter.Key("Scale");
			aWriter.Double(myUniformScale);
			aWriter.Key("Rotate X per second");
			aWriter.Double(myAngularSpeeds.x);
			aWriter.Key("Rotate Y per second");
			aWriter.Double(myAngularSpeeds.y);
			aWriter.Key("Rotate Z per second");
			aWriter.Double(myAngularSpeeds.z);
			aWriter.Key("Orbit");
			aWriter.Bool(myShouldOrbit);
			aWriter.Key("Delay");
			aWriter.Double(myDelay);
			aWriter.Key("Duration");
			aWriter.Double(myDuration);

			aWriter.EndObject();
		}
	};

	struct SVFXSerializable
	{
		char myName[64];

		std::vector<SVFXMeshTransformData> myVFXMeshes;
		std::vector<SParticleEmitterTransformData> myParticleEmitters;

		void SerializeEffects(rapidJsonWriter& aWriter)
		{
			aWriter.StartObject();

			aWriter.Key("Name");
			aWriter.String(myName);

			aWriter.Key("VFXMeshes");
			aWriter.StartArray();

			for (auto& mesh : myVFXMeshes)
			{
				mesh.Serialize(aWriter);
			}

			aWriter.EndArray();

			aWriter.Key("ParticleSystems");
			aWriter.StartArray();
			
			for (auto& emitter : myParticleEmitters)
			{
				emitter.Serialize(aWriter);
			}

			aWriter.EndArray();

			aWriter.EndObject();
		}
	};

	class CVFXEditorWindow : public CWindow
	{
	public:
		CVFXEditorWindow(const char* aName);
		~CVFXEditorWindow() override;

		void OnEnable()	override;
		void OnInspectorGUI() override;
		void OnDisable() override;

	private:
		void ShowMainMenu();
		void ShowParticleEmitterTransformData(SParticleEmitterTransformData& someData, unsigned int anIndex);
		void ShowVFXMeshTransformData(SVFXMeshTransformData& someData, unsigned int anIndex);
		void ShowVFXMeshWindow();
		void ShowParticleEffectWindow();
		
		void OpenVFXMeshWindow(const std::string& aPath);
		void OpenParticleEffectWindow(const std::string& aPath);
		
		void AddEffect(unsigned int anIndex);
		
		void SaveToFile();
		void LoadFile(std::string aFilePath);

	private:
		std::vector<SVFXSerializable> myEffects;
		std::vector<std::string> myEffectFilePaths;
		EVFXEditorMenu myCurrentMenu;
		SVFXMeshData* myMeshData;
		SParticleEmitterData* myEmitterData;
		SCurveData myCurrentCurveData;
		unsigned int myCurrentEffectIndex = 0;
		unsigned int myCurrentTabIndex = 0;
		unsigned int mySelectedIndex = 0;
		char mySaveDestination[256];
		char myVFXMeshDataDestination[256];
		char myParticleEmitterDataDestination[256];
	};
}

