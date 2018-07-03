#! /usr/bin/env bash

set -e

MYDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd ${MYDIR}

# add Anaconda2
export ANACONDA2_ROOT=/usr/local/services/kd_anaconda2-1.0/lib/anaconda2
export PATH="${ANACONDA2_ROOT}/bin:$PATH"
export PYTHONPATH="${ANACONDA2_ROOT}/lib/python2.7/site-packages:$PYTHONPATH"

raw_data_dir=raw_data
model_dir=`pwd`/video_tab/model_dir
export_model_dir=`pwd`/video_tab/export_model_dir
dict_dir=`pwd`/video_tab/dict_dir
train_data_path=${raw_data_dir}/train_data.vt.in
eval_data_path=${raw_data_dir}/eval_data.vt.in

lr=0.5
embedding_dim=128
train_ws=20
train_lower_ws=1
min_count=100
t=0.0001
batch_size=128
num_sampled=10
epoch=1
hidden_units=""
prefetch_size=100000
max_train_steps=-1
save_summary_steps=100000000
save_checkpoints_secs=3600
log_step_count_steps=20000
recall_k=350
use_saved_dict=0
use_profile_hook=0
profile_steps=100000
root_ops_path=lib/
remove_model_dir=1
optimize_level=1
receive_ws=100
use_subset=1
dropout=0.0
ntargets=1
chief_lock=${model_dir}/chief.lock
max_distribute_train_steps=-1
train_nce_biases=0
shuffle_batch=1
predict_ws=50
sample_dropout=0.0
optimizer_type='ada'
tfrecord_file=${raw_data_dir}/train_data.vt.tfrecord
num_tfrecord_file=42
train_data_format='fasttext'  # 'tfrecord', 'fasttext'
tfrecord_map_num_parallel_calls=2
train_parallel_mode='train_op_parallel' # 'default', 'train_op_parallel'
num_train_op_parallel=8
use_batch_normalization=0
sgd_lr_decay_type='fasttext_decay'  # 'exponential_decay', 'fasttext_decay'
sgd_lr_decay_steps=100
sgd_lr_decay_rate=0.99
use_clip_gradients=0
clip_norm=500.0

if [[ ${train_data_format} == 'tfrecord' ]]; then
    dump_tfrecord_is_delete=1
    echo 'dump tfrecord ...'
    export LD_LIBRARY_PATH=./lib/:$LD_LIBRARY_PATH  # libtensorflow_framework.so

    ./utils/tfrecord_writer \
        --tfrecord_file ${tfrecord_file} \
        --ws ${train_ws} \
        --lower_ws ${train_lower_ws} \
        --min_count ${min_count} \
        -t ${t} \
        --ntargets ${ntargets} \
        --sample_dropout ${sample_dropout} \
        --train_data_path ${train_data_path} \
        --dict_dir ${dict_dir} \
        --threads ${num_tfrecord_file} \
        --is_delete ${dump_tfrecord_is_delete} \
        --use_saved_dict ${use_saved_dict}
    echo 'dump tfrecord OK'
fi

python main.py \
    --train_data_path ${train_data_path} \
    --eval_data_path ${eval_data_path} \
    --lr ${lr} \
    --embedding_dim ${embedding_dim} \
    --train_ws ${train_ws} \
    --train_lower_ws ${train_lower_ws} \
    --min_count ${min_count} \
    --t ${t} \
    --verbose 2 \
    --min_count_label 50 \
    --label "__label__" \
    --batch_size ${batch_size} \
    --num_sampled ${num_sampled} \
    --epoch ${epoch} \
    --hidden_units "${hidden_units}" \
    --model_dir ${model_dir} \
    --export_model_dir ${export_model_dir} \
    --prefetch_size ${prefetch_size} \
    --max_train_steps ${max_train_steps} \
    --save_summary_steps ${save_summary_steps} \
    --save_checkpoints_secs ${save_checkpoints_secs} \
    --keep_checkpoint_max 2 \
    --log_step_count_steps ${log_step_count_steps} \
    --recall_k ${recall_k} \
    --dict_dir ${dict_dir} \
    --use_saved_dict ${use_saved_dict} \
    --use_profile_hook ${use_profile_hook} \
    --profile_steps ${profile_steps} \
    --root_ops_path ${root_ops_path} \
    --remove_model_dir ${remove_model_dir} \
    --optimize_level ${optimize_level} \
    --receive_ws ${receive_ws} \
    --use_subset ${use_subset} \
    --dropout ${dropout} \
    --ntargets ${ntargets} \
    --chief_lock ${chief_lock} \
    --max_distribute_train_steps ${max_distribute_train_steps} \
    --train_nce_biases ${train_nce_biases} \
    --shuffle_batch ${shuffle_batch} \
    --predict_ws ${predict_ws} \
    --sample_dropout ${sample_dropout} \
    --optimizer_type ${optimizer_type} \
    --tfrecord_file ${tfrecord_file} \
    --num_tfrecord_file ${num_tfrecord_file} \
    --train_data_format ${train_data_format} \
    --tfrecord_map_num_parallel_calls ${tfrecord_map_num_parallel_calls} \
    --train_parallel_mode ${train_parallel_mode} \
    --num_train_op_parallel ${num_train_op_parallel} \
    --use_batch_normalization ${use_batch_normalization} \
    --sgd_lr_decay_type ${sgd_lr_decay_type} \
    --sgd_lr_decay_steps ${sgd_lr_decay_steps} \
    --sgd_lr_decay_rate ${sgd_lr_decay_rate} \
    --use_clip_gradients ${use_clip_gradients} \
    --clip_norm ${clip_norm}
