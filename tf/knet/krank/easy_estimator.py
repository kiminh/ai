#! /usr/bin/env python
# -*- coding=utf8 -*-

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function


import tensorflow as tf
import os

from datetime import datetime
from tensorflow.python.eager import context


class EasyEstimator(tf.estimator.Estimator):
    def __init__(self, model_fn, model_dir=None, config=None, params=None,
                 warm_start_from=None):

        super(EasyEstimator, self).__init__(
            model_fn=model_fn, model_dir=model_dir, config=config,
            params=params, warm_start_from=warm_start_from)

    def easy_train(self, input_fn, steps=None, max_steps=None):
        with context.graph_mode():
            if (steps is not None) and (max_steps is not None):
                raise ValueError(
                    'Can not provide both steps and max_steps.')
            if steps is not None and steps <= 0:
                raise ValueError(
                    'Must specify steps > 0, given: {}'.format(steps))
            if max_steps is not None and max_steps <= 0:
                raise ValueError('Must specify max_steps > 0, given: {}'
                                 .format(max_steps))

            if max_steps is not None:
                start_step = _load_global_step_from_checkpoint_dir(
                    self._model_dir)
                if max_steps <= start_step:
                    tf.logging.info('Skipping training since max_steps '
                                    'has already saved.')
                    return self

            loss = self._easy_train_model(input_fn, steps, max_steps)
            tf.logging.info('Loss for final step: %s.', loss)
            return self

    def _easy_train_model(self, input_fn, steps, max_steps):
        with tf.Graph().as_default() as g, g.device(self._device_fn):
            tf.set_random_seed(self._config.tf_random_seed)
            global_step_tensor = tf.train.get_or_create_global_step(g)
            (features,
             labels) = self._easy_get_features_and_labels_from_input_fn(
                 input_fn, tf.estimator.ModeKeys.TRAIN)
            estimator_spec = self._easy_call_model_fn(
                features, labels, tf.estimator.ModeKeys.TRAIN)
            merged_summary = tf.summary.merge_all()
            summary_writer = tf.summary.FileWriter(self._model_dir)
            saver = tf.train.Saver(
                sharded=True,
                max_to_keep=self._config.keep_checkpoint_max,
                save_relative_paths=True)

            with tf.Session() as sess:
                sess.run(tf.global_variables_initializer())
                sess.run(self._iterator_initializer)
                summary_writer.add_graph(sess.graph)

                self._easy_maybe_restore_model(sess, saver)

                tf.logging.info('{} Start training ...'.format(datetime.now()))

                current_steps = 0
                while True:
                    try:
                        _, loss = sess.run([estimator_spec.train_op,
                                            estimator_spec.loss])
                        global_steps = sess.run(global_step_tensor)
                        current_steps += 1
                        if _check_stop(current_steps, global_steps,
                                       steps, max_steps):
                            break
                    except tf.errors.OutOfRangeError:
                        break

                    if self._check_global_steps(
                            global_steps, self._config.log_step_count_steps):
                        self._logging(sess, global_steps, loss)

                    if self._check_global_steps(
                            global_steps, self._config.save_checkpoints_steps):
                        self._save_ckpt(sess, global_steps, saver)

                    if self._check_global_steps(
                            global_steps, self._config.save_summary_steps):
                        self._save_summary(
                            sess, global_steps, summary_writer, merged_summary)

                self._logging(sess, global_steps, loss)
                self._save_ckpt(sess, global_steps, saver)
                summary_writer.close()

            return loss

    def _easy_get_features_and_labels_from_input_fn(self, input_fn, mode):
        result = self._easy_call_input_fn(input_fn, mode)

        return self._easy_parse_input_fn_result(result)

    def _easy_call_input_fn(self, input_fn, mode):
        with tf.device('/cpu:0'):
            return input_fn()

    def _easy_parse_input_fn_result(self, result):
        iterator = result.make_initializable_iterator()
        self._iterator_initializer = iterator.initializer
        data = iterator.get_next()

        if isinstance(data, (list, tuple)):
            if len(data) != 2:
                raise ValueError('input_fn should return (features, labels)'
                                 ' as a len 2 tuple.')
            return data[0], data[1]
        return data, None

    def _easy_call_model_fn(self, features, labels, mode):
        tf.logging.info('Calling model_fn.')
        model_fn_results = self._model_fn(
            features=features, labels=labels, params=self._params, mode=mode)
        tf.logging.info('Done calling model_fn.')

        if not isinstance(model_fn_results, tf.estimator.EstimatorSpec):
            raise ValueError('model_fn should return an EstimatorSpec.')

        return model_fn_results

    def _easy_maybe_restore_model(self, sess, saver):
        lastest_path = tf.train.latest_checkpoint(self._model_dir)
        if lastest_path is not None:
            tf.logging.info('restore model ckpt from {} ...'
                            .format(lastest_path))
            saver.restore(sess, lastest_path)

    def _logging(self, sess, global_steps, loss):
        tf.logging.info('{} global_steps = {}, loss = {}'
                        .format(datetime.now(), global_steps, loss))

    def _save_ckpt(self, sess, global_steps, saver):
        ckpt_name = os.path.join(self._model_dir,
                                 'model.ckpt-{}'.format(global_steps))
        ckpt_path = saver.save(sess, ckpt_name)
        tf.logging.info('{} Model ckpt saved at {}'
                        .format(datetime.now(), ckpt_path))

    def _save_summary(self, sess, global_steps, writer, merged_summary):
        merged_summary = sess.run(merged_summary)
        writer.add_summary(merged_summary, global_steps)
        writer.flush()
        tf.logging.info('{} Summary saved.'.format(datetime.now()))

    def _check_global_steps(self, global_steps, steps):
        return True if global_steps % steps == 0 else False


def _load_global_step_from_checkpoint_dir(checkpoint_dir):
    try:
        checkpoint_reader = tf.train.NewCheckpointReader(
            tf.train.latest_checkpoint(checkpoint_dir))
        return checkpoint_reader.get_tensor(tf.GraphKeys.GLOBAL_STEP)
    except Exception:
        return 0


def _check_stop(current_steps, global_steps, steps, max_steps):
    if steps is not None and current_steps >= steps:
        return True
    if max_steps is not None and global_steps >= max_steps:
        return True
    return False
