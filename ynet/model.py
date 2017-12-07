#! /usr/bin/env python
# -*-coding:utf-8 -*-

import tensorflow as tf
from input_data_binary import load_video_embeddings_from_binary


def model_fn(features, labels, mode, params):
    # loading pretrained word vectors
    video_embeddings, num_videos, embedding_dim = \
        load_video_embeddings_from_binary(params["embeddings_file_path"])
    video_biases = tf.Variable(tf.zeros([num_videos]))

    x = tf.gather(video_embeddings, features["watched"])
    mean_input = tf.reduce_mean(x, 1)

    keep_prob = params["keep_prob"]
    """Model function for Estimator."""
    # Connect the first hedden layer to input layer
    # (features["watched"]) with relu activation
    first_hidden_layer = tf.layers.dense(
        inputs=mean_input,
        units=2048,
        activation=tf.nn.relu,
        kernel_initializer=tf.truncated_normal_initializer(0, 1),
        kernel_regularizer=tf.contrib.layers.l2_regularizer(0.1),
        name='fc1'
    )
    first_hidden_layer = tf.nn.dropout(first_hidden_layer, keep_prob)

    # Connect the second hidden layer to first hidden layer with relu
    second_hidden_layer = tf.layers.dense(
        inputs=first_hidden_layer,
        units=1024,
        activation=tf.nn.relu,
        kernel_initializer=tf.truncated_normal_initializer(0, 1),
        kernel_regularizer=tf.contrib.layers.l2_regularizer(0.1),
        name='fc2'
    )
    second_hidden_layer = tf.nn.dropout(second_hidden_layer, keep_prob)

    # Connect the third hidden layer to first hidden layer with relu
    third_hidden_layer = tf.layers.dense(
        inputs=second_hidden_layer,
        units=512,
        activation=tf.nn.relu,
        kernel_initializer=tf.truncated_normal_initializer(0, 1),
        kernel_regularizer=tf.contrib.layers.l2_regularizer(0.1),
        name='fc3'
    )
    third_hidden_layer = tf.nn.dropout(third_hidden_layer, keep_prob)

    output_layer = tf.layers.dense(
        inputs=third_hidden_layer,
        units=256,
        activation=tf.nn.relu,
        kernel_initializer=tf.truncated_normal_initializer(0, 1),
        kernel_regularizer=tf.contrib.layers.l2_regularizer(0.1),
        name='fc4'
    )

    # TODO Generate top-K predictions
    probs = tf.nn.bias_add(
        tf.matmul(output_layer, video_embeddings, transpose_b=True),
        video_biases
    )
    predictions = tf.argmax(probs)

    if mode == tf.estimator.ModeKeys.PREDICT:
        return tf.estimator.EstimatorSpec(
            mode=mode,
            predictions={"results": predictions}
        )

    # Calculate loss
    losses = tf.nn.nce_loss(
        weights=video_embeddings,
        biases=video_biases,
        labels=labels,
        inputs=output_layer,
        num_sampled=params["num_sampled"],
        num_classes=num_videos,
        name="nce_losses"
    )
    loss = tf.reduce_mean(losses, name="nce_loss_mean")

    optimizer = tf.train.GradientDescentOptimizer(
        learning_rate=params["learning_rate"])

    train_op = optimizer.minimize(
        loss=loss,
        global_step=tf.train.get_global_step(),
        name="train_op"
    )

    # Calculate root mean squared error as additional eval metric
    eval_metric_ops = {
        "rmse": tf.metrics.root_mean_squared_error(
            tf.cast(labels, tf.int64), predictions)
    }

    # Provide an estimator spec for 'ModeKeys.EVAL' and 'ModeKeys.TRAIN'
    return tf.estimator.EstimatorSpec(
        mode=mode,
        loss=loss,
        train_op=train_op,
        eval_metric_ops=eval_metric_ops
    )