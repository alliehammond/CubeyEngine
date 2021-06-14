#pragma once
namespace CBYDefines
{
    const int BlockSize = 1;
    //Render distance in chunks
    const int RenderDistance = 4;
    //Distance before loaded chunks are deleted
    const int RenderDeleteDistance = 6;
    const int ChunkLoadsPerFrame = 1;


    //************************************************************************************
    //**Region file data [Don't modify these - will invalidate all region files]
    const int ChunkDataByteBoundarys = 256;
    //Starting offset of chunk data based on file header size
    const int HeaderBoundaryOffset = 1;
    //Chunk size must be even
    const int ChunkSize = 64;
    //************************************************************************************
}
