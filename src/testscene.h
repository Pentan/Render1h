#ifndef R1H_TESTSCENE_H
#define R1H_TESTSCENE_H

#include "scene.h"

namespace r1h {
void setupEduptScene(Scene &scene, const double aspect);
void setupTestObjScene(Scene &scene, const double aspect);
void setupCornelBoxObjScene(Scene &scene, const double aspect);
void setupAABBTestScene(Scene &scene, const double aspect);
void setupManyTrisObjScene(Scene &scene, const double aspect);
}

#endif