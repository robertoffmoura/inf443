#pragma once

struct gui_scene_structure
{
    bool wireframe;

    float height = 0.2f;
    float scaling = 6.0f;
    int octave = 7;
    float persistency = 0.5f;

	bool trajectory = false;
};