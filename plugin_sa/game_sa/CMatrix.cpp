/*
    Plugin-SDK (Grand Theft Auto San Andreas) source file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "CMatrix.h"
#include "rwplcore.h"

#include <cstring>

CMatrix::CMatrix(CMatrix const& matrix) 
: m_pAttachMatrix(nullptr), m_bOwnsAttachedMatrix(false)
{
	// ((void (__thiscall *)(CMatrix *, CMatrix const&))0x59BCF0)(this, matrix);

	CopyOnlyMatrix(matrix);
}

// like previous + attach
CMatrix::CMatrix(RwMatrix *matrix, bool temporary) : m_pAttachMatrix(nullptr)
{
	// ((void (__thiscall *)(CMatrix *, RwMatrix *, bool))0x59C050)(this, matrix, temporary);

	Attach(matrix, temporary);
}

// destructor detaches matrix if attached
CMatrix::~CMatrix(void)
{
	// ((void (__thiscall *)(CMatrix *))0x59ACD0)(this);

	Detach();
}

void CMatrix::Attach(RwMatrix *matrix, bool temporary)
{
	// ((void (__thiscall *)(CMatrix *, RwMatrix *, bool))0x59BD10)(this, matrix, temporary);

	Detach();

	m_pAttachMatrix = matrix;
	m_bOwnsAttachedMatrix = temporary;

	Update();
}

void CMatrix::Detach(void)
{
	// ((void (__thiscall *)(CMatrix *))0x59ACF0)(this);

	if (m_bOwnsAttachedMatrix && m_pAttachMatrix) {
        RwMatrixDestroy(m_pAttachMatrix);
	}
    m_pAttachMatrix = nullptr;
}

// copy base RwMatrix to another matrix
void CMatrix::CopyOnlyMatrix(CMatrix const& matrix)
{
	// ((void (__thiscall *)(CMatrix *, CMatrix const&))0x59ADD0)(this, matrix);
	
	// *reinterpret_cast<RwMatrix*>(this) = reinterpret_cast<RwMatrix const&>(matrix);
	std::memcpy(this, &matrix, sizeof(RwMatrix));
}

// update RwMatrix with attaching matrix. This doesn't check if attaching matrix is present, so use it only if you know it is present.
// Using UpdateRW() is more safe since it perform this check.
void CMatrix::Update(void)
{
	// ((void (__thiscall *)(CMatrix *))0x59BB60)(this);

	UpdateMatrix(m_pAttachMatrix);
}

// update RwMatrix with attaching matrix.
void CMatrix::UpdateRW(void)
{
	// ((void (__thiscall *)(CMatrix *))0x59BBB0)(this);

	if (m_pAttachMatrix) {
    	CMatrix::UpdateRW(m_pAttachMatrix);
	}
}

// update RwMatrix with this matrix
void CMatrix::UpdateRW(RwMatrix *matrix)
{
	// ((void (__thiscall *)(CMatrix *, RwMatrix *))0x59AD70)(this, matrix);
	
    matrix->right = reinterpret_cast<RwV3d&>(GetRight());
    matrix->up = reinterpret_cast<RwV3d&>(GetForward());
    matrix->at = reinterpret_cast<RwV3d&>(GetUp());
    matrix->pos = reinterpret_cast<RwV3d&>(pos);

    RwMatrixUpdate(matrix);
}

void CMatrix::UpdateMatrix(RwMatrix* rwMatrix)
{
	// plugin::CallMethod<0x59AD20, CMatrix*, RwMatrix*>(this, rwMatrix);
	
	GetRight() = reinterpret_cast<CVector&>(rwMatrix->right);
	GetForward() = reinterpret_cast<CVector&>(rwMatrix->up);
	GetUp() = reinterpret_cast<CVector&>(rwMatrix->at);
    pos = reinterpret_cast<CVector&>(rwMatrix->pos);
}

void CMatrix::ResetOrientation(void)
{
	//((void (__thiscall *)(CMatrix *))0x59AEA0)(this);
	
    GetRight()  .Set(1.0f, 0.0f, 0.0f);
    GetForward().Set(0.0f, 1.0f, 0.0f);
    GetUp()     .Set(0.0f, 0.0f, 1.0f);
}

void CMatrix::SetUnity(void)
{
	//((void (__thiscall *)(CMatrix *))0x59AE70)(this);

	SetScale(1.0f);
}

// scale on three axes
void CMatrix::SetScale(float right, float forward, float up)
{
	//((void (__thiscall *)(CMatrix *, float, float, float))0x59AF00)(this, x, y, z);
	
    GetRight()	.Set(right,  0.0f,   0.0f);
    GetForward().Set( 0.0f,  forward,  0.0f);
    GetUp()		.Set( 0.0f,  0.0f,  up);
    
	pos.Set(0.0f);
}

void CMatrix::SetScale(CVector const &scale) {
	SetScale(scale.x, scale.y, scale.z);
}

void CMatrix::SetScale(float scale)
{
	//((void (__thiscall *)(CMatrix *, float))0x59AED0)(this, scale);

	SetScale(scale, scale, scale);
}

void CMatrix::SetTranslateOnly(float x, float y, float z)
{
	//((void (__thiscall *)(CMatrix *, float, float, float))0x59AF80)(this, x, y, z);

	pos.Set(x, y, z);
}

void CMatrix::SetTranslateOnly(CVector const &newPos) {
	//SetTranslateOnly(newPos.x, newPos.y, newPos.z);

	pos = newPos;
}

// like previous + reset orientation
void CMatrix::SetTranslate(float x, float y, float z)
{
	//((void (__thiscall *)(CMatrix *, float, float, float))0x59AF40)(this, x, y, z);

	ResetOrientation();
	pos.Set(x, y, z);
}

void CMatrix::SetTranslate(CVector const &newPos) {
	SetTranslate(newPos.x, newPos.y, newPos.z);
}

void CMatrix::SetRotateXOnly(float pitch)
{
	//((void (__thiscall *)(CMatrix *, float))0x59AFA0)(this, pitch);
	
    const float fSin = std::sin(pitch);
    const float fCos = std::cos(pitch);

    GetRight()  .Set(1.0f,  0.0f,  0.0f);
    GetForward().Set(0.0f,  fCos,  fSin);
    GetUp()     .Set(0.0f, -fSin,  fCos);
}

void CMatrix::SetRotateYOnly(float roll)
{
	//((void (__thiscall *)(CMatrix *, float))0x59AFE0)(this, roll);
	
    const float fSin = sin(roll);
    const float fCos = cos(roll);

    GetRight()  .Set(fCos,  0.0F, -fSin);
    GetForward().Set(0.0F,  1.0F,  0.0F);
    GetUp()     .Set(fSin,  0.0F,  fCos);
}

void CMatrix::SetRotateZOnly(float yaw)
{
	//((void (__thiscall *)(CMatrix *, float))0x59B020)(this, yaw);
	
    const float fSin = sin(yaw);
    const float fCos = cos(yaw);

    GetRight()  .Set( fCos, fSin, 0.0F);
    GetForward().Set(-fSin, fCos, 0.0F);
    GetUp()     .Set( 0.0F, 0.0F, 1.0F);
}

void CMatrix::SetRotateOnly(float pitch, float roll, float yaw)
{
	//((void (__thiscall *)(CMatrix *, float, float, float))0x59B120)(this, pitch, roll, yaw);
	
	// precompute trigo ratios
    const float sX = sin(pitch);
    const float cX = cos(pitch);
    const float sY = sin(roll);
    const float cY = cos(roll);
    const float sZ = sin(yaw);
    const float cZ = cos(yaw);

	// reuse composite terms
	const float sX_sY = sX * sY;
	const float sX_cY = sX * cY;
	const float cZ_cY = cZ * cY;
	const float sZ_cY = sZ * cY;
	const float cZ_sY = cZ * sY;
	const float sZ_sY = sZ * sY;
	const float sZ_cX = sZ * cX;
	const float cZ_cX = cZ * cX;

    GetRight()  .Set(cZ_cY - sZ*sX_sY, sZ_cY + cZ*sX_sY, -sY*cX);
	GetForward().Set(          -sZ_cX,            cZ_cX,     sX);
	GetUp()     .Set(cZ_sY + sZ*sX_cY, sZ_sY - cZ*sX_cY,  cY*cX);
}

void CMatrix::SetRotateOnly(CVector const &rotation) {
	SetRotateOnly(rotation.x, rotation.y, rotation.z);
}

void CMatrix::SetRotateX(float pitch)
{
	//((void (__thiscall *)(CMatrix *, float))0x59B060)(this, pitch);
	
    CMatrix::SetRotateXOnly(pitch);
    pos.Set(0.0f, 0.0f, 0.0f);
}

void CMatrix::SetRotateY(float roll)
{
	//((void (__thiscall *)(CMatrix *, float))0x59B0A0)(this, roll);
	
    CMatrix::SetRotateYOnly(roll);
    pos.Set(0.0f, 0.0f, 0.0f);
}

void CMatrix::SetRotateZ(float yaw)
{
	//((void (__thiscall *)(CMatrix *, float))0x59B0E0)(this, yaw);
	
    CMatrix::SetRotateZOnly(yaw);
    pos.Set(0.0f, 0.0f, 0.0f);
}

void CMatrix::SetRotate(float pitch, float roll, float yaw)
{
	//((void (__thiscall *)(CMatrix *, float, float, float))0x59B120)(this, pitch, roll, yaw);
	
	SetRotateOnly(pitch, roll, yaw);
    pos.Set(0.0f, 0.0f, 0.0f);
}

void CMatrix::SetRotate(CVector const &rotation) {
	SetRotate(rotation.x, rotation.y, rotation.z);
}

void CMatrix::SetRotate(CQuaternion  const& quat)
{
	//((void (__thiscall *)(CMatrix *, CQuaternion  const&))0x59BBF0)(this, quat);
    
    const CVector vecImag2 = quat.imag + quat.imag;

    const float x2x = vecImag2.x * quat.imag.x;
    const float y2x = vecImag2.y * quat.imag.x;
    const float z2x = vecImag2.z * quat.imag.x;

    const float y2y = vecImag2.y * quat.imag.y;
    const float z2y = vecImag2.z * quat.imag.y;
    const float z2z = vecImag2.z * quat.imag.z;

    const float x2r = vecImag2.x * quat.real;
    const float y2r = vecImag2.y * quat.real;
    const float z2r = vecImag2.z * quat.real;

    GetRight().Set  (1.0F - z2z - y2y,        z2r + y2x,        z2x - y2r);
    GetForward().Set(       y2x - z2r, 1.0F - z2z - x2x,        x2r + z2y);
    GetUp().Set     (       y2r + z2x,        z2y - x2r, 1.0F - y2y - x2x);
}

// column-vector convention
// converts local(relative) -> global(world)
CVector CMatrix::MultiplyRotation(CVector const &direction) const {
    // return plugin::CallAndReturn<CVector, 0x59C790, CMatrix  const&, CVector  const&>(matrix, vec);
    return direction.x * GetRight() + direction.y * GetForward() + direction.z * GetUp();
}

// row-vector convention
// converts global(world) -> local(relative)
CVector CMatrix::MultiplyTransposedRotation(CVector const& direction) const {
    //return plugin::CallAndReturn<CVector, 0x59C810, CVector  const&, CMatrix  const&>(direction, matrix);
    const CVector &right = GetRight();
	const CVector &forward = GetForward();
	const CVector &up = GetUp();
    return direction.x * CVector(right.x, forward.x, up.x)
         + direction.y * CVector(right.y, forward.y, up.y)
         + direction.z * CVector(right.z, forward.z, up.z);
}

void CMatrix::Translate(CVector const &offset) {
	pos += offset;
}

void CMatrix::Translate(float x, float y, float z)
{
	pos.x += x;
	pos.y += y;
	pos.z += z;
}

void CMatrix::RotateX(float pitch, bool keepPos)
{
	//((void (__thiscall *)(CMatrix *, float))0x59B1E0)(this, pitch);
	
	// x' = x
	// y' = c*y - s*z
	// z' = s*y + c*z
	
    const float c = std::cos(pitch);
    const float s = std::sin(pitch);

	CVector &right = GetRight();
	CVector &forward = GetForward();
	CVector &up = GetUp();

	auto rotate = [&](CVector& direction)
    {
        const float y = direction.y;
        const float z = direction.z;
        direction.y =  c * y - s * z;
        direction.z =  s * y + c * z;
    };

    rotate(right);
    rotate(up);
    rotate(at);

	if (!keepPos) {
		rotate(pos);
	}
}

void CMatrix::RotateY(float roll, bool keepPos)
{
	//((void (__thiscall *)(CMatrix *, float))0x59B2C0)(this, roll);

	// x' = s*z + c*x
	// y' = y
	// z' = c*z - s*x

    const float c = std::cos(roll);
    const float s = std::sin(roll);

    auto rotate = [&](CVector& v)
    {
        const float x = v.x;
        const float z = v.z;
        v.x = s * z + c * x;
        v.z = c * z - s * x;
    };

    rotate(right);
    rotate(up);
    rotate(at);
	
	if (!keepPos) {
		rotate(pos);
	}
}

void CMatrix::RotateZ(float yaw, bool keepPos)
{
	//((void (__thiscall *)(CMatrix *, float))0x59B390)(this, yaw);

	// x' =  c*x - s*y
	// y' =  s*x + c*y
	// z' =  z
	
    const float c = std::cos(yaw);
    const float s = std::sin(yaw);

    auto rotate = [&](CVector& v)
    {
        const float x = v.x;
        const float y = v.y;
        v.x =  c * x - s * y;
        v.y =  s * x + c * y;
    };

    rotate(right);
    rotate(up);
    rotate(at);
	
	if (!keepPos) {
		rotate(pos);
	}
}

// This function is the straightforward version of:
// this->ConvertToEulerAngles(yaw, roll, pitch, CMatrix::INTRINSIC | CMatrix::SEQUENCE_ZYX)
void CMatrix::Rotate(float pitch, float roll, float yaw, bool keepPos)
{
    // ((void (__thiscall *)(CMatrix *, float, float, float))0x59B460)(this, pitch, roll, yaw);

	const float cx = std::cos(pitch);
    const float sx = std::sin(pitch);
    const float cy = std::cos(roll);
    const float sy = std::sin(roll);
    const float cz = std::cos(yaw);
    const float sz = std::sin(yaw);

    // Rotation matrix rows (intrinsic ZYX)
    const CVector r0(
        cy * cz,
        cy * sz,
       -sy
    );

    const CVector r1(
        sx * sy * cz - cx * sz,
        sx * sy * sz + cx * cz,
        sx * cy
    );

    const CVector r2(
        cx * sy * cz + sx * sz,
        cx * sy * sz - sx * cz,
        cx * cy
    );

    auto rotate = [&](const CVector& v)
    {
        return CVector(
            r0.x * v.x + r0.y * v.y + r0.z * v.z,
            r1.x * v.x + r1.y * v.y + r1.z * v.z,
            r2.x * v.x + r2.y * v.y + r2.z * v.z
        );
    };

    right = rotate(right);
    up    = rotate(up);
    at    = rotate(at);
    
	if (!keepPos) {
        pos   = rotate(pos);
    }

}

void CMatrix::Rotate(CVector const &rotation, bool keepPos) {
	Rotate(rotation.x, rotation.y, rotation.z);
}

void CMatrix::Reorthogonalise()
{
	//((void (__thiscall *)(CMatrix *))0x59B6A0)(this);

    CVector &right = GetRight();
	CVector &forward = GetForward();
	CVector &up = GetUp();
    
    up = right.Cross(forward);
    up.Normalise();
    right = forward.Cross(up);
    right.Normalise();
    forward = up.Cross(right);
    //forward.Normalise(); // unecessary, guaranteed to be normalized
}

void CMatrix::CopyToRwMatrix(RwMatrix *matrix)
{
	//((void (__thiscall *)(CMatrix *, RwMatrix *))0x59B8B0)(this, matrix);
    
    UpdateRW(matrix);
}

void CMatrix::Scale(float x, float y, float z) {
    //plugin::CallMethod<0x459350, CMatrix *, float, float, float>(this, x, y, z);
    
    GetRight()   *= x;
    GetForward() *= y;
    GetUp()      *= z;
}

void CMatrix::Scale(float scale) {
    //plugin::CallMethod<0x459350, CMatrix *, float>(this, scale);

    Scale(scale, scale, scale);
}

void CMatrix::ForceUpVector(CVector const &vecUp) {
    //plugin::CallMethod<0x59B7E0, CMatrix *, CVector>(this, vecUp);

    CVector &right = GetRight();
	CVector &forward = GetForward();
	CVector &up = GetUp();

    right   = forward.Cross(vecUp);
    forward = vecUp.Cross(right);
    up      = vecUp;
}

void CMatrix::ForceUpVector(float x, float y, float z) {
    ForceUpVector(CVector(x, y, z));
}

void CMatrix::ConvertToEulerAngles(float &initial, float &intermediate, float &final, CMatrix::eMatrixEulerFlags flags) const {
	//plugin::CallMethod<0x59A840, const CMatrix*, float*, float*, float*, eMatrixEulerFlags>(this, &initial, &intermediate, &final, flags);

    constexpr float gimbalLockThreshold = 0.0000019073486f;
    float matrix[3][3];
    const CVector &right = GetRight();
	const CVector &forward = GetForward();
	const CVector &up = GetUp();

    matrix[0][0] = right.x;
    matrix[0][1] = right.y;
    matrix[0][2] = right.z;

    matrix[1][0] = forward.x;
    matrix[1][1] = forward.y;
    matrix[1][2] = forward.z;

    matrix[2][0] = up.x;
    matrix[2][1] = up.y;
    matrix[2][2] = up.z;

    const bool swap2ndAnd3rdSeq = (flags & __SWAP_2ND_3RD_SEQ) != 0;

    unsigned char idx1 = ((flags >> 3) & 0x3); // primaryAxisIndex
    if (idx1 >= 3) idx1 -= 3;
    unsigned char idx2 = (1 + idx1 + swap2ndAnd3rdSeq);
    if (idx2 >= 3) idx2 -= 3;
    unsigned char idx3 = (2 + idx1 - swap2ndAnd3rdSeq);
    if (idx3 >= 3) idx3 -= 3;

    if (flags & eMatrixEulerFlags::EXTRINSIC) {
        float r13 = matrix[idx1][idx3];
        float r12 = matrix[idx1][idx2];
        float cy = sqrt(r12 * r12 + r13 * r13);
        if (cy > gimbalLockThreshold) {
            initial      = atan2(                r12,                 r13);
            intermediate = atan2(                 cy,  matrix[idx1][idx1]);
            final        = atan2( matrix[idx2][idx3], -matrix[idx3][idx1]);
        } else {
            initial      = atan2(-matrix[idx2][idx3],  matrix[idx2][idx2]);
            intermediate = atan2(                 cy,  matrix[idx1][idx1]);
            final        = 0.0f;
        }
    } else {
        float r21 = matrix[idx2][idx1];
        float r11 = matrix[idx1][idx1];
        float cy = sqrt(r11 * r11 + r21 * r21);
        if (cy > gimbalLockThreshold) {
            initial      = atan2( matrix[idx3][idx2], matrix[idx3][idx3]);
            intermediate = atan2(-matrix[idx3][idx1],                 cy);
            final        = atan2(                r21,                r11);
        } else {
            initial      = atan2(-matrix[idx2][idx3], matrix[idx2][idx2]);
            intermediate = atan2(-matrix[idx3][idx1], cy);
            final        = 0.0f;
        }
    }

    if (flags & eMatrixEulerFlags::SWAP_1ST_3RD_VALUES) {
        std::swap(initial, final);
    }

    if (swap2ndAnd3rdSeq) {
        initial      *= -1.0f;
        intermediate *= -1.0f;
        final        *= -1.0f;
    }
}

void CMatrix::ConvertFromEulerAngles(float initial, float intermediate, float final, CMatrix::eMatrixEulerFlags flags) {
	//plugin::CallMethod<0x59AA40, CMatrix*, float, float, float, eMatrixEulerFlags>(this, initial, intermediate, final, flags);
    
    const bool swap2ndAnd3rdSeq = (flags & __SWAP_2ND_3RD_SEQ) != 0;

    unsigned char iInd1 = ((flags >> 3) & 0x3); // primaryAxisIndex
    if (iInd1 >= 3) iInd1 -= 3;
    unsigned char iInd2 = (1 + iInd1 + swap2ndAnd3rdSeq);
    if (iInd2 >= 3) iInd2 -= 3;
    unsigned char iInd3 = (2 + iInd1 - swap2ndAnd3rdSeq);
    if (iInd3 >= 3) iInd3 -= 3;

    float matrix[3][3];

    if (flags & eMatrixEulerFlags::SWAP_1ST_3RD_VALUES)
        std::swap(initial, final);

    if (swap2ndAnd3rdSeq) {
        initial      *= -1.0f;
        intermediate *= -1.0f;
        final        *= -1.0f;
    }

    const float fSinX = sin(initial);
    const float fCosX = cos(initial);
    const float fSinY = sin(intermediate);
    const float fCosY = cos(intermediate);
    const float fSinZ = sin(final);
    const float fCosZ = cos(final);

    if (flags & eMatrixEulerFlags::EXTRINSIC) {
        matrix[iInd1][iInd1] = fCosY;
        matrix[iInd1][iInd2] = fSinX*fSinY;
        matrix[iInd1][iInd3] = fCosX*fSinY;

        matrix[iInd2][iInd1] =   fSinY*fSinZ;
        matrix[iInd2][iInd2] =   fCosX*fCosY  - fCosY*fSinX*fSinZ;
        matrix[iInd2][iInd3] = -(fSinX*fCosZ) -(fCosX*fCosY*fSinZ);

        matrix[iInd3][iInd1] = -(fCosZ*fSinY);
        matrix[iInd3][iInd2] =   fCosX*fSinZ  + fCosY*fCosZ*fSinX;
        matrix[iInd3][iInd3] = -(fSinX*fSinZ) + fCosX*fCosY*fCosZ;
    } else {
        matrix[iInd1][iInd1] =   fCosY*fCosZ;
        matrix[iInd1][iInd2] = -(fCosX*fSinZ) + fCosZ*fSinX*fSinY;
        matrix[iInd1][iInd3] =   fSinX*fSinZ  + fCosX*fCosZ*fSinY;

        matrix[iInd2][iInd1] =   fCosY*fSinZ;
        matrix[iInd2][iInd2] =   fCosX*fCosZ  + fSinX*fSinY*fSinZ;
        matrix[iInd2][iInd3] = -(fCosZ*fSinX) + fCosX*fSinY*fSinZ;

        matrix[iInd3][iInd1] = -fSinY;
        matrix[iInd3][iInd2] =  fCosY*fSinX;
        matrix[iInd3][iInd3] =  fCosX*fCosY;
    }

    GetRight().Set  (matrix[0][0], matrix[0][1], matrix[0][2]);
    GetForward().Set(matrix[1][0], matrix[1][1], matrix[1][2]);
    GetUp().Set     (matrix[2][0], matrix[2][1], matrix[2][2]);
}

void CMatrix::operator=(CMatrix const& rvalue)
{
	//((void (__thiscall *)(CMatrix *, CMatrix const&))0x59BBC0)(this, rvalue);
    
    CopyOnlyMatrix(rvalue);
    UpdateRW();
}

void CMatrix::operator+=(CMatrix const& rvalue)
{
	//((void (__thiscall *)(CMatrix *, CMatrix const&))0x59ADF0)(this, rvalue);
    
    GetRight() += rvalue.GetRight();
    GetForward() += rvalue.GetForward();
    GetUp() += rvalue.GetUp();
    pos += rvalue.pos;
}

void CMatrix::operator*=(CMatrix const& rvalue)
{
	//((void (__thiscall *)(CMatrix *, CMatrix const&))0x411A80)(this, rvalue);
    
    *this = (*this * rvalue);
}

CMatrix operator*(CMatrix const&a, CMatrix const&b) {
    CMatrix result = CMatrix();

    //((void(__cdecl *)(CMatrix*, CMatrix const&, CMatrix const&))0x59BE30)(&result, a, b);
    
    CVector const &a_right   = a.GetRight();
	CVector const &a_forward = a.GetForward();
	CVector const &a_up      = a.GetUp();
    
    CVector const &b_right   = b.GetRight();
	CVector const &b_forward = b.GetForward();
	CVector const &b_up      = b.GetUp();

    result.GetRight()   = a_right * b_right.x   + a_forward * b_right.y   + a_up * b_right.z;
    result.GetForward() = a_right * b_forward.x + a_forward * b_forward.y + a_up * b_forward.z;
    result.GetUp()      = a_right * b_up.x      + a_forward * b_up.y      + a_up * b_up.z;
    result.pos          = a_right * b.pos.x     + a_forward * b.pos.y     + a_up * b.pos.z + a.pos;

    return result;
}

CVector operator*(CMatrix const&a, CVector const&b) {
    //CVector result;
    //((void(__cdecl *)(CVector*, CMatrix const&, CVector const&))0x59C890)(&result, a, b);
    //return result;

    return a.MultiplyRotation(b) + a.pos;
}

CMatrix operator+(CMatrix const&a, CMatrix const&b) {
    CMatrix result;
    //((void(__cdecl *)(CMatrix*, CMatrix const&, CMatrix const&))0x59BFA0)(&result, a, b);
    
    result.GetRight()   = a.GetRight()   + b.GetRight();
    result.GetForward() = a.GetForward() + b.GetForward();
    result.GetUp()      = a.GetUp()      + b.GetUp();
    result.pos          = a.pos          + b.pos;

    return result;


}