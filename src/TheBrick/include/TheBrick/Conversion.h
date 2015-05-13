#ifndef _CONVERSION_H_
#define _CONVERSION_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <Onager/myMath.h>

namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    inline ong::vec3 PuReToOng(const PuRe_Vector3F& a_rVector)
    {
        return ong::vec3(a_rVector.X, a_rVector.Y, a_rVector.Z);
    }

    // **************************************************************************
    // **************************************************************************
    inline ong::mat3x3 PuReToOng(const PuRe_MatrixF& a_rMatrix)
    {
        return ong::mat3x3(PuReToOng(PuRe_MatrixF::GetRow(a_rMatrix, 0)), PuReToOng(PuRe_MatrixF::GetRow(a_rMatrix, 1)), PuReToOng(PuRe_MatrixF::GetRow(a_rMatrix, 2)));
    }

    // **************************************************************************
    // **************************************************************************
    inline ong::Quaternion PuReToOng(const PuRe_QuaternionF& a_rQuaternion)
    {
        return ong::Quaternion(ong::vec3(a_rQuaternion.X, a_rQuaternion.Y, a_rQuaternion.Z), a_rQuaternion.W);
    }

    // **************************************************************************
    // **************************************************************************
    inline PuRe_Vector3F OngToPuRe(const ong::vec3& a_rVector)
    {
        return PuRe_Vector3F(a_rVector.x, a_rVector.y, a_rVector.z);
    }

    // **************************************************************************
    // **************************************************************************
    inline PuRe_MatrixF OngToPuRe(const ong::mat3x3& a_rMatrix)
    {
        return PuRe_MatrixF(TheBrick::OngToPuRe(a_rMatrix.r1), TheBrick::OngToPuRe(a_rMatrix.r1), TheBrick::OngToPuRe(a_rMatrix.r2));
    }

    // **************************************************************************
    // **************************************************************************
    inline PuRe_QuaternionF OngToPuRe(const ong::Quaternion& a_rQuaternion)
    {
        return PuRe_QuaternionF(TheBrick::OngToPuRe(a_rQuaternion.v), a_rQuaternion.w);
    }
}

#endif /* _CONVERSION_H_ */