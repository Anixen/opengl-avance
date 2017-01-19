#version 430

in vec3 vViewSpacePosition; // Position du vertex dans View
in vec3 vViewSpaceNormal; // Normale du vertex dans View
in vec3 vViewSpaceTangent; // Tangente du vertex dans View
in vec2 vTexCoords; // Coordonnées de texture

uniform vec3 uDirectionalLightDir;
uniform vec3 uDirectionalLightColor;
uniform float uDirectionalLightIntensity;

//uniform bool uEnablePointLights;
//uniform int uNbPointLights;
//layout(std430, binding = 4) buffer aPointLightPosition
//{
//    vec4 PointLightPositions[];
//};
//
//layout(std430, binding = 5) buffer aPointLightColor
//{
//    vec4 PointLightColors[];
//};
//
//layout(std430, binding = 6) buffer aPointLightIntensity
//{
//    float PointLightIntensities[];
//};
//
//layout(std430, binding = 7) buffer aPointLightEnabled
//{
//    int PointLightEnableds[];
//};

uniform vec3 uKa;
uniform sampler2D uKaSampler;
uniform bool uKaMap;

uniform vec3 uKd;
uniform sampler2D uKdSampler;
uniform bool uKdMap;

uniform vec3 uKs;
uniform sampler2D uKsSampler;
uniform bool uKsMap;

uniform float uNs;
uniform sampler2D uNsSampler;
uniform bool uNsMap;

uniform float ud;
uniform sampler2D udSampler;
uniform bool udMap;

uniform sampler2D uNormalSampler;
uniform bool uNormalMap;
vec3 normal;

layout(location = 0) out vec3 fPosition;
layout(location = 1) out vec3 fNormal;
layout(location = 2) out vec3 fAmbient;
layout(location = 3) out vec3 fDiffuse;
layout(location = 4) out vec4 fGlossyShininess;


vec3 bumpedNormal(vec4 normalTexel)
{
    vec3 normal = normalize(vViewSpaceNormal);
    vec3 tangent = normalize(vViewSpaceTangent);
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    vec3 bitangent = cross(tangent, normal);

    vec3 bumpMapNormal = 2.0 * normalTexel.rgb - vec3(1.0, 1.0, 1.0);
    mat3 TBN = mat3(tangent, bitangent, normal);

    return normalize(TBN * bumpMapNormal);
}

vec3 blinnPhong(vec3 Ka, vec3 Kd, vec3 Ks, float Ns, vec3 lightColor, float lightIntensity, vec3 lightDirection, float distance) {

    vec3 wi = normalize(lightDirection);
    vec3 w0 = normalize(-vViewSpacePosition);
    vec3 halfVector = normalize((wi + w0) / 2);
    vec3 N = normalize(normal);
    vec3 Li = lightColor * lightIntensity / (distance * distance);

    // Li * [(Kd * (wi.N) + Ks * (halfVector.N) ^ shininess)]

    return Li * ((Kd * max(0.f, dot(wi, N)) + Ks * pow(max(0.f, dot(halfVector, N)), Ns)));
}


void main()
{
    vec4 texelKa = vec4(1.f, 1.f, 1.f, 1.f);
    if( uKaMap) {
        texelKa = texture(uKaSampler, vTexCoords);
    }

    vec4 texelKd = vec4(1.f, 1.f, 1.f, 1.f);
    if( uKdMap) {
        texelKd = texture(uKdSampler, vTexCoords);
    }

    vec4 texelKs = vec4(1.f, 1.f, 1.f, 1.f);
    if( uKsMap) {
        texelKs = texture(uKsSampler, vTexCoords);
    }

    vec4 texelNs = vec4(1.f, 1.f, 1.f, 1.f);
    if( uNsMap) {
        texelNs = texture(uNsSampler, vTexCoords);
    }

    vec4 texeld = vec4(1.f, 1.f, 1.f, 1.f);
    if( udMap) {
        texeld = texture(udSampler, vTexCoords);
    }

    vec4 texelNormal = vec4(1.f, 1.f, 1.f, 1.f);
    normal = vViewSpaceNormal;
    if( uNormalMap) {
        texelNormal = texture(uNormalSampler, vTexCoords);
        normal = bumpedNormal(texelNormal);
    }

    vec3 Ka = uKa * texelKa.rgb;
    vec3 Kd = uKd * texelKd.rgb;
    vec3 Ks = uKs * texelKs.rgb;
    float Ns = uNs * texelNs.r;
    float d = ud * texeld.r;

    fPosition = vViewSpacePosition;
    //fNormal = normalize(vViewSpaceNormal);
    fNormal = normal;
    fAmbient = Ka;
    fDiffuse = Kd;
    fGlossyShininess = vec4(Ks, Ns);


    /*
    // Diffuse
    fColor = texelKd.rgb * uKd *
            ( uDirectionalLightColor * uDirectionalLightIntensity * max(0, dot(vViewSpaceNormal, uDirectionalLightDir))
            + uPointLightColor * uPointLightIntensity * max(0, dot(vViewSpaceNormal, dirToPointLight)) / (distToPointLight * distToPointLight));
    //*/

//    vec3 contribution = vec3(0, 0, 0);
//    contribution += blinnPhong(Ka, Kd, Ks, Ns, uDirectionalLightColor, uDirectionalLightIntensity, uDirectionalLightDir, 1);
//
//    if(uEnablePointLights)
//        for(int i = 0; i < uNbPointLights; ++i) {
//            if(0 == PointLightEnableds[i])
//                continue;
//            float distToPointLight = length(PointLightPositions[i].xyz - vViewSpacePosition);
//            vec3 dirToPointLight = (PointLightPositions[i].xyz - vViewSpacePosition) / distToPointLight;
//            contribution += blinnPhong(Ka, Kd, Ks, Ns, PointLightColors[i].rgb, PointLightIntensities[i], dirToPointLight, distToPointLight);
//        }


    //fColor = vec4(contribution, 1.f);

    //fColor = vec4(d, d, d, 1);
    //fColor = Ks.rgba;
    //fColor = vec4(normalize(vViewSpaceTangent), 1.f);
    //fColor = vec4(normalize(vViewSpaceNormal), 1.f);
    //fColor = texelNormal.rgba;
    //fColor = vec4(normal, 1.f);
}