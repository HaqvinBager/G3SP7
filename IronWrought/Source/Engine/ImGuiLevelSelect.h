#pragma once
class CImGuiLevelSelect
{
public:
	CImGuiLevelSelect();
	~CImGuiLevelSelect();

	void RenderWindow();

private:

	void LoadLevel(std::string aLevelPath);

	int mySelectedScene;

};

