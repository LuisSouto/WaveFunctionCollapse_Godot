## Intro
Godot wrapper for a Wave Function Collapse (WFC) implementation in C++ through GDExtension. Please check [WaveFunctionCollapse_CPP's README](https://github.com/LuisSouto/WaveFunctionCollapse_CPP#) and references therein for details about WFC.

## Usage
The implementation of the WFC algorithm is encapsulated in the WFC class. The list of methods and members is provided in the Godot documentation included in this repo. The most important one is the `Config` property, an instance of the `WFCConfig` resource. There you can specify settings like the size of the output image, periodicity conditions, pattern length, etc.

There are two examples showcasing who to use this class to generate procedurally generated levels. The first application is contained in the file *wfc_panel.tscn* inside the `project/scenes` folder. This scene pops up a window where you can draw on an empty canvas using the patterns extracted from the input sprite, and WFC will fill in the blanks when the autocomplete button is pressed. If the highlight grid option is selected, cells where a pattern can be drawn will be marked in yellow. This is to ensure the WFC constraints are respected. Notice that sometimes a cell will be marked yellow for a given pattern but upon left-clicking on the cell, the pattern will not be drawn. This is because WFC realizes after drawing the pattern that the adjacency constraints cannot be satisfied. In that case the cell will no longer be marked as yellow. You can erase cells that you have drawn with the right-click button on the top-left of the pattern. It is also possible to save the image in the Output folder. Below a short demo on how to use it.
<video src="https://github.com/user-attachments/assets/de5d7a4c-4e24-4f6b-857d-e7c9c58294af">

The second application is in the file *dual_grid_example.tscn*, which uses a simplified dual grid as input for WFC, and then maps the output to a more elaborate texture (please don't criticize the textures too much, I'm aware of my drawing limitations). Running this scene will pop up a 2D map with a panda on a sort of meadow, with Godot icons marking the entrance and exit. A path is forced from entrance to exit using a discrete brownian bridge, i.e. a random path between two points A and B. The rest of the map is generated using WFC. You can see this is my attempt at showcasing how WFC can be used for real levels, with an (almost) guaranteed path from entrance to exit, collision boxes so that the panda does not fall into the water, and so on. I highly recommend to look at the code to understand how the example works. If something is not clear please reach out. Below a short demo of this scene.
<video src="https://github.com/user-attachments/assets/9dfe673c-1454-4a61-a197-1f790e50e8e9">

The name dual grid in this example is a bit two-fold. The main reason is that I'm using a tileset of 16-tiles (with symmetry you can get away with less), which is explained for example [here](https://www.youtube.com/watch?v=jEWFSv3ivTg). The second reason is I have two tilesets: a simplified grid for WFC, and a prettier grid for the actual drawing. This is because WFC struggles when there's a lot of detail in the image. For example, a pond with several shades of blue will lead to more chaotic images than a pond that has one shade of blue, even though from a level perspective they're both ponds. So a solution is to send the simple version to WFC, and map a more detailed version to the output image.

## How to build
There is a CMake file provided that uses GCC to compile the library. As long as you have these dependencies installed (in particular C++20) you should be able to build the library as follows:
1. Open a terminal and cd to a location where you want to clone the repo.
2. Run the following commands:
```
git clone --recurse-submodules https://github.com/LuisSouto/WaveFunctionCollapse_Godot.git
cd WaveFunctionCollapse_Godot
cmake -B build/ -DCMAKE_BUILD_TYPE=Release
cmake --build build/
```
You can also use a debug build instead of release if you prefer. The Godot project is inside the `project` folder.

Please keep in mind that I only use Ubuntu and therefore I haven't tested compiling in other operating systems. If you encounter any problems I'm happy to look at the issue together.

## Submodules
This repository depends on two submodules: [godot-cpp](https://github.com/godotengine/godot-cpp) for the GDExtension funcionality, and [WaveFunctionCollapse_CPP](https://github.com/LuisSouto/WaveFunctionCollapse_CPP) which contains the actual WFC implementation.

## AI Disclosure
I made use of AI to help me configure the CMake file and to help me register a few properties in Godot. I still wrote the code myself in my own style though, no vibe coding involved.

Please check [WaveFunctionCollapse_CPP's AI Disclosure](https://github.com/LuisSouto/WaveFunctionCollapse_CPP#ai-disclosure), a submodule on which this repository depends on.

I mention this for transparency and because some people may want to use libraries that have been entirely generated by humans for their own projects. In that sense, even if this is largely a human generated project, I cannot say AI had zero involvement. 

## Assets
All assets in `project/assets` were made by me. As you can tell I'm no artist.

## License
Copyright (c) 2026 Luis Antonio Souto Arias

MIT License, see LICENSE.md for further details.
