#pragma once
#include "Window.h"
#include "SerializerUtility.h"

namespace IronWroughtImGui {

	struct STabWindow
	{
		std::string myName = "";
		unsigned int myTabIndex = 0;
	};

	struct SVFXSerializable
	{
		char myName[64];

		//std::vector<CVFXBase*>	myVFXBases;
		//std::vector<Matrix>		myVFXTransforms;
		//std::vector<Matrix>		myVFXTransformsOriginal;
		std::vector<Vector3>	myVFXParentOffsets;
		std::vector<Vector3>	myVFXRotationAroundParent;
		std::vector<float>		myVFXScale;
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
		std::vector<Vector3> myEmitterParentOffsets;
		std::vector<Vector3> myEmitterRotationsAroundParent;
		std::vector<float>	 myEmitterScale;
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
			aWriter.StartObject();

			aWriter.Key("Name");
			aWriter.String(myName);

			aWriter.Key("VFXMeshes");
			aWriter.StartArray();
			aWriter.EndArray();

			aWriter.Key("ParticleSystems");
			aWriter.StartArray();
			aWriter.StartObject();

			aWriter.Key("Path");
			aWriter.String("Assets/Graphics/VFX/JSON/Data/ParticleData_Pull.json");
			aWriter.Key("Offset X");
			aWriter.Double(myEmitterParentOffsets[0].x);
			aWriter.Key("Offset Y");
			aWriter.Double(myEmitterParentOffsets[0].y);
			aWriter.Key("Offset Z");
			aWriter.Double(myEmitterParentOffsets[0].z);
			aWriter.Key("Rotation X");
			aWriter.Double(myEmitterRotationsAroundParent[0].x);
			aWriter.Key("Rotation Y");
			aWriter.Double(myEmitterRotationsAroundParent[0].y);
			aWriter.Key("Rotation Z");
			aWriter.Double(myEmitterRotationsAroundParent[0].z);
			aWriter.Key("Scale");
			aWriter.Double(myEmitterScale[0]);
			aWriter.Key("Rotate X per second");
			aWriter.Double(myEmitterAngularSpeeds[0].x);
			aWriter.Key("Rotate Y per second");
			aWriter.Double(myEmitterAngularSpeeds[0].x);
			aWriter.Key("Rotate Z per second");
			aWriter.Double(myEmitterAngularSpeeds[0].x);
			aWriter.Key("Orbit");
			aWriter.Bool(myEmitterShouldOrbit[0]);
			aWriter.Key("Delay");
			aWriter.Double(myEmitterBaseDelays[0]);
			aWriter.Key("Duration");
			aWriter.Double(myEmitterDurations[0]);

			Serializer<std::vector<Vector2>> ser;
			ser.Serialize("SizeCurve", myParticleSizeCurves[0], aWriter);

			aWriter.EndObject();
			aWriter.EndArray();

			aWriter.EndObject();
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

		std::unordered_map<std::string, std::vector<Vector2>> myPointsMap;
		std::vector<SVFXSerializable> myEffects;
		unsigned int myCurrentEffectIndex = 0;
		unsigned int myCurrentTabIndex = 0;
		float myColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
		char myCurrentEffectNameBuffer[64];
	};
}

