#pragma once
namespace CBYDefines
{
    const float frameRateCap = 900.0f;

    //Chunk size must be even
    const int ChunkSize = 64;
    const int BlockSize = 1;
    //Render distance in chunks
    const int RenderDistance = 4;
    //Distance before loaded chunks are deleted
    const int RenderDeleteDistance = 6;
    const int ChunkLoadsPerFrame = 1;
}
