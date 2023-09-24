#!/usr/bin/env bash

WORKING_DIR="."
OUTPUT_DIR="${WORKING_DIR}/generated"

HEADER="${OUTPUT_DIR}/gen_shaders.h"
HEADER_TEMP="${HEADER}.temp"
SOURCE="${OUTPUT_DIR}/gen_shaders.cpp"
SHADER_DIR="${WORKING_DIR}/res/shaders"

process_folder() {
	local DIR=$1
	local PREFIX=${DIR#$SHADER_DIR}
	local PREFIX=${PREFIX#/}

	for f in "$DIR"/*
	do
		if [[ -d $f ]]; then
			process_folder $f
		else
			PROC_FILE_NAME=${f##*/} # remove pathing of file
			PROC_FILE_NAME_NO_EXT=${PROC_FILE_NAME%\.*}
			echo "processing $f"
			cat >> ${HEADER_TEMP} <<- EOM
			/* generated from ${PREFIX}${PREFIX:+/}${PROC_FILE_NAME} */
			extern shader_t ${PREFIX}${PREFIX:+_}${PROC_FILE_NAME_NO_EXT};

			EOM

			cat >> ${SOURCE} <<- EOM
			shader_t ${PROC_FILE_NAME_NO_EXT} = R"(
				$(sed 's/^/\t/' ${f})
			)";
			
			EOM
		fi
	done
}

mkdir -p ${OUTPUT_DIR}
touch "${HEADER_TEMP}"
touch "${SOURCE}"

cat > "${HEADER_TEMP}" <<- EOM
#ifndef __GEN_SHADERS_H__
#define __GEN_SHADERS_H__

/* File generated by $(basename "$0") */

#ifdef __cplusplus
extern "C" {
#endif

typedef const char* const shader_t;

EOM

cat > "${SOURCE}" <<- EOM
/* File generated by $(basename "$0") */

#include "${HEADER##*/}"

#ifdef __cplusplus
extern "C" {
#endif

EOM


process_folder "${SHADER_DIR}"

cat >> "${HEADER_TEMP}" <<-EOM
#ifdef __cplusplus
}
#endif

#endif /* __GEN_SHADERS_H__ */
EOM

cat >> "${SOURCE}" <<-EOM

#ifdef __cplusplus
}
#endif
EOM

if cmp -s ${HEADER} ${HEADER_TEMP}; then
	rm ${HEADER_TEMP}
else
	mv ${HEADER_TEMP} ${HEADER}
fi
