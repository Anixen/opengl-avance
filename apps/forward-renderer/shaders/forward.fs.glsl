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


out vec4 fColor;


vec3 blinnPhong(vec3 Ka, vec3 Kd, vec3 Ks, float Ns, vec3 lightColor, float lightIntensity, vec3 lightDirection, float distance) {

    vec3 wi = normalize(lightDirection);
    vec3 w0 = normalize(-vViewSpacePosition);
    vec3 halfVector = normalize((wi + w0) / 2);
    vec3 N = normalize(vViewSpaceNormal);
    vec3 Li = lightColor * lightIntensity / distance * distance;

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

    float distToPointLight = length(uPointLightPosition - vViewSpacePosition);
    vec3 dirToPointLight = (uPointLightPosition - vViewSpacePosition) / distToPointLight;

    vec3 Ka = uKa * texelKa.rgb;
    vec3 Kd = uKd * texelKd.rgb;
    vec3 Ks = uKs * texelKs.rgb;
    float Ns = uNs * texelNs.r;
    float d = ud * texeld.r;

    /*
    // Diffuse
    fColor = texelKd.rgb * uKd *
            ( uDirectionalLightColor * uDirectionalLightIntensity * max(0, dot(vViewSpaceNormal, uDirectionalLightDir))
            + uPointLightColor * uPointLightIntensity * max(0, dot(vViewSpaceNormal, dirToPointLight)) / (distToPointLight * distToPointLight));
    //*/

    fColor = vec4(
        blinnPhong(Ka, Kd, Ks, Ns, uPointLightColor, uPointLightIntensity, dirToPointLight, distToPointLight)
      + blinnPhong(Ka, Kd, Ks, Ns, uDirectionalLightColor, uDirectionalLightIntensity, uDirectionalLightDir, 1),
       1.f);

    //fColor = vec4(d, d, d, 1);

    //fColor = Ks.rgb;
    //fColor = vViewSpaceNormal;
}