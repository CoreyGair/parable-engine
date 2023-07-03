#version 450

layout(set = 0, binding = 0) uniform PerFrameUniformBufferObject {
    mat4 view;
    mat4 projection;
} frame;

layout( push_constant ) uniform constants
{
	mat4 model;
} pushConstants;


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = frame.projection * frame.view * pushConstants.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}