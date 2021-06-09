#pragma once
namespace CBYDefines
{
    const float frameRateCap = 900.0f;

    //Chunk size must be even
    const int ChunkSize = 16;
    const int BlockSize = 1;
    //Render distance in chunks
    const int RenderDistance = 6;
    //Distance before loaded chunks are deleted
    const int RenderDeleteDistance = 7;
    const int ChunkLoadsPerFrame = 1;
}
