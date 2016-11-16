#version 330 core

// unnecessary?
layout(location = 0) out float fragmentdepth;

void main() {
    // noop, // unnecessary?
    fragmentdepth = gl_FragCoord.z;
} 