#! /usr/bin/env bash

set -e

MYDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# speedup 2~5x
# https://github.com/facebookresearch/faiss/wiki/Troubleshooting
# https://github.com/facebookresearch/faiss/issues/53
export OMP_WAIT_POLICY=PASSIVE

# for deployment
# use the specified libblas.so and liblapack.so in this directory
# because it's almost 5x faster than the machine's own ones
export LD_LIBRARY_PATH="${MYDIR}/bin:$LD_LIBRARY_PATH"

${MYDIR}/bin/do_kmeans $@
