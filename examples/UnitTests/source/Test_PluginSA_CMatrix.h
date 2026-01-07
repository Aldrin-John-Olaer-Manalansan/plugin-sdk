#pragma once
#include <plugin.h>
#include "utest.h"
#include <CMatrix.h>

using namespace plugin;

constexpr int32_t cfloatingPointAcceptableULP = 6;

static void PrintBytes(const void* const buffer, size_t size) {
    const uint8_t* const bytes = (uint8_t*)buffer;
    uint8_t pad = 0;
    for (size_t i = 0; i < size; i++) {
        printf("0x%.2X, ", bytes[i]);
        pad++;
        if (pad >= 16) {
            printf("\n");
            pad = 0;
        }
    }
    printf("\n");
}

static void RandomData(void* data, size_t size) {
    unsigned char *ptr = reinterpret_cast<unsigned char *>(data);
	for (size_t i = 0; i < size; i++) {
		ptr[i] = rand() % 256;
	}
}

static float RandomFloat(float min, float max) {
    // Generate a random float between 0.0 and 1.0
    float scale = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    // Scale and shift to the desired range [min, max]
    return min + scale * (max - min); 
}

// "Unit in the last place(ULP)" for testing equality of floats
static inline int32_t FloatToOrderedInt(float f) {
    int32_t i;
    std::memcpy(&i, &f, sizeof(float));

    // Make lexicographically ordered as twos-complement integer
    if (i < 0)
        i = 0x80000000 - i;

    return i;
}

static bool AlmostEqualULP(float a, float b, int maxUlps) {
    // Handle NaN
    if (std::isnan(a) || std::isnan(b))
        return false;

    // Handle infinities
    if (std::isinf(a) || std::isinf(b))
        return a == b;

    int32_t ia = FloatToOrderedInt(a);
    int32_t ib = FloatToOrderedInt(b);

    if (std::abs(ia - ib) > maxUlps) {
        printf("threshold reached %d\n", std::abs(ia - ib));
    }

    return std::abs(ia - ib) <= maxUlps;
}
//////////////////////////

static bool IsFloatApproximatelyEqual(float a, float b) {
    return AlmostEqualULP(a, b, cfloatingPointAcceptableULP);
}

static bool IsVectorApproximatelyEqual(CVector &a, CVector &b) {
    return IsFloatApproximatelyEqual(a.x, b.x)
        && IsFloatApproximatelyEqual(a.y, b.y)
        && IsFloatApproximatelyEqual(a.z, b.z);
}

static bool IsMatrixApproximatelyEqual(CMatrix &a, CMatrix &b) {
    return a.m_pAttachMatrix == b.m_pAttachMatrix
        && a.m_bOwnsAttachedMatrix == b.m_bOwnsAttachedMatrix
        && IsVectorApproximatelyEqual(a.GetRight(), b.GetRight())
        && IsVectorApproximatelyEqual(a.GetForward(), b.GetForward())
        && IsVectorApproximatelyEqual(a.GetUp(), b.GetUp())
        && IsVectorApproximatelyEqual(a.pos, b.pos);
}

static void RandomizeComponents(CMatrix &matrix) {
    matrix.GetRight().x = RandomFloat(-1.0f, 1.0f);
    matrix.GetRight().y = RandomFloat(-1.0f, 1.0f);
    matrix.GetRight().z = RandomFloat(-1.0f, 1.0f);
    matrix.GetRight().Normalise();

    matrix.GetForward().x = RandomFloat(-1.0f, 1.0f);
    matrix.GetForward().y = RandomFloat(-1.0f, 1.0f);
    matrix.GetForward().z = RandomFloat(-1.0f, 1.0f);
    matrix.GetForward().Normalise();

    matrix.GetUp().x = RandomFloat(-1.0f, 1.0f);
    matrix.GetUp().y = RandomFloat(-1.0f, 1.0f);
    matrix.GetUp().z = RandomFloat(-1.0f, 1.0f);
    matrix.GetUp().Normalise();

    matrix.pos.x = RandomFloat(-2000.0f, 2000.0f);
    matrix.pos.y = RandomFloat(-2000.0f, 2000.0f);
    matrix.pos.z = RandomFloat(-2000.0f, 2000.0f);
}

static void PrintMatrix(CMatrix &matrix) {
    printf("Right: %f %f %f\n", matrix.GetRight().x, matrix.GetRight().y, matrix.GetRight().z);
    printf("Forward: %f %f %f\n", matrix.GetForward().x, matrix.GetForward().y, matrix.GetForward().z);
    printf("Up: %f %f %f\n", matrix.GetUp().x, matrix.GetUp().y, matrix.GetUp().z);
    printf("Pos: %f %f %f\n", matrix.pos.x, matrix.pos.y, matrix.pos.z);
    printf("m_pAttachMatrix=%p m_bOwnsAttachedMatrix=%hhu\n", matrix.m_pAttachMatrix, matrix.m_bOwnsAttachedMatrix);
}

UTEST(CMatrix, ctor_CopyMatrix)
{
    auto copied = CMatrix();
    RandomData(&copied, sizeof(CMatrix));

    auto modern = CMatrix(copied);
    auto legacy = CMatrix();
    ((void (__thiscall *)(CMatrix *, CMatrix const&))0x59BCF0)(&legacy, copied);
    EXPECT_EQ(modern == legacy, true);

    // garbage/dummy values require manual cleanup
    copied.m_pAttachMatrix = nullptr;
    copied.m_bOwnsAttachedMatrix = false;
    modern.m_pAttachMatrix = nullptr;
    modern.m_bOwnsAttachedMatrix = false;
    legacy.m_pAttachMatrix = nullptr;
    legacy.m_bOwnsAttachedMatrix = false;
}

UTEST(CMatrix, ctor_CopyRwMatrixThenDelete)
{
    // need to figure out a way to correctly mock test DTOR without causing exception errors
    
    auto copied = RwMatrix();
    RandomData(&copied, sizeof(RwMatrix));

    {
        CMatrix modern = CMatrix(&copied, false);
        CMatrix legacy = CMatrix();
        ((void (__thiscall *)(CMatrix *, RwMatrix *, bool))0x59C050)(&legacy, &copied, false);
        EXPECT_EQ(modern == legacy, true);
        ((void (__thiscall *)(CMatrix *))0x59ACD0)(&legacy);
    }

    //{
    //    CMatrix modern = CMatrix(&copied, true);
    //    CMatrix legacy = CMatrix();
    //    ((void (__thiscall *)(CMatrix *, RwMatrix *, bool))0x59C050)(&legacy, &copied, true);
    //    EXPECT_EQ(modern == legacy, true);
    //    ((void (__thiscall *)(CMatrix *))0x59ACD0)(&legacy);
    //}
}

UTEST(CMatrix, AttachDetach)
{
    auto attached = RwMatrix();
    RandomData(&attached, sizeof(RwMatrix));

    auto modern = CMatrix();
    auto legacy = CMatrix();

    modern.Attach(&attached, false);
    ((void (__thiscall *)(CMatrix *, RwMatrix *, bool))0x59BD10)(&legacy, &attached, false);
    EXPECT_EQ(modern == legacy, true);
    modern.Detach();
    ((void (__thiscall *)(CMatrix *))0x59ACF0)(&legacy);
    EXPECT_EQ(modern == legacy, true);
    
    // need to figure out a way to correctly mock test this without causing exception errors
    //modern.Attach(&attached, true);
    //((void (__thiscall *)(CMatrix *, RwMatrix *, bool))0x59BD10)(&legacy, &attached, true);
    //EXPECT_EQ(modern == legacy, true);
    //modern.Detach();
    //((void (__thiscall *)(CMatrix *))0x59ACF0)(&legacy);
    //EXPECT_EQ(modern == legacy, true);
}

UTEST(CMatrix, CopyOnlyMatrix)
{
    auto copied = CMatrix();
    RandomData(&copied, sizeof(CMatrix));

    auto modern = CMatrix();
    auto legacy = CMatrix();

    modern.CopyOnlyMatrix(copied);
    ((void (__thiscall *)(CMatrix *, CMatrix const&))0x59ADD0)(&legacy, copied);
    EXPECT_EQ(modern == legacy, true);
    
    // garbage/dummy values require manual cleanup
    copied.m_pAttachMatrix = nullptr;
    modern.m_pAttachMatrix = nullptr;
    legacy.m_pAttachMatrix = nullptr;
    copied.m_bOwnsAttachedMatrix = false;
    modern.m_bOwnsAttachedMatrix = false;
    legacy.m_bOwnsAttachedMatrix = false;
}

UTEST(CMatrix, UpdateMatrix)
{
    auto copied = RwMatrix();
    auto attached = RwMatrix();
    RandomData(&copied, sizeof(RwMatrix));
    RandomData(&attached, sizeof(RwMatrix));

    auto modern = CMatrix(&attached, false);
    auto legacy = CMatrix();
    ((void (__thiscall *)(CMatrix *, RwMatrix *, bool))0x59BD10)(&legacy, &attached, false);

    modern.UpdateMatrix(&copied);
    plugin::CallMethod<0x59AD20, CMatrix*, RwMatrix*>(&legacy, &copied);
    EXPECT_EQ(modern == legacy, true);

    modern.Update();
    ((void (__thiscall *)(CMatrix *))0x59BB60)(&legacy);
    EXPECT_EQ(modern == legacy, true);
}

UTEST(CMatrix, UpdateRW)
{
    auto copied_modern = RwMatrix();
    auto copied_legacy = RwMatrix();
    auto attached_modern = RwMatrix();
    auto attached_legacy = RwMatrix();

    auto modern = CMatrix(&attached_modern, false);
    auto legacy = CMatrix(&attached_legacy, false);
    RandomData(&modern, sizeof(RwMatrix));
    memcpy(&legacy, &modern, sizeof(RwMatrix));
    
    modern.UpdateRW();
    ((void (__thiscall *)(CMatrix *))0x59BBB0)(&legacy);
    EXPECT_EQ(std::memcmp(&copied_modern, &copied_legacy, sizeof(RwMatrix)), 0);
    EXPECT_EQ(std::memcmp(&attached_modern, &attached_legacy, sizeof(RwMatrix)), 0);

    modern.UpdateRW(&copied_modern);
    ((void (__thiscall *)(CMatrix *, RwMatrix *))0x59AD70)(&legacy, &copied_legacy);
    EXPECT_EQ(std::memcmp(&copied_modern, &copied_legacy, sizeof(RwMatrix)), 0);
    EXPECT_EQ(std::memcmp(&attached_modern, &attached_legacy, sizeof(RwMatrix)), 0);
}

UTEST(CMatrix, ResetOrientation)
{
    auto modern = CMatrix();
    auto legacy = CMatrix();

    modern.ResetOrientation();
    ((void (__thiscall *)(CMatrix *))0x59AEA0)(&legacy);
    EXPECT_EQ(modern == legacy, true);
}

UTEST(CMatrix, SetUnity)
{
    auto modern = CMatrix();
    auto legacy = CMatrix();

    modern.SetUnity();
    ((void (__thiscall *)(CMatrix *))0x59AE70)(&legacy);
    EXPECT_EQ(modern == legacy, true);
}

UTEST(CMatrix, SetScale)
{
    auto modern = CMatrix();
    auto legacy = CMatrix();

    float x = 123.4f, y = 567.8f, z = 910.1f;

    modern.SetScale(13.0);
    ((void (__thiscall *)(CMatrix *, float))0x59AED0)(&legacy, 13.0);
    EXPECT_EQ(modern == legacy, true);

    modern.SetScale(x, y, z);
    ((void (__thiscall *)(CMatrix *, float, float, float))0x59AF00)(&legacy, x, y, z);
    EXPECT_EQ(modern == legacy, true);
}

UTEST(CMatrix, SetTranslateOnly)
{
    auto modern = CMatrix();
    auto legacy = CMatrix();
    
    float x = 123.4f, y = 567.8f, z = 910.1f;

    modern.SetTranslateOnly(x, y, z);
    ((void (__thiscall *)(CMatrix *, float, float, float))0x59AF80)(&legacy, x, y, z);
    EXPECT_EQ(modern == legacy, true);
}

UTEST(CMatrix, SetTranslate)
{
    auto modern = CMatrix();
    auto legacy = CMatrix();
    
    float x = 123.4f, y = 567.8f, z = 910.1f;

    modern.SetTranslate(x, y, z);
    ((void (__thiscall *)(CMatrix *, float, float, float))0x59AF40)(&legacy, x, y, z);
    EXPECT_EQ(IsMatrixApproximatelyEqual(modern, legacy), true);
}

UTEST(CMatrix, SetRotateXOnly)
{
    auto modern = CMatrix();
    auto legacy = CMatrix();

    float value = RandomFloat(-3.14f, 3.14f);
    modern.SetRotateXOnly(value);
    ((void (__thiscall *)(CMatrix *, float))0x59AFA0)(&legacy, value);
    EXPECT_EQ(modern == legacy, true);
}

UTEST(CMatrix, SetRotateYOnly)
{
    auto modern = CMatrix();
    auto legacy = CMatrix();

    float value = RandomFloat(-3.14f, 3.14f);
    modern.SetRotateYOnly(value);
    ((void (__thiscall *)(CMatrix *, float))0x59AFE0)(&legacy, value);
    EXPECT_EQ(modern == legacy, true);
}

UTEST(CMatrix, SetRotateZOnly)
{
    auto modern = CMatrix();
    auto legacy = CMatrix();

    float value = RandomFloat(-3.14f, 3.14f);
    modern.SetRotateZOnly(value);
    ((void (__thiscall *)(CMatrix *, float))0x59B020)(&legacy, value);
    EXPECT_EQ(modern == legacy, true);
}

UTEST(CMatrix, SetRotateX)
{
    auto modern = CMatrix();
    auto legacy = CMatrix();

    float value = RandomFloat(-3.14f, 3.14f);
    modern.SetRotateX(value);
    ((void (__thiscall *)(CMatrix *, float))0x59B060)(&legacy, value);
    EXPECT_EQ(modern == legacy, true);
}

UTEST(CMatrix, SetRotateY)
{
    auto modern = CMatrix();
    auto legacy = CMatrix();

    float value = RandomFloat(-3.14f, 3.14f);
    modern.SetRotateY(value);
    ((void (__thiscall *)(CMatrix *, float))0x59B0A0)(&legacy, value);
    EXPECT_EQ(modern == legacy, true);
}

UTEST(CMatrix, SetRotateZ)
{
    auto modern = CMatrix();
    auto legacy = CMatrix();

    float value = RandomFloat(-3.14f, 3.14f);
    modern.SetRotateZ(value);
    ((void (__thiscall *)(CMatrix *, float))0x59B0E0)(&legacy, value);
    EXPECT_EQ(modern == legacy, true);
}

UTEST(CMatrix, SetRotate_Vector)
{
    auto modern = CMatrix();
    auto legacy = CMatrix();

    float x = RandomFloat(-3.14f, 3.14f);
    float y = RandomFloat(-3.14f, 3.14f);
    float z = RandomFloat(-3.14f, 3.14f);
    modern.SetRotate(x, y, z);
    ((void (__thiscall *)(CMatrix *, float, float, float))0x59B120)(&legacy, x, y, z);
    EXPECT_EQ(IsMatrixApproximatelyEqual(modern, legacy), true);
}

UTEST(CMatrix, SetRotate_Quat)
{
    auto modern = CMatrix();
    auto legacy = CMatrix();

    CQuaternion quat = {{RandomFloat(-1.0f, 1.0f), RandomFloat(-1.0f, 1.0f), RandomFloat(-1.0f, 1.0f)}, RandomFloat(-1.0f, 1.0f)};
    modern.SetRotate(quat);
    ((void (__thiscall *)(CMatrix *, CQuaternion  const&))0x59BBF0)(&legacy, quat);
    EXPECT_EQ(IsMatrixApproximatelyEqual(modern, legacy), true);
}

UTEST(CMatrix, MultiplyRotation)
{
    auto modern = CMatrix();
    auto legacy = CMatrix();

    CVector vector = CVector(RandomFloat(-3.14f, 3.14f), RandomFloat(-3.14f, 3.14f), RandomFloat(-3.14f, 3.14f));
    modern.MultiplyRotation(vector);
    plugin::CallAndReturn<CVector, 0x59C790, CMatrix  const&, CVector  const&>(legacy, vector);
    EXPECT_EQ(modern == legacy, true);
}

UTEST(CMatrix, MultiplyTransposedRotation)
{
    auto modern = CMatrix();
    auto legacy = CMatrix();

    CVector vector = CVector(RandomFloat(-3.14f, 3.14f), RandomFloat(-3.14f, 3.14f), RandomFloat(-3.14f, 3.14f));
    modern.MultiplyTransposedRotation(vector);
    plugin::CallAndReturn<CVector, 0x59C810, CVector  const&, CMatrix  const&>(vector, legacy);
    EXPECT_EQ(modern == legacy, true);
}

UTEST(CMatrix, RotateX)
{
    auto modern = CMatrix();
    auto legacy = CMatrix();

    float value = RandomFloat(-3.14f, 3.14f);
    modern.RotateX(value);
    ((void (__thiscall *)(CMatrix *, float))0x59B1E0)(&legacy, value);
    EXPECT_EQ(modern == legacy, true);
}

UTEST(CMatrix, RotateY)
{
    auto modern = CMatrix();
    auto legacy = CMatrix();

    float value = RandomFloat(-3.14f, 3.14f);
    modern.RotateY(value);
    ((void (__thiscall *)(CMatrix *, float))0x59B2C0)(&legacy, value);
    EXPECT_EQ(modern == legacy, true);
}

UTEST(CMatrix, RotateZ)
{
    auto modern = CMatrix();
    auto legacy = CMatrix();

    float value = RandomFloat(-3.14f, 3.14f);
    modern.RotateZ(value);
    ((void (__thiscall *)(CMatrix *, float))0x59B390)(&legacy, value);
    EXPECT_EQ(modern == legacy, true);
}

UTEST(CMatrix, Rotate)
{
    auto modern = CMatrix();
    auto legacy = CMatrix();

    float x = RandomFloat(-3.14f, 3.14f);
    float y = RandomFloat(-3.14f, 3.14f);
    float z = RandomFloat(-3.14f, 3.14f);
    modern.Rotate(x, y, z);
    ((void (__thiscall *)(CMatrix *, float, float, float))0x59B460)(&legacy, x, y, z);
    EXPECT_EQ(modern == legacy, true);
}

UTEST(CMatrix, Reorthogonalise)
{
    auto modern = CMatrix();
    auto legacy = CMatrix();

    RandomizeComponents(modern);
    memcpy(&legacy, &modern, sizeof(CMatrix));

    modern.Reorthogonalise();
    ((void (__thiscall *)(CMatrix *))0x59B6A0)(&legacy);
    EXPECT_EQ(IsMatrixApproximatelyEqual(modern, legacy), true);
}

UTEST(CMatrix, CopyToRwMatrix)
{
    auto copied_modern = RwMatrix();
    auto copied_legacy = RwMatrix();

    auto modern = CMatrix();
    auto legacy = CMatrix();
    RandomData(&modern, sizeof(RwMatrix));
    memcpy(&legacy, &modern, sizeof(RwMatrix));

    modern.CopyToRwMatrix(&copied_modern);
    ((void (__thiscall *)(CMatrix *, RwMatrix *))0x59B8B0)(&legacy, &copied_legacy);
    EXPECT_EQ(std::memcmp(&copied_modern, &copied_legacy, sizeof(RwMatrix)), 0);
}

UTEST(CMatrix, Scale)
{
    auto modern = CMatrix();
    auto legacy = CMatrix();

    RandomizeComponents(modern);
    memcpy(&legacy, &modern, sizeof(CMatrix));
    
    const float x = RandomFloat(2.0, 100.0f);
    const float y = RandomFloat(2.0, 100.0f);
    const float z = RandomFloat(2.0, 100.0f);

    modern.Scale(x, y, z);
    plugin::CallMethod<0x5A2E60, CMatrix *, float, float, float>(&legacy, x, y, z);
    EXPECT_EQ(IsMatrixApproximatelyEqual(modern, legacy), true);

    modern.Scale(x);
    plugin::CallMethod<0x459350, CMatrix *, float>(&legacy, x);
    EXPECT_EQ(IsMatrixApproximatelyEqual(modern, legacy), true);
}

UTEST(CMatrix, ForceUpVector)
{
    auto modern = CMatrix();
    auto legacy = CMatrix();

    RandomizeComponents(modern);
    memcpy(&legacy, &modern, sizeof(CMatrix));
    
    CVector vector = {RandomFloat(2.0, 100.0f), RandomFloat(2.0, 100.0f), RandomFloat(2.0, 100.0f)};

    modern.ForceUpVector(vector);
    plugin::CallMethod<0x59B7E0, CMatrix *, CVector>(&legacy, vector);
    EXPECT_EQ(IsMatrixApproximatelyEqual(modern, legacy), true);
}

UTEST(CMatrix, ConvertToEulerAngles)
{
    auto modern = CMatrix();
    auto legacy = CMatrix();

    RandomizeComponents(modern);
    memcpy(&legacy, &modern, sizeof(CMatrix));
    
    float modern_initial, modern_intermediate, modern_final;
    float legacy_initial, legacy_intermediate, legacy_final;
    for (unsigned char flags = 0; flags < 24; flags++) {
        modern.ConvertToEulerAngles(modern_initial, modern_intermediate, modern_final, static_cast<CMatrix::eMatrixEulerFlags>(flags));
        plugin::CallMethod<0x59A840, const CMatrix*, float*, float*, float*, CMatrix::eMatrixEulerFlags>(
            &legacy, &legacy_initial, &legacy_intermediate, &legacy_final, static_cast<CMatrix::eMatrixEulerFlags>(flags));
        EXPECT_EQ(IsFloatApproximatelyEqual(modern_initial, legacy_initial), true);
        EXPECT_EQ(IsFloatApproximatelyEqual(modern_intermediate, legacy_intermediate), true);
        EXPECT_EQ(IsFloatApproximatelyEqual(modern_final, legacy_final), true);
    }
}

UTEST(CMatrix, ConvertFromEulerAngles)
{
    auto modern = CMatrix();
    auto legacy = CMatrix();
    
    float initialAngle = RandomFloat(-3.14f, 3.14f);
    float intermediateAngle = RandomFloat(-3.14f, 3.14f);
    float finalAngle = RandomFloat(-3.14f, 3.14f);

    for (unsigned char flags = 0; flags < 24; flags++) {
        modern.ConvertFromEulerAngles(initialAngle, intermediateAngle, finalAngle, static_cast<CMatrix::eMatrixEulerFlags>(flags));
        plugin::CallMethod<0x59AA40, CMatrix*, float, float, float, CMatrix::eMatrixEulerFlags>(
            &legacy, initialAngle, intermediateAngle, finalAngle, static_cast<CMatrix::eMatrixEulerFlags>(flags));
        EXPECT_EQ(IsMatrixApproximatelyEqual(modern, legacy), true);
    }
}

UTEST(CMatrix, StoreOperator)
{
    auto copied = CMatrix();
    RandomData(&copied, sizeof(CMatrix));

    auto modern = CMatrix();
    auto legacy = CMatrix();
    
    modern = copied;
    ((void (__thiscall *)(CMatrix *, CMatrix const&))0x59BBC0)(&legacy, copied);
    EXPECT_EQ(modern == legacy, true);

    // garbage/dummy values require manual cleanup
    copied.m_pAttachMatrix = nullptr;
    copied.m_bOwnsAttachedMatrix = false;
    modern.m_pAttachMatrix = nullptr;
    modern.m_bOwnsAttachedMatrix = false;
    legacy.m_pAttachMatrix = nullptr;
    legacy.m_bOwnsAttachedMatrix = false;
}

UTEST(CMatrix, AddEqualsOperator)
{
    auto copied = CMatrix();
    RandomizeComponents(copied);

    auto modern = CMatrix();
    auto legacy = CMatrix();
    RandomizeComponents(modern);
    memcpy(&legacy, &modern, sizeof(CMatrix));
    
    modern += copied;
    ((void (__thiscall *)(CMatrix *, CMatrix const&))0x59ADF0)(&legacy, copied);
    EXPECT_EQ(IsMatrixApproximatelyEqual(modern, legacy), true);

    // garbage/dummy values require manual cleanup
    copied.m_pAttachMatrix = nullptr;
    copied.m_bOwnsAttachedMatrix = false;
    modern.m_pAttachMatrix = nullptr;
    modern.m_bOwnsAttachedMatrix = false;
    legacy.m_pAttachMatrix = nullptr;
    legacy.m_bOwnsAttachedMatrix = false;
}

UTEST(CMatrix, MultiplyEqualsOperator)
{
    auto copied = CMatrix();
    RandomizeComponents(copied);

    auto modern = CMatrix();
    auto legacy = CMatrix();
    RandomizeComponents(modern);
    memcpy(&legacy, &modern, sizeof(CMatrix));
    
    modern *= copied;
    ((void (__thiscall *)(CMatrix *, CMatrix const&))0x411A80)(&legacy, copied);
    EXPECT_EQ(IsMatrixApproximatelyEqual(modern, legacy), true);

    // garbage/dummy values require manual cleanup
    copied.m_pAttachMatrix = nullptr;
    copied.m_bOwnsAttachedMatrix = false;
    modern.m_pAttachMatrix = nullptr;
    modern.m_bOwnsAttachedMatrix = false;
    legacy.m_pAttachMatrix = nullptr;
    legacy.m_bOwnsAttachedMatrix = false;
}

UTEST(CMatrix, MultiplyMatrixOperator)
{
    auto matrix1 = CMatrix();
    auto matrix2 = CMatrix();
    RandomizeComponents(matrix1);
    RandomizeComponents(matrix2);

    auto modern = CMatrix();
    auto legacy = CMatrix();
    
    modern = matrix1 * matrix2;
    ((void(__cdecl *)(CMatrix*, CMatrix const&, CMatrix const&))0x59BE30)(&legacy, matrix1, matrix2);
    EXPECT_EQ(IsMatrixApproximatelyEqual(modern, legacy), true);

    // garbage/dummy values require manual cleanup
    matrix1.m_pAttachMatrix = nullptr;
    matrix1.m_bOwnsAttachedMatrix = false;
    matrix2.m_pAttachMatrix = nullptr;
    matrix2.m_bOwnsAttachedMatrix = false;
    modern.m_pAttachMatrix = nullptr;
    modern.m_bOwnsAttachedMatrix = false;
    legacy.m_pAttachMatrix = nullptr;
    legacy.m_bOwnsAttachedMatrix = false;
}

UTEST(CMatrix, MultiplyVectorOperator)
{
    auto matrix = CMatrix();
    CVector vector = {RandomFloat(2.0, 100.0f), RandomFloat(2.0, 100.0f), RandomFloat(2.0, 100.0f)};
    RandomizeComponents(matrix);
    
    CVector modern = matrix * vector;
    CVector legacy = {0.0f, 0.0f, 0.0f};
    ((void(__cdecl *)(CVector*, CMatrix const&, CVector const&))0x59C890)(&legacy, matrix, vector);
    EXPECT_EQ(IsVectorApproximatelyEqual(modern, legacy), true);
}

UTEST(CMatrix, AddMatrixOperator)
{
    auto matrix1 = CMatrix();
    auto matrix2 = CMatrix();
    RandomizeComponents(matrix1);
    RandomizeComponents(matrix2);

    auto modern = CMatrix();
    auto legacy = CMatrix();
    
    modern = matrix1 + matrix2;
    ((void(__cdecl *)(CMatrix*, CMatrix const&, CMatrix const&))0x59BFA0)(&legacy, matrix1, matrix2);
    EXPECT_EQ(IsMatrixApproximatelyEqual(modern, legacy), true);

    // garbage/dummy values require manual cleanup
    matrix1.m_pAttachMatrix = nullptr;
    matrix1.m_bOwnsAttachedMatrix = false;
    matrix2.m_pAttachMatrix = nullptr;
    matrix2.m_bOwnsAttachedMatrix = false;
    modern.m_pAttachMatrix = nullptr;
    modern.m_bOwnsAttachedMatrix = false;
    legacy.m_pAttachMatrix = nullptr;
    legacy.m_bOwnsAttachedMatrix = false;
}
