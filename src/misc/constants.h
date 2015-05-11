#ifndef ONCGL_MISC_CONSTANTS_H
#define ONCGL_MISC_CONSTANTS_H

namespace oncgl {

#define RESOURCE_DIRS_PREFIX std::string("")

// Colors for lightsources etc.
#define COLOR_WHITE glm::vec3(1.0f, 1.0f, 1.0f)
#define COLOR_RED glm::vec3(1.0f, 0.0f, 0.0f)
#define COLOR_GREEN glm::vec3(0.0f, 1.0f, 0.0f)
#define COLOR_CYAN glm::vec3(0.0f, 1.0f, 1.0f)
#define COLOR_BLUE glm::vec3(0.0f, 0.0f, 1.0f)

// colors for terminal output
#define K_RED  "\x1B[31m"
#define K_GREEN  "\x1B[32m"
#define K_YELLOW  "\x1B[33m"
#define K_BLUE  "\x1B[34m"
#define K_MAGENTA  "\x1B[35m"
#define K_CYAN  "\x1B[36m"
#define K_WHITE "\x1B[37m"
#define K_RESET "\033[0m"

} // namespace oncgl

#endif // ONCGL_MISC_CONSTANTS_H
