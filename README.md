# Stadium

_A Distributed Metadata-Private Messaging System_

**SOSP Paper:**
Nirvan Tyagi, Yossi Gilad, Derek Leung, Matei Zaharia, and Nickolai Zeldovich. _Stadium: A Distributed Metadata-Private Messaging System_. SOSP 2017.

**ePrint:**
Nirvan Tyagi, Yossi Gilad, Derek Leung, Matei Zaharia, and Nickolai Zeldovich. _Stadium: A Distributed Metadata-Private Messaging System_. Cryptology ePrint Archive, Report 2016/943. http://eprint.iacr.org/2016/943. 2016.

The system consists of several main components. A participating stadium server must deploy a *shuffle server*, *shuffle client*, and the *stadium server* and *stadium coordinator*. 

This system also contains an optimized version of Stephanie Bayer and Jens Groth's verifiable shuffle.

**Bayer and Groth Verifiable Shuffles:**
Stephanie Bayer and Jens Groth. _Efficient zero-knowledge argument for correctness of a shuffle_. EUROCRYPT 2012.

## Notes on the verifiable shuffle

The original version of the verifiable shuffle is [here](https://github.com/derbear/verifiable-shuffle). Our modified version of the verified shuffle is [here](https://github.com/nirvantyagi/stadium/tree/master/groth) and mirrored [here](https://github.com/derbear/verifiable-shuffle/tree/stadium). 

We modified Bayer and Groth's verifiable shuffle, decreasing latency by more than an order of magnitude. We optimized the shuffle by applying the following improvements:

- Added OpenMP directives to optimize key operations, such as Brickell et al.'s multi-exponentiation routines.
- Replaced the use of integers with Moon and Langley's implementation of Bernstein's curve25519 group. (We avoid point compression and decompression in intermediary operations to improve speed.)
- Improved point serialization and deserialization with byte-level representations of the data.
- Taking into account different performance profile of curve25519, replaced some multi-exponentiation routines with naive version and tweaked multi-exponentiation window sizes. The bottleneck for the shuffle is currently in multi-exponentiation routines.
- Added some more small optimizations (e.g. powers of 2, reduce dynamic memory allocations, etc.)

## Installing Stadium dependencies

_This setup describes a deployment on an Ubuntu machine. Adapt commands as needed for another OS_

1. Install dependencies from `apt`.
sudo apt-get install make, g++, libssl-dev, libgmp3-dev, libboost-all-dev, libtool
2. Install NTL and add to library path. Do *not* install `libntl-dev` from `apt`! That version of the library is not thread-safe, which will cause mysterious crashes. Make sure to build it from source.
```shell
wget http://www.shoup.net/ntl/ntl-9.8.1.tar.gz
gunzip ntl-9.8.1.tar.gz
tar xf ntl-9.8.1.tar
cd ntl-9.8.1/src
./configure NTL_THREADS=on NTL_GMP_LIP=on NTL_THREAD_BOOST=on SHARED=on
make
sudo make install
export LD_LIBRARY_PATH=/usr/local/lib/:\$LD_LIBRARY_PATH
```

## Build Stadium binaries

1. Set up your go workspace with src, bin, and pkg directories as outlined [here](https://golang.org/doc/code.html)
2. Clone Stadium repository within workspace. `git clone https://github.com/nirvantyagi/stadium.git`
3. Build verifiable shuffle library and add to library path.
```shell 
cd stadium/groth
make clean; make lib
chmod +x libshuffle.so
sudo cp libshuffle.so /usr/local/lib/
```
4. Build Stadium binaries.
```shell
go install stadium/groth stadium/stadium stadium/coordinator stadium/server`
```

## Running a 3-server Stadium configuration
The following commands must be run from a directory that contains `stadium/groth/config`.
If you have your go workspace bin in bath, `cd stadium/groth` to run, else copy `stadium/groth/config` to whatever directory you will run the binaries from.
In 4 different terminals, run each of the following commands.
 
 ```shell
server -conf config/three-server.conf -id 0
server -conf config/three-server.conf -id 1
server -conf config/three-server.conf -id 2
coordinator -conf config/three-server.conf
 ```

## Benchmarking the shuffle

Steps for benchmarking the `groth` crypto library:

1. Follow the same instructions for _Installing Stadium dependencies_.

2. Build and run the benchmark.

Go to `stadium/groth/` and run `make test`. This will produce the test, which you can run with `./test`.

3. Benchmarking on AWS

Helper scripts you may find useful lie inside `aws/`. Note that you will need to configure your own AWS account and certificates to run the benchmarks.
