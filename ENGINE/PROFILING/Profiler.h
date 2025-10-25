#ifdef TRACY_ENABLE
#define PROFILE_SCOPE()
#define PROFILE_SCOPE_N(name)
#define PROFILE_FRAME()
#define PROFILE_GPU_SCOPE(name)
#else

#include "Tracy/Tracy.hpp"
#include "Tracy/TracyOpenGL.hpp"
#define PROFILE_SCOPE() ZoneScoped
#define PROFILE_SCOPE_N(name) ZoneScopedN(name)
#define PROFILE_FRAME() FrameMark
#define PROFILE_GPU_SCOPE(name) TracyGpuZone(name)
#endif
