#version 430

layout (triangles) in;
layout (line_strip) out;
layout (max_vertices = 2) out;

out vec3 fragmentColor;

// Uniforms to hold the model-view-projection and model-view matrix
uniform mat4 MVP;
uniform mat4 MV;

// Uniform to store the length of the visualized normals
uniform float normal_length;

vec3 ab = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
vec3 ac = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
vec3 face_normal = normalize(cross(ab, ac));

vec4 tri_centroid = (gl_in[0].gl_Position +
                     gl_in[1].gl_Position +
                     gl_in[2].gl_Position) / 3.0;

void main(){
    gl_Position = MVP * tri_centroid;
    fragmentColor = vec3(1.0,0.0,0.0);
    EmitVertex();

    gl_Position = MVP * (tri_centroid + vec4(face_normal * normal_length,0.0));
    fragmentColor = vec3(1.0,0.0,0.0);
    EmitVertex();
    EndPrimitive();
}
