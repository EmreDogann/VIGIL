#pragma once
#include <assimp/matrix3x3.h>
#include <assimp/matrix4x4.h>

namespace Assimp
{
    /// \brief
    static glm::mat4 ToGlm(const aiMatrix4x4 & from)
    {
        glm::mat4 to;
        //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
        to[0][0] = (float)from.a1; to[1][0] = (float)from.a2; to[2][0] = (float)from.a3; to[3][0] = (float)from.a4;
        to[0][1] = (float)from.b1; to[1][1] = (float)from.b2; to[2][1] = (float)from.b3; to[3][1] = (float)from.b4;
        to[0][2] = (float)from.c1; to[1][2] = (float)from.c2; to[2][2] = (float)from.c3; to[3][2] = (float)from.c4;
        to[0][3] = (float)from.d1; to[1][3] = (float)from.d2; to[2][3] = (float)from.d3; to[3][3] = (float)from.d4;
        return to;
    }
    
    /// \brief
    static glm::mat3 ToGlm(const aiMatrix3x3 & from)
    {
        glm::mat3 to;
        //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
        to[0][0] = (float)from.a1; to[1][0] = (float)from.a2; to[2][0] = (float)from.a3;
        to[0][1] = (float)from.b1; to[1][1] = (float)from.b2; to[2][1] = (float)from.b3;
        to[0][2] = (float)from.c1; to[1][2] = (float)from.c2; to[2][2] = (float)from.c3;
        //to[0][3] = 0.0f; to[1][3] = from.d2; to[2][3] = from.d3; 
        return to;
    }
}
