#include "SchnorrProof.h"

#include "CurvePoint.h"
#include "FakeZZ.h"
#include "Functions.h"
#include <string.h>
#include <openssl/sha.h>
NTL_CLIENT

const ZZ ord = ZZ(NTL::conv<NTL::ZZ>("7237005577332262213973186563042994240857116359379907606001950938285454250989"));

// TODO: small proof sizes are inefficient (serialized and deserialized twice)
ZZ SchnorrProof::fiat_shamir_a() {
  // this can be preprocessed in SHA but probably cheap
  char temp[CurvePoint::bytesize + 32];
  CurvePoint bp = curve_basepoint();
  bp.serialize(temp); // don't need canonical repr: basepoint is a constant

  memcpy(&temp[CurvePoint::bytesize], this->a_canonical, 32);
  string s = string(temp, CurvePoint::bytesize+32);
 
  unsigned char hash[SHA256_DIGEST_LENGTH];
  Functions::sha256(s, hash);

  ZZ c;
  ZZFromBytes(c, hash, 32);

  return c % ord;
}

ZZ SchnorrProof::fiat_shamir_b() {
  // this can be preprocessed in SHA but probably cheap
  char temp[CurvePoint::bytesize + 32];
  CurvePoint bp = curve_basepoint();
  bp.serialize(temp); // don't need canonical repr: basepoint is a constant

  memcpy(&temp[CurvePoint::bytesize], this->b_canonical, 32);
  string s = string(temp, CurvePoint::bytesize+32);
 
  unsigned char hash[SHA256_DIGEST_LENGTH];
  Functions::sha256(s, hash);

  ZZ c;
  ZZFromBytes(c, hash, 32);

  return c % ord;
}



// prove and serialize
SchnorrProof::SchnorrProof(const ZZ& alpha) {
/*  ZZ c, w;

  w = RandomBnd(ord);

  //make a and b
  basepoint_scalarmult(this->a, w);
  this->a.serialize_canonical(this->a_canonical);

  //c = fiat_shamir();
  c = 5;

  ZZ t1;
  MulMod(t1, alpha, c, ord);
  AddMod(this->r, t1, w, ord);
*/
}


//david: decryption proof creator
SchnorrProof::SchnorrProof(const ZZ& alpha, CurvePoint& K) {
  ZZ c1, c2, w;

  w = RandomBnd(ord);

  //compute a and b
  basepoint_scalarmult(this->a, w);
  PowerMod(this->b, K, w, ZZ(0));
  this->a.serialize_canonical(this->a_canonical);
  this->b.serialize_canonical(this->b_canonical);

  //compute challenge
  c1 = fiat_shamir_a();
  c2 = fiat_shamir_b();

  //compute r
  ZZ t;
  MulMod(t, alpha, c1, ord);
  AddMod(this->r1, t, w, ord);
  MulMod(t, alpha, c2, ord);
  AddMod(this->r2, t, w, ord);

  //compute z 
  PowerMod(this->z, K, alpha, ZZ(0)); 

  //uint8_t n1[32], n2[32], n3[32];
  //edgamal_compress_point(n1, &(this->a).P);
  //edgamal_compress_point(n2, &(this->b).P);
  //edgamal_compress_point(n3, &(this->z).P);
  //cout << "prove " << +(n1[0]) << " " << +(n2[0]) << " " << +(n3[0]) << endl;
}

void SchnorrProof::serialize(char *output) {
  unsigned char* o = (unsigned char*) output;
  this->a.serialize(&output[32*0]);
  this->b.serialize(&output[32*1]);
  this->z.serialize(&output[32*2]);
  BytesFromZZ(&o[32*3], this->r1, 32);
  BytesFromZZ(&o[32*4], this->r2, 32);
  memcpy(&output[32*5], this->a_canonical, 32);
  memcpy(&output[32*6], this->b_canonical, 32);
}

// deserialize and verify

SchnorrProof::SchnorrProof(const char *serialized) {
  unsigned char* s = (unsigned char*) serialized;
  this->a.deserialize(&serialized[32*0]);
  this->b.deserialize(&serialized[32*1]);
  this->z.deserialize(&serialized[32*2]);
  ZZFromBytes(this->r1, &s[32*3], 32);
  ZZFromBytes(this->r2, &s[32*4], 32);
  memcpy(this->a_canonical, &serialized[32*5], 32);
  memcpy(this->b_canonical, &serialized[32*6], 32);
}

// given x = g^alpha, g, and proof (r, a)
int SchnorrProof::verify(const CurvePoint& x) {
/*  ZZ c;
  //c = fiat_shamir();
  c = 5;

  CurvePoint ta0, tg, ta;
  basepoint_scalarmult(tg, this->r);
  PowerMod(ta0, x, c, ZZ(0)); // these don't work with fake ZZs
  MulMod(ta, this->a, ta0, ZZ(0));

  return ta == tg;
*/
}

//david: verify decryption proof
int SchnorrProof::verify_dec(const CurvePoint& K, const CurvePoint& y) {

  // don't delete the below 5 lines; keep as a model for printing CurvePoints
  //uint8_t n1[32], n2[32], n3[32];
  //edgamal_compress_point(n1, &(this->a).P);
  //edgamal_compress_point(n2, &(this->b).P);
  //edgamal_compress_point(n3, &(this->z).P);
  //cout << "verify " << +(n1[0]) << " " << +(n2[0]) << " " << +(n3[0]) << endl;

  ZZ c1, c2;
  c1 = fiat_shamir_a();
  c2 = fiat_shamir_b();

  CurvePoint ta0, tg, ta;
  basepoint_scalarmult(tg, this->r1);
  PowerMod(ta0, y, c1, ZZ(0)); // these don't work with fake ZZs
  MulMod(ta, this->a, ta0, ZZ(0));

  CurvePoint sa0, sg, sa;
  PowerMod(sg, K, this->r1, ZZ(0));
  PowerMod(sa0, this->z, c1, ZZ(0)); // these don't work with fake ZZs
  MulMod(sa, this->b, sa0, ZZ(0));

  CurvePoint ua0, ug, ua;
  basepoint_scalarmult(ug, this->r2);
  PowerMod(ua0, y, c2, ZZ(0)); // these don't work with fake ZZs
  MulMod(ua, this->a, ua0, ZZ(0));

  CurvePoint va0, vg, va;
  PowerMod(vg, K, this->r2, ZZ(0));
  PowerMod(va0, this->z, c2, ZZ(0)); // these don't work with fake ZZs
  MulMod(va, this->b, va0, ZZ(0));


  return (ta == tg && sa == sg && ua == ug && va == vg);
}




