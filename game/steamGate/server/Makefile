

MG_PATH = ../../../..

RANDOM = ${MG_PATH}/minorGems/crypto/cryptoRandom
RANDOM_CPP = ${RANDOM}.cpp
RANDOM_H = ${RANDOM}.h

CURVE = ${MG_PATH}/minorGems/crypto/keyExchange/curve25519
CURVE_CPP = ${CURVE}.cpp
CURVE_H = ${CURVE}.h

ENCODING = ${MG_PATH}/minorGems/formats/encodingUtils
ENCODING_CPP = ${ENCODING}.cpp
ENCODING_H = ${ENCODING}.h


DEPENDS = ${RANDOM_H} ${RANDOM_CPP} ${CURVE_H} ${CURVE_CPP} \
${ENCODING_H} ${ENCODING_CPP}


curve25519GenKeys: curve25519GenKeys.cpp ${DEPENDS}
	g++ -I${MG_PATH} -o curve25519GenKeys curve25519GenKeys.cpp ${RANDOM_CPP} ${CURVE_CPP} ${ENCODING_CPP}