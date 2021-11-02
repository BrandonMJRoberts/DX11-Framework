#ifndef _CLOUDS_H_
#define _CLOUDS_H_

class VolumetricClouds final
{
public:
	VolumetricClouds();
	~VolumetricClouds();

	void Render();
	void Update(const float deltaTime);

private:

};

#endif