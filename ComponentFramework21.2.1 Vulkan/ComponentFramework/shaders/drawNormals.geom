#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (triangles) in; /// bringing in triangles

layout (line_strip, max_vertices = 2) out; //bringing out lines

layout (std140, binding = 0) uniform CameraMatricies {
    mat4 projection;
    mat4 view;
};


layout (location = 0) in VertexStage { //pushed from the vert stage
    vec3 normal;
} vs_in[];



void main() {
    float length  = 0.01;
    for(int index = 0; index < 3; index++){
        //Its now also * projection, because we want the one point to be in NDC coords
        gl_Position = projection * gl_in[index].gl_Position; //glin[0].gl_Position is the first point in the triangle we set in the vert - its the array of the vertices of the triangle
        EmitVertex();
        //this point is just the length out along the normal from the other point
        gl_Position = projection * (gl_in[index].gl_Position + (vec4(vs_in[index].normal, 1.0) * length));
        EmitVertex();

        EndPrimitive();
    }
}