#include "CurvePoint.h"
#include "FakeZZ.h"
NTL_CLIENT

// a proof that the verifier knows alpha given g^alpha = (some) x
// requires alpha, g (implicitly generator of CurvePoint), and parameterized randomness w (implicitly generated)
// outputs nizk proof (a, r) with a = g^w and r = hash(g, a) * alpha + w
class SchnorrProof {
public:
  static int size;

  SchnorrProof(const ZZ& alpha);
  SchnorrProof(const ZZ& alpha, CurvePoint& K);
  SchnorrProof(const char *serialized);

  // input: ciphertext this proof was constructed from (x = g^alpha)
  // output: returns 1 if the proof succeeds and 0 on failure
  int verify(const CurvePoint& x);
  int verify_dec(const CurvePoint& K, const CurvePoint& y);

  void serialize(char *output); // 128 (a) + 32 (a serialized) + 32 (r) bytes
  static const int bytesize = 32*7;

private:
  ZZ fiat_shamir_a();
  ZZ fiat_shamir_b();

  CurvePoint a;
  CurvePoint b;
  CurvePoint z;
  ZZ r1;
  ZZ r2;
  // computed on initialization if not supplied: need it anyway to compute Fiat-Shamir (verify critical path)
  char a_canonical[32];
  char b_canonical[32];
};
