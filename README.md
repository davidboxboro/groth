# Verifiable Shuffle Experiments

A modified cryptographic library built on the Bayer–Groth verifiable shuffle and the Stadium messaging system's implementation.

Includes C++ shuffle and proof routines, Go bindings for encryption/decryption and verification, configurable permutations, and benchmark code.

## Build

The C++ library requires OpenMP, NTL, GMP, Boost, and OpenSSL. With those dependencies installed:

```bash
make -C groth lib
```

This builds `groth/libshuffle.so`. Go bindings and tests are in `groth/interface.go` and `groth/groth_test.go`; the inherited deployment code uses a GOPATH-based setup. Upstream notices and references are retained in the bundled implementation.
