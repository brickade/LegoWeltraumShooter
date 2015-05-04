#include "include/TheBrick/Conversion.h"
namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    ong::vec3 PuReToOng(PuRe_Vector3F a_Vector)
    {
        return ong::vec3(a_Vector.X, a_Vector.Y, a_Vector.Z);
    }

    // **************************************************************************
    // **************************************************************************
    ong::mat3x3 PuReToOng(PuRe_MatrixF a_Matrix)
    {
        return ong::mat3x3(PuReToOng(PuRe_MatrixF::GetRow(a_Matrix, 0)), PuReToOng(PuRe_MatrixF::GetRow(a_Matrix, 1)), PuReToOng(PuRe_MatrixF::GetRow(a_Matrix, 2)));
    }

    // **************************************************************************
    // **************************************************************************
    ong::Quaternion PuReToOng(PuRe_QuaternionF a_Quaternion)
    {
        return ong::Quaternion(ong::vec3(a_Quaternion.X, a_Quaternion.Y, a_Quaternion.Z), a_Quaternion.W);
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector3F OngToPuRe(ong::vec3 a_Vector)
    {
        return PuRe_Vector3F(a_Vector.x, a_Vector.y, a_Vector.z);
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_MatrixF OngToPuRe(ong::mat3x3 a_Matrix)
    {
        return PuRe_MatrixF(TheBrick::OngToPuRe(a_Matrix.r1), TheBrick::OngToPuRe(a_Matrix.r1), TheBrick::OngToPuRe(a_Matrix.r2));
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_QuaternionF OngToPuRe(ong::Quaternion a_Quaternion)
    {
        return PuRe_QuaternionF(TheBrick::OngToPuRe(a_Quaternion.v), a_Quaternion.w);
    }
}