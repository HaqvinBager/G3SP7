#pragma once
#include "Window.h"
//#include "rapidjson/filewritestream.h"
#include "SerializerUtility.h"

namespace IronWroughtImGui {

	struct SVFXSerializable
	{
		std::string myName = "";

		//std::vector<CVFXBase*>	myVFXBases;
		//std::vector<Matrix>		myVFXTransforms;
		//std::vector<Matrix>		myVFXTransformsOriginal;
		std::vector<Vector3>	myVFXAngularSpeeds;
		std::vector<float>		myVFXDelays;
		std::vector<float>		myVFXDurations;
		std::vector<float>		myVFXBaseDelays;
		std::vector<float>		myVFXBaseDurations;
		std::vector<bool>		myVFXShouldOrbit;
		std::vector<bool>		myVFXIsActive;

		//std::vector<CParticleEmitter*> myParticleEmitters;
		//std::vector<Matrix>	 myEmitterTransforms;
		//std::vector<Matrix>	 myEmitterTransformsOriginal;
		std::vector<Vector3> myEmitterAngularSpeeds;
		//std::vector<std::vector<CParticleEmitter::SParticleVertex>> myParticleVertices;
		//std::vector<std::queue<CParticleEmitter::SParticleVertex>>	myParticlePools;
		std::vector<float> myEmitterDelays;
		std::vector<float> myEmitterDurations;
		std::vector<float> myEmitterBaseDelays;
		std::vector<float> myEmitterBaseDurations;
		std::vector<float> myEmitterTimers;
		std::vector<bool>  myEmitterShouldOrbit;

		std::vector<std::vector<Vector2>> myParticleSizeCurves;

		void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& aWriter)
		{
			aWriter.Key("VFXEffects");
			aWriter.StartArray();
			aWriter.StartObject();

			aWriter.Key("Name");
			aWriter.String(myName.c_str());

			aWriter.Key("VFXMeshes");
			aWriter.StartArray();
			aWriter.EndArray();

			aWriter.Key("ParticleSystems");
			aWriter.StartArray();
			aWriter.StartObject();

			aWriter.Key("Path");
			aWriter.String("Assets/Graphics/VFX/JSON/Data/ParticleData_Pull.json");
			aWriter.Key("Offset X");
			aWriter.Double(0.0);
			aWriter.Key("Offset Y");
			aWriter.Double(0.65);
			aWriter.Key("Offset Z");
			aWriter.Double(2.7);
			aWriter.Key("Rotation X");
			aWriter.Double(0.0);
			aWriter.Key("Rotation Y");
			aWriter.Double(0.0);
			aWriter.Key("Rotation Z");
			aWriter.Double(0.0);
			aWriter.Key("Scale");
			aWriter.Double(1.0);
			aWriter.Key("Rotate X per second");
			aWriter.Double(0.0);
			aWriter.Key("Rotate Y per second");
			aWriter.Double(0.0);
			aWriter.Key("Rotate Z per second");
			aWriter.Double(0.0);
			aWriter.Key("Orbit");
			aWriter.Bool(true);
			aWriter.Key("Delay");
			aWriter.Double(0.0);
			aWriter.Key("Duration");
			aWriter.Double(4.0);

			Serializer<std::vector<Vector2>> ser;
			ser.Serialize("SizeCurve", myParticleSizeCurves[0], aWriter);

			aWriter.EndObject();
			aWriter.EndArray();

			aWriter.EndObject();
			aWriter.EndArray(); // VFXEffects
		}
	};

	struct SParticleSystemData
	{
		std::vector<Vector2> mySizeCurve;

		void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& aWriter)
		{
			//aWriter.StartObject();
			aWriter.Key("SizeCurve");
			aWriter.StartArray();
			for (unsigned int i = 0; i < mySizeCurve.size(); ++i)
			{
				aWriter.StartObject();
				aWriter.Key("x");
				aWriter.Double(mySizeCurve[i].x);
				aWriter.Key("y");
				aWriter.Double(mySizeCurve[i].y);
				aWriter.EndObject();
			}
			aWriter.EndArray(static_cast<rapidjson::SizeType>(mySizeCurve.size()));
			//aWriter.EndObject(1);
		}
	};

	class CCurveWindow : public CWindow
	{
	public:
		CCurveWindow(const char* aName);
		~CCurveWindow() override;

		void OnEnable()	override;
		void OnInspectorGUI() override;
		void OnDisable() override;
		void SaveToFile();

	private:
		//void Save(const char* aJsonFile, const char* aVariableName, const std::vector<ImVec2>& somePoints);

		std::unordered_map<std::string, std::vector<Vector2>> myPointsMap;
	};
}

