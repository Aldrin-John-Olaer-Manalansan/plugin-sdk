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
	enum class e_EulerAngleType : unsigned char {
		TaitBryan,     // three distinct axes (yaw/pitch/roll style)
		ProperEuler    // repeated axis (e.g. ZXZ, XYX)
	};

	struct t_EulerAngleConversionFlags {
		unsigned char swapXAndZ: 1 = false;
		unsigned char angleType: 1 = false; // see e_EulerAngleType
		unsigned char isFlipped: 1 = false; // if set negate all three angles
		unsigned char primaryAxisIndex: 2 = 2; // index (0, 1, 2) into byte_866D9C[] that selects primary axis/order
		/*
		 * Tested Combinations:
		 * { true,  true,  true, 1} = 0x0F: Always used by the game
		 * {false, false, false, 2} = 0x10: Returns Relative Angles(x=yaw, y=pitch, z=roll) without negation(less CPU Cycles)
		 * {false, false,  true, 2} = 0x14: Returns Relative Angles(x=yaw, y=pitch, z=roll) that matches the angles returned by Native Commands
		 */
	};

	VALIDATE_SIZE(t_EulerAngleConversionFlags, 1);

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
	void Update(); // update RwMatrix with attaching matrix. This doesn't check if attaching matrix is present, so use it only if you know it is present.
	               // Using UpdateRW() is more safe since it perform this check.
	void UpdateRW(); // update RwMatrix with attaching matrix.
	void UpdateRW(RwMatrix *matrix); // update RwMatrix with this matrix
	void SetUnity();
	void ResetOrientation();
	void SetScale(float scale); // set (scaled)
	void SetScale(CVector const &scale);
	void SetScale(float x, float y, float z); // set (scaled)
	void SetTranslateOnly(CVector const &pos);
	void SetTranslateOnly(float x, float y, float z);
	void SetTranslate(CVector const &pos);
	void SetTranslate(float x, float y, float z); // like previous + reset orientation
	void SetRotateXOnly(float pitch);
	void SetRotateYOnly(float roll);
	void SetRotateZOnly(float yaw);
	void SetRotateX(float pitch);
	void SetRotateY(float roll);
	void SetRotateZ(float yaw);
	void SetRotate(CVector const &rotation);
	void SetRotate(float pitch, float roll, float yaw); // sets the rotation on 3 axes + resets the position to origin(0, 0, 0)
	void RotateX(float pitch);
	void RotateY(float roll);
	void RotateZ(float yaw);
	void Rotate(CVector const &rotation);
	void Rotate(float pitch, float roll, float yaw); // rotate on 3 axes
	void ConvertToEulerAngles(float &x, float &y, float &z, CMatrix::t_EulerAngleConversionFlags flags = {}) const;
	CVector ConvertToEulerAngles(t_EulerAngleConversionFlags flags = {}) const;
	void ConvertFromEulerAngles(CVector const &rotation, t_EulerAngleConversionFlags flags = {});
	void ConvertFromEulerAngles(float x, float y, float z, t_EulerAngleConversionFlags flags = {});
	void Translate(CVector const &offset);
	void Translate(float x, float y, float z); // move the position
	void Reorthogonalise();
	void CopyToRwMatrix(RwMatrix *matrix); // similar to UpdateRW(RwMatrixTag *)
	void SetRotate(CQuaternion  const& quat);
    void Scale(float scale);
    void Scale(float x, float y, float z);
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