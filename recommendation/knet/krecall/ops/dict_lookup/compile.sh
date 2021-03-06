#! /usr/bin/env bash

TF_CFLAGS=( $(python -c 'import tensorflow as tf; print(" ".join(tf.sysconfig.get_compile_flags()))') )
TF_LFLAGS=( $(python -c 'import tensorflow as tf; print(" ".join(tf.sysconfig.get_link_flags()))') )
echo "TF_CFLAGS = " ${TF_CFLAGS[@]}
echo "TF_LFLAGS = " ${TF_LFLAGS[@]}
echo "compile dict_lookup_ops.so ..."

g++ -std=c++11 -shared \
    dict_lookup_ops.cc \
    dict_lookup_kernels.cc \
    -o dict_lookup_ops.so \
    -fPIC ${TF_CFLAGS[@]} ${TF_LFLAGS[@]} -O2
