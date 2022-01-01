#version 330

// 4 vertices per-primitive -- 2 for the line (1,2) and 2 for adjacency (0,3)
layout (lines) in;

// Standard fare for drawing lines
layout (line_strip, max_vertices = 2) out;

void main (void) {

  gl_Position = gl_in[0].gl_Position; // First vertex in the line
  EmitVertex();

  gl_Position = gl_in[1].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);; // Second vertex in the line
  EmitVertex();

  EndPrimitive();
}