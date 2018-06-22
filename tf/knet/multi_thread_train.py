#! /usr/bin/env python
# -*- coding=utf8 -*-

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function


import os
import tensorflow as tf

import build_model_fn
import model_keys
import input_data
import hook
import args_parser
from estimator.estimator import MultiThreadEstimator
from estimator.run_config import RunConfig

opts = None


def delete_dir(filename):
    tf.logging.info("To delete file '{}' ...".format(filename))
    if tf.gfile.Exists(filename):
        if tf.gfile.IsDirectory(filename):
            tf.logging.info("delete dir '{}' ...".format(filename))
            tf.gfile.DeleteRecursively(filename)
            tf.logging.info("delete dir '{}' OK".format(filename))
        else:
            raise Exception(
                "'{}' exists and not a directory.".format(filename))


def build_estimator():
    """Build estimator."""

    dict_meta = input_data.parse_dict_meta(opts)
    feature_columns, predict_feature_columns = input_data.feature_columns(opts)
    """ session config
    session_config = tf.ConfigProto(device_count={"CPU": 1},
                                    inter_op_parallelism_threads=1,
                                    intra_op_parallelism_threads=1,
                                    log_device_placement=False)
    """
    config = RunConfig(
        model_dir=opts.model_dir,
        tf_random_seed=None,
        save_summary_steps=opts.save_summary_steps,
        save_checkpoints_secs=opts.save_checkpoints_secs,
        session_config=None,
        keep_checkpoint_max=opts.keep_checkpoint_max,
        keep_checkpoint_every_n_hours=10000,
        log_step_count_steps=opts.log_step_count_steps)
    estimator = MultiThreadEstimator(
        model_fn=build_model_fn.knet_model_fn,
        config=config,
        params={
            'feature_columns': feature_columns,
            'predict_feature_columns': predict_feature_columns,
            'n_classes': dict_meta["nwords"] + 1,
            'ntokens': dict_meta["ntokens"] * opts.epoch,
            'opts': opts
        },
        threads=opts.threads)
    return estimator


def init_dictionary():
    if opts.remove_model_dir:
        tf.logging.info("Remove model dir ...")
        delete_dir(opts.model_dir)
        tf.logging.info("Remove model dir OK")
        os.makedirs(opts.model_dir)
    else:
        tf.logging.info("Don't remove model dir.")

    tf.logging.info('Init dict ...')
    input_data.init_dict(opts)
    tf.logging.info('Init dict OK')


def create_hooks():
    """Create profile hooks."""

    save_steps = opts.profile_steps
    meta_hook = hook.MetadataHook(save_steps=save_steps,
                                  output_dir=opts.model_dir)
    profile_hook = tf.train.ProfilerHook(save_steps=save_steps,
                                         output_dir=opts.model_dir,
                                         show_dataflow=True,
                                         show_memory=True)
    hooks = [meta_hook, profile_hook] if opts.use_profile_hook else None

    return hooks


def train_and_eval_in_local_mode():
    """Train and eval model in lcoal mode."""

    tf.logging.info("Beginning train model ...")

    opts.estimator.train(input_fn=lambda: input_data.train_input_fn(opts),
                         max_steps=opts.max_train_steps,
                         hooks=opts.hooks)
    tf.logging.info("Train model OK")

    # evaluate model
    tf.logging.info("Beginning evaluate model ...")
    opts.estimator.evaluate(input_fn=lambda: input_data.eval_input_fn(opts),
                            hooks=opts.hooks)
    tf.logging.info("Evaluate model OK")


def export_model_in_local_mode():
    """Export model in local mode."""

    if not os.path.exists(opts.dict_dir):
        os.mkdir(opts.dict_dir)

    tf.logging.info("Save nce weights and biases ...")
    build_model_fn.save_model_nce_params(opts.estimator, opts.dict_dir)
    tf.logging.info("Save nce weights and biases OK")

    if opts.use_subset:
        tf.logging.info("Save subset dict and nce params ...")
        build_model_fn.filter_and_save_subset(opts.dict_dir)
        tf.logging.info("Save subset dict and nce params OK")

    # export model
    tf.logging.info("Beginning export model ...")
    assets_dict_dir = os.path.basename(opts.dict_dir)
    dict_params = {}
    for name in model_keys.DICT_PARAM_NAMES:
        src = os.path.join(opts.dict_dir, name)
        dest = os.path.join(assets_dict_dir, name)
        dict_params[dest] = src

    assets_extra = dict_params

    opts.estimator.export_savedmodel(
        opts.export_model_dir,
        serving_input_receiver_fn=input_data.build_serving_input_fn(opts),
        assets_extra=assets_extra)
    tf.logging.info("Export model OK")


def main(argv):
    global opts

    opts = args_parser.parse(argv)

    init_dictionary()

    opts.estimator = build_estimator()
    opts.hooks = create_hooks()

    tf.logging.info(opts)

    train_and_eval_in_local_mode()
    export_model_in_local_mode()


if __name__ == '__main__':
    tf.logging.set_verbosity(tf.logging.INFO)
    tf.app.run(main)