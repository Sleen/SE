#pragma once

#include <stack>

#include "Macro.h"
#include "Control.h"

SE_BEGIN

class Scene : public Container{
private:
	stack<Scene*> scenes;

public:
	
};

SE_END
