#version 330

in vec3 vViewSpacePosition; // Position du vertex dans View
in vec3 vViewSpaceNormal; // Normale du vertex dans View
in vec2 vTexCoords; // Coordonnées de texture

uniform vec3 uDirectionalLightDir;
uniform vec3 uDirectionalLightColor;
uniform float uDirectionalLightIntensity;

uniform vec3 uPointLightPosition;
uniform vec3 uPointLightColor;
uniform float uPointLightIntensity;

uniform vec3 uKd;

uniform sampler2D uKdSampler;

out vec3 fColor;

void main()
{
   vec4 texel = texture(uKdSampler, vTexCoords);

   float distToPointLight = length(uPointLightPosition - vViewSpacePosition);
   vec3 dirToPointLight = (uPointLightPosition - vViewSpacePosition) / distToPointLight;

   //fColor = vViewSpaceNormal;
   fColor = texel.rgb * uKd *
            ( uDirectionalLightColor * uDirectionalLightIntensity * max(0, dot(vViewSpaceNormal, uDirectionalLightDir))
            + uPointLightColor * uPointLightIntensity * max(0, dot(vViewSpaceNormal, dirToPointLight)) / (distToPointLight * distToPointLight));
}