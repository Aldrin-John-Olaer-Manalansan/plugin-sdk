/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once
#include "PluginBase.h"
#include "RenderWare.h"
#include "CQuaternion.h"

class CMatrix {
public:
	// unused, but retained here as layout for the enum below
	// struct tMatrixEulerFlags {
	// 	bool swapXZ: 1;
	// 	bool isExtrinsic: 1; // false = intrinsic, true = extrinsic
	// 	bool swapYZ: 1;
	// 	unsigned char primaryAxisIndex: 2; // index (0, 1, 2) into byte_866D9C[] that selects primary axis/order
	// }; // { true,  false,  true, 2} = 0x15 = 21U: is Always used by the game
	enum eMatrixEulerFlags : unsigned char {
		SWAP_1ST_3RD_VALUES = 0x01, // The sequence is unaffected, but the Initial and Final values were swapped
		__SWAP_2ND_3RD_SEQ = 0x04, // no need to use this flag as it is already applied at the sequence flags stated below

		INTRINSIC = 0x0,
		EXTRINSIC = 0x2,

		// X = Pitch, Y = Roll, Z = Yaw
		SEQUENCE_XYZ = 0x00,
		SEQUENCE_YZX = 0x08,
		SEQUENCE_ZXY = 0x10,
		SEQUENCE_XZY = SEQUENCE_XYZ | __SWAP_2ND_3RD_SEQ,
		SEQUENCE_YXZ = SEQUENCE_YZX | __SWAP_2ND_3RD_SEQ,
		SEQUENCE_ZYX = SEQUENCE_ZXY | __SWAP_2ND_3RD_SEQ,

		DEFAULT = SEQUENCE_ZYX | SWAP_1ST_3RD_VALUES,
	};
	VALIDATE_SIZE(eMatrixEulerFlags, 1);

    // RwV3d-like:
    CVector      right; // x-axis
    unsigned int flags;
    CVector      up; // y-axis, the member name is supposed to be "forward"
    unsigned int pad1;
    CVector      at; // z-axis, the member name is supposed to be "up"
    unsigned int pad2;
    CVector      pos;
    unsigned int pad3;
    
	RwMatrix *m_pAttachMatrix;
	bool m_bOwnsAttachedMatrix; // do we need to delete attaching matrix at detaching

    inline CMatrix() {
        m_pAttachMatrix = nullptr;
        m_bOwnsAttachedMatrix = false;
    }

    CMatrix(plugin::dummy_func_t) {}
	CMatrix(CMatrix const& matrix);
	CMatrix(RwMatrix *matrix, bool temporary); // like previous + attach
	~CMatrix(); // destructor detaches matrix if attached 
	void Attach(RwMatrix *matrix, bool temporary);
	void Detach();
	void CopyOnlyMatrix(CMatrix const& matrix); // copy base RwMatrix to another matrix
	void CopyToRwMatrix(RwMatrix *matrix); // similar to UpdateRW(RwMatrixTag *)
	void ConvertToEulerAngles(float &initial, float &intermediate, float &final, eMatrixEulerFlags flags) const;
	void ConvertFromEulerAngles(float initial, float intermediate, float final, eMatrixEulerFlags flags);
	void ForceUpVector(CVector const &vecUp);
	void ForceUpVector(float x, float y, float z);
	void ResetOrientation();
    void Scale(float scale);
    void Scale(float x, float y, float z);
	void SetScale(float scale); // set (scaled)
	void SetScale(CVector const &scale);
	void SetScale(float right, float forward, float up); // set (scaled)
	void SetUnity();
	void SetTranslateOnly(CVector const &pos);
	void SetTranslateOnly(float x, float y, float z);
	void SetTranslate(CVector const &pos);
	void SetTranslate(float x, float y, float z); // like previous + reset orientation
	void SetRotateXOnly(float pitch);
	void SetRotateYOnly(float roll);
	void SetRotateZOnly(float yaw);
	void SetRotateOnly(CVector const &rotation);
	void SetRotateOnly(float pitch, float roll, float yaw); // sets the rotation on 3 axes
	void SetRotateX(float pitch);
	void SetRotateY(float roll);
	void SetRotateZ(float yaw);
	void SetRotate(CVector const &rotation);
	void SetRotate(float pitch, float roll, float yaw); // sets the rotation on 3 axes + resets the position to origin(0, 0, 0)
	void SetRotate(CQuaternion  const& quat);
	void RotateX(float pitch, bool keepPos = false);
	void RotateY(float roll, bool keepPos = false);
	void RotateZ(float yaw, bool keepPos = false);
	void Rotate(CVector const &rotation, bool keepPos = false);
	// This function is the straightforward version of:
	// this->ConvertToEulerAngles(yaw, roll, pitch, CMatrix::INTRINSIC | CMatrix::SEQUENCE_ZYX)
	void Rotate(float pitch, float roll, float yaw, bool keepPos = false);

	CVector MultiplyRotation(CVector const &direction) const;
	CVector MultiplyTransposedRotation(CVector const& direction) const;
	void Translate(CVector const &offset);
	void Translate(float x, float y, float z); // move the position
	void Reorthogonalise();
	void Update(); // update RwMatrix with attaching matrix. This doesn't check if attaching matrix is present, so use it only if you know it is present.
	               // Using UpdateRW() is more safe since it perform this check.
	void UpdateRW(); // update RwMatrix with attaching matrix.
	void UpdateRW(RwMatrix *matrix); // update RwMatrix with this matrix
	void UpdateMatrix(RwMatrix* rwMatrix);
	void operator=(CMatrix const& right);
	void operator+=(CMatrix const& right);
	void operator*=(CMatrix const& right);

	CVector& GetRight() { return right; }
	const CVector& GetRight() const { return right; }

	CVector& GetForward() { return up; }
	const CVector& GetForward() const { return up; }

	CVector& GetUp() { return at; }
	const CVector& GetUp() const { return at; }

	CVector& GetPosition() { return pos; }
	const CVector& GetPosition() const { return pos; }
};

VALIDATE_SIZE(CMatrix, 0x48);

CMatrix operator*(CMatrix const&a, CMatrix const&b);
CVector operator*(CMatrix const&a, CVector const&b);
CMatrix operator+(CMatrix const&a, CMatrix const&b);