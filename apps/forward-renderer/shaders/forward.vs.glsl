#version 430

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

// Matrices de transformation
uniform mat4 uModelViewProjMatrix; // Projection de la position vers l'ecran ?
uniform mat4 uModelViewMatrix; // Tranformation de la position vers l'espace View
uniform mat4 uNormalMatrix; // Tranformation de la normale vers l'espace View

out vec3 vViewSpacePosition; // Position du vertex dans View
out vec3 vViewSpaceNormal; // Normale du vertex dans View
out vec2 vTexCoords; // Coordonnées de texture

void main() {

	// Convertir la position et normale du vertex en coordonees homogenes
    vec4 hPosition = vec4(aPosition, 1); // Position -> Cordonnee homogène à 1
    vec4 hNormal = vec4(aNormal, 0); // Position -> Cordonnee homogène à 0

	vViewSpacePosition = vec3(uModelViewMatrix * hPosition);
	vViewSpaceNormal = vec3(uNormalMatrix * hNormal);
	vTexCoords = aTexCoords;

	gl_Position = uModelViewProjMatrix * hPosition;
 }